#include <stdio.h>
#include <libconfig.h>
#include <stdlib.h>
#include <time.h>
#include "structs.h"
#include "coord.h"
#include "physics.h"
#include "vecmath.h"
#include "rout.h"
#include "rk4.h"
#include "orbit.h"

struct config_t cfg;                //Config File
double beginTime;                   //In JD
float h;                            //Timestep
double Jd;                          //Current time in Julian Date 
double Met;                         //Current time in Mission Elapsed Time
int numberOfStages;                 //Total number of stages
double simulationRunTime;           //How long the simulation took in seconds

FILE *outBurn;
FILE *outCoast;
FILE *outKml;
FILE *outForce;
FILE *outSpent;

state launchState;                  //Position, time, etc at launch
Rocket_Stage *stages;               //The rocket
Rocket_Stage currentStage;          //The stage currently being simulated

void printHelp();
void printVersion();
void readConfigFile(char *filename);
Rocket_Stage run(Rocket_Stage stage);

/**
 * ToOrbit Sim
 */
int main(int argc, char **argv)
{
    char *file = "orbit.cfg";   //Default config file name
    int i;
    clock_t start, end;         //For seeing how long the simulation takes
    
    /* Read switches */
    /* Start at i = 1 to skip the command name. */
    for (i = 1; i < argc; i++) 
    {
	    /* Check for a switch (leading "-"). */
	    if (argv[i][0] == '-') 
	    {
	        /* Use the next character to decide what to do. */
	        switch (argv[i][1]) 
	        {
		        case 'c':   // set config file name
		            file = argv[i+1];
				    break;
				case 'h':   // print help
				    printHelp();
				    return 0;
				    break;
				case 'v' :  // version
				    printVersion();
				    return 0;
				    break;
		        default:	
		            fprintf(stderr, "Unknown switch %s\n", argv[i]);
	        }
	    }
    }
    
    /* Read the config file */
    /* reading the config file should populate all of the start time and 
     * rocket stucts, so we can use them below */
    readConfigFile(file);
    
    /* Output File handleing */
    
    // Try to open files
    outBurn = fopen("Output/out-burn.dat", "w");
    outCoast = fopen("Output/out-coast.dat", "w");
    outKml = fopen("Output/out.kml", "w");
    outForce = fopen("Output/out-force.dat", "w");
    outSpent = fopen("Output/out-spentStages.dat", "w");
    
    // See if it worked
    if (   outBurn == NULL 
        || outCoast == NULL 
        || outKml == NULL
        || outForce == NULL 
        || outSpent == NULL)
    {
        printf("File Handle error.");
        exit(1);
    }
    
    // Print Headers
    PrintHeader(outBurn);
    PrintHeader(outCoast);
    PrintHeader(outSpent);
    PrintKmlHeader(outKml);
    
    /* Begin Simulation */
    start = clock();
    
    // Set the time
    Met = 0;
    Jd = BeginTime();
    
    stages[0].initialState = LaunchState();
    
    /* Do it */
    for(i = 0; i < numberOfStages; i++)
    {
        stages[i] = run(stages[i]);
    
        if ((i + 1) < numberOfStages)
        {
            state nextStageInitialState = stages[i].separationState;
            stages[i + 1].initialState.s = nextStageInitialState.s;
            stages[i + 1].initialState.U = nextStageInitialState.U;
            stages[i + 1].initialState.a = LinearAcceleration(stages[i + 1].initialState, Met);
            stages[i + 1].initialState.met = nextStageInitialState.met;
            double backInTime = Met - nextStageInitialState.met;
            Met = nextStageInitialState.met;
            Jd = Jd - SecondsToDecDay(backInTime);
        }
        fprintf(outBurn, "\n");
        fprintf(outCoast, "\n");
        PrintSimResult(stages[i]);
    }
    
    /* Finished */
    end = clock();
    simulationRunTime = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    PrintHtmlResult(stages);
    MakePltFiles(stages[numberOfStages - 1].apogeeState);
    
    /* Close open files */
    // Print Footers
    PrintKmlFooter(outKml);
    
    // close file
    fclose(outBurn);
    fclose(outCoast);
    fclose(outKml);
    fclose(outSpent);
    
    /* Free memory */
    free(stages);
    
    /* exit */
    return 0;
}

/**
 * Handles the actual running of the program
 */
Rocket_Stage run(Rocket_Stage stage)
{   
    double simTime;
    double lastTime = 0;
    double currentAltitude, lastAltitude;
    double burnoutTime = 0;
    unsigned int mode, lastMode;
    int notLastStage = 1;
    int i;
    state currentState;
    state lastState;

    // Init
    currentState = stage.initialState;
    lastState = stage.initialState;
    lastAltitude = Altitude(currentState);
    lastMode = stage.mode;
    
    if (stage.description.stage >= (NumberOfStages() - 1))
    {
        notLastStage = 0;
    }
    
    /* Run the simulation until the stage hits the ground or it's taking too
     * long. whichever comes first. 
     */
    for (simTime = Met; simTime < 1000; simTime += h)
    {        
        // Evaluate current state
        currentAltitude = Altitude(currentState);
        if (stage.mode == BURNING && currentState.m.fuelMass < 0)
        {
            stage.mode = COASING;
        }
        mode = stage.mode;
        
        // If the rocket starts to decend, then we must have pased apogee
        ///TODO: this is, of course, not always true.
        ///TODO: also, I should interpolate this.
        if (lastAltitude < currentAltitude)
        {
            stage.apogeeState = lastState;
        }
        
        // If the stage is below the "ground"
        ///TODO: this should be interpolated
        if (currentAltitude < 0)
        {
            stage.splashdownState = lastState;
            break;
        }
        
        // As long as the stage motor is lit
        if (mode == BURNING)
        {
            currentState.m.fuelMass -= 0.1;
            PrintStateLine(outBurn, Jd, currentState);  
        }
        else
        {
            // If we just stoped burning
            if (lastMode == BURNING)
            {
                stage.burnoutState = currentState;
                burnoutTime = Met;
            }

            // Wait some time, then separate
            if (Met - burnoutTime > stage.description.stageDelay && mode < SEPARATED
                && notLastStage > 0)
            {
                stage.separationState = currentState;
                stage.mode = SEPARATED;
            }
            
            // Print
            if (stage.mode == SEPARATED)
                PrintStateLine(outSpent, Jd, currentState);
            else
                PrintStateLine(outCoast, Jd, currentState);
        }
      
        // Making a kml file with updates only ever one second
        if ( (Met - lastTime) > 1.0 )
        {
            PrintKmlLine(outKml, currentState);
            lastTime = Met;
        }
        
        lastState = currentState;                       //LastRocket
        lastAltitude = Altitude(currentState);
        lastMode = stage.mode;                          //LastMode
        currentState = rk4(currentState, h);            //NewRocket
        Jd += SecondsToDecDay(h);                       //Increment time
        Met += h;
        currentState.met = Met;
        currentStage = stage;
    }
    
    return stage;
}

void readConfigFile(char *filename)
{
    int numOfStages;
    int i;
    
    /* Initialize the configuration */
    config_init(&cfg);
    
    /* Load the file */
    if (!config_read_file(&cfg, filename))
    {
        printf("failed config_read_file \"%s\"\n", filename);
        exit(1);
    }
    else
    {
        /* Config file layout */
        config_setting_t *configTStep           = NULL;
        config_setting_t *configLaunchPosition  = NULL;
        config_setting_t *configLaunchTime      = NULL;
        config_setting_t *configStages          = NULL;
        
        configTStep             = config_lookup(&cfg, "timeStep");
        configLaunchPosition    = config_lookup(&cfg, "launch.position");
        configLaunchTime        = config_lookup(&cfg, "launch.juliandate");
        configStages            = config_lookup(&cfg, "stages");
	
	    /* Make sure values are found in the config file */
        if (    !configTStep 
             || !configLaunchPosition 
             || !configLaunchTime 
             || !configStages) 
        {
            printf("failed config_lookup\n");
            exit(1);
        }
        else
        {
            // Time Step
            h = config_setting_get_float(configTStep);
            
            // Launch Position
            double lat = (double) config_setting_get_float_elem(configLaunchPosition, 0);
            double lon = (double) config_setting_get_float_elem(configLaunchPosition, 1);
            double alt = (double) config_setting_get_float_elem(configLaunchPosition, 2);
            
            // Time
            beginTime = (double) config_setting_get_float(configLaunchTime);
            
            /* Stages */
            // Allocate Memory
            numOfStages = config_setting_length(configStages);
            stages = (Rocket_Stage *) malloc(numOfStages * sizeof(Rocket_Stage));
            numberOfStages = numOfStages;   //GlobalVariable

            // Loop through stages in config file
            for (i = 0; i < numOfStages; i++)
            {
                // Get one stage
                config_setting_t *stage = NULL;
                stage =  config_setting_get_elem(configStages, i);
                if (stage == NULL)
                {
                    printf("Stage %d broke", i + 1);
                    exit(1);
                }
                
                double emptyMass    = (double) config_setting_get_float_elem(stage, 0);
                double fuelMass     = (double) config_setting_get_float_elem(stage, 1);
                double isp          = (double) config_setting_get_float_elem(stage, 2);
                double thrust       = (double) config_setting_get_float_elem(stage, 3);
                double delay        = (double) config_setting_get_float_elem(stage, 4);
                
                rocketDesc desc;
                desc.stage = i;
                desc.emptyMass = emptyMass;
                desc.stageDelay = delay;
                
                motor m;
                m.fuelMass = fuelMass;
                m.isp = isp;
                m.thrust = thrust;
                
                state initialState;
                initialState.s = ZeroVec();
                initialState.U = ZeroVec();
                initialState.a = ZeroVec();
                initialState.m = m;
                initialState.met = 0;
                
                // Init Stage
                stages[i].description = desc;
                stages[i].initialState = initialState;
                stages[i].currentState = initialState;    
                stages[i].mode = BURNING;
            }// End Stages Loop
                        
            state initialRocketState;
            initialRocketState = stages[0].initialState;
            initialRocketState.s = cartesian(Re + alt, PI/2.0 - radians(lat), radians(lon));
            initialRocketState.U = stages[0].initialState.U;
            initialRocketState.a = LinearAcceleration(initialRocketState, 0);
            
            launchState = initialRocketState;
        }// End config lookup if
    }// End config file read if
}

state LaunchState()
{
    return launchState;
}

double BeginTime()
{
    return beginTime;
}

double RunTime()
{
    return simulationRunTime;
}

Rocket_Stage CurrentStage()
{
    return currentStage;
}

int NumberOfStages()
{
    return numberOfStages;
}

Rocket_Stage *WholeRocket()
{
    return stages;
}

void printHelp()
{
    printf("ToOrbit Sim version %#.1f\n", VERSION);
    printf("©2009 Nathan Bergey availible under GPL 3\n\n");
    printf("Switches:\n");
    printf("\t-c - Config file name\n");
    printf("\t-v - Version number\n");
    printf("\n");
    printf("Examples:\n");
    printf("\torbit -c config.cfg\n");
    printf("\n");
}

void printVersion()
{
    printf("%#.1f\n", VERSION);
}
