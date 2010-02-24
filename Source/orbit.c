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
double beginTime;                   //Start time in JD
float h;                            //Timestep
double Jd;                          //Current time in Julian Date 
double Met;                         //Current time in Mission Elapsed Time
int numberOfStages;                 //Total number of stages
double simulationRunTime;           //How long the simulation took in seconds

char *configFileName = "orbit.cfg"; //Default Config File Name
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
void readCommandLineSwitches(int argc, char **argv);
void readConfigFile();
void initOutputFiles();
Rocket_Stage run(Rocket_Stage stage);

/**
 * ToOrbit Sim
 */
int main(int argc, char **argv)
{
    clock_t start, end;         //For seeing how long the simulation takes
    int i;
    
    /* Read switches */
    readCommandLineSwitches(argc, argv);
    
    /* Read the config file */
    /* reading the config file should populate all of the start time and 
     * rocket stucts, so we can use them below */
    readConfigFile();
    
    /* Attempt to create Output files */
    initOutputFiles();
    
    /* Begin Simulation */
    start = clock();
    
    // Set the time
    Met = 0;
    Jd = BeginTime();
    
    stages[0].initialState = LaunchState();
    stages[0].mode = BURNING;
    
    // Do it
    for(i = 0; i < numberOfStages; i++)
    {
        /* This does all the work, returns a stage that has run throught the
         * simulation
         */
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
        fprintf(outSpent, "\n");
        PrintSimResult(stages[i]);
    }
    
    /* Finished */
    end = clock();
    simulationRunTime = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    PrintHtmlResult(stages);
    MakePltFiles(stages[numberOfStages - 1]);
    
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
    for (simTime = 0; simTime < 10000; simTime += h)
    {
        // State Logic
        currentAltitude = Altitude(currentState);
        if (stage.mode == INIT
            && simTime >= stage.description.ignitionDelay)
        {   
            printf("Stage Ignition!\n");
            fprintf(outCoast, "\n");
            stage.mode = BURNING;
        }
        if (stage.mode == BURNING && currentState.m.fuelMass < 0)
        {
            printf("Burnout!\n");
            PrintStateLine(outBurn, Jd, lastState);
            PrintStateLine(outCoast, Jd, currentState);
            stage.mode = COASING;
            stage.burnoutState = lastState;
            burnoutTime = Met;
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
            printf("Hit the Ground!!\n");
            stage.splashdownState = lastState;
            break;
        }
        
        ///TODO: Actually integrate this.
        if (mode == BURNING)
        {
            double mdot =  MDot(currentState, Met) * h;
            currentState.m.fuelMass -= mdot;
        }
        
        if (mode > BURNING) // Wait some time, then separate
        {
            if (Met - burnoutTime > stage.description.stageDelay 
                && mode != SEPARATED
                && notLastStage > 0)
            {
                printf("Separation!\n");
                stage.separationState = lastState;
                stage.mode = SEPARATED;
            }
        }
        
        // Print files no more often than every tenth of a second.
        if ( (Met - lastTime) > 0.5 )
        {
           // PrintKmlLine(outKml, currentState);
            if ( (longitude(currentState) < 0 && longitude(lastState) > 0)
                || (longitude(currentState) > 0 && longitude(lastState) < 0)
                )
            
            {
                printf("Cross the line!\n");
                fprintf(outBurn, "\n");
                fprintf(outCoast, "\n");
                fprintf(outSpent, "\n");
            }
            switch (mode)
            {
                case INIT:
                    PrintStateLine(outCoast, Jd, currentState);
                    break;
                case BURNING:
                    PrintStateLine(outBurn, Jd, currentState);
                    break;
                case COASING:
                    PrintStateLine(outCoast, Jd, currentState);
                    break;
                case SEPARATED:
                    PrintStateLine(outSpent, Jd, currentState);
                    break;
                default:
                    PrintStateLine(outCoast, Jd, currentState);
                    break;
            }  
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
    
    if (stage.separationState.met == 0.0)
        stage.separationState = currentState;
    
    return stage;
}

void readCommandLineSwitches(int argc, char **argv)
{
    int i;
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
		            configFileName = argv[i+1];
				    break;
				case 'h':   // print help
				    printHelp();
				    exit(0);
				    break;
				case 'v' :  // version
				    printVersion();
				    exit(0);
				    break;
		        default:	
		            fprintf(stderr, "Unknown switch %s\n", argv[i]);
	        }
	    }
    }
}

void readConfigFile()
{
    int numOfStages;
    int i;
    
    /* Initialize the configuration */
    config_init(&cfg);
    
    /* Load the file */
    if (!config_read_file(&cfg, configFileName))
    {
        printf("failed config_read_file \"%s\"\n", configFileName);
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
                double ingnition    = (double) config_setting_get_float_elem(stage, 4);
                double stageing     = (double) config_setting_get_float_elem(stage, 5);
                
                rocketDesc desc;
                desc.stage = i;
                desc.emptyMass = emptyMass;
                desc.ignitionDelay = ingnition;
                desc.stageDelay = stageing;
                
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
                stages[i].burnoutState = initialState;    
                stages[i].separationState = initialState;    
                stages[i].apogeeState = initialState;    
                stages[i].splashdownState = initialState;    
                stages[i].mode = INIT;
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

void initOutputFiles()
{
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
