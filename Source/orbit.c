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
static int thrustCurve_noFile(vec2 **curve, double thrust, double fuel, double isp);
static int thrustCurve_File(vec2 **curve, const char *fileName, double thrust);
double initFuelMass(Rocket_Stage stage);

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
    stages[0].initialState.fuelMass = initFuelMass(stages[0]);
    stages[0].mode = BURNING;
    
    // Do it
    for(i = 0; i < numberOfStages; i++)
    {
        /* This does all the work, returns a stage that has run throught the
         * simulation
         */
        currentStage = stages[i];
        stages[i] = run(stages[i]);

        /* If this is not the last stage then prime the next stage
         * with the data from when the last stage separated
         */
        if ((i + 1) < numberOfStages)
        {
            state nextStageInitialState = stages[i].separationState;
            stages[i + 1].initialState.s = nextStageInitialState.s;
            stages[i + 1].initialState.U = nextStageInitialState.U;
            stages[i + 1].initialState.a = LinearAcceleration(stages[i + 1].initialState, Met);
            stages[i + 1].initialState.met = nextStageInitialState.met;
            // Go back in time to when the stages separated
            double backInTime = Met - nextStageInitialState.met;
            Met = nextStageInitialState.met;
            Jd = Jd - SecondsToDecDay(backInTime);
        }
        // Print blank lines in the files to separate the stages in gnuplot
        fprintf(outBurn, "\n");
        fprintf(outCoast, "\n");
        fprintf(outSpent, "\n");
        // Show some output on the screen
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
    
    /* Run the simulation until the stage hits the ground or it's takeing too
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
        if (stage.mode == BURNING && currentState.fuelMass < 0)
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
            double fuelLoss =  MDot(currentState, Met) * h;
            currentState.fuelMass -= fuelLoss;
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
        if ( (Met - lastTime) > 0.01 )
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
                    PrintKmlLine(outKml, currentState);
                    PrintForceLine(outForce, Jd, currentState);
                    break;
                case BURNING:
                    PrintStateLine(outBurn, Jd, currentState);
                    PrintKmlLine(outKml, currentState);
                    PrintForceLine(outForce, Jd, currentState);
                    break;
                case COASING:
                    PrintStateLine(outCoast, Jd, currentState);
                    PrintKmlLine(outKml, currentState);
                    PrintForceLine(outForce, Jd, currentState);
                    break;
                case SEPARATED:
                    PrintStateLine(outSpent, Jd, currentState);
                    PrintForceLine(outForce, Jd, currentState);
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
    int i, j, k;
    
    /* Initialize the configuration */
    config_init(&cfg);
    
    // Dummy initial state
    state initialState;
    initialState.s = ZeroVec();
    initialState.U = ZeroVec();
    initialState.a = ZeroVec();
    initialState.fuelMass = 0;
    initialState.met = 0;
    
    /* Load the file */
    if (!config_read_file(&cfg, configFileName))
    {
        printf("failed config_read_file \"%s\"\n", configFileName);
        exit(1);
    }
    
    /* Config file layout */
    config_setting_t *configTStep           = NULL;
    config_setting_t *configLaunchPosition  = NULL;
    config_setting_t *configLaunchVelocity  = NULL;
    config_setting_t *configLaunchTime      = NULL;
    config_setting_t *configStages          = NULL;
    
    configTStep             = config_lookup(&cfg, "timeStep");
    configLaunchPosition    = config_lookup(&cfg, "launch.position");
    configLaunchVelocity    = config_lookup(&cfg, "launch.velocity");
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

    // Time Step
    h = config_setting_get_float(configTStep);
    
    // Launch Position
    double lat = (double) config_setting_get_float_elem(configLaunchPosition, 0);
    double lon = (double) config_setting_get_float_elem(configLaunchPosition, 1);
    double alt = (double) config_setting_get_float_elem(configLaunchPosition, 2);

    // Launch Velocity (not required)

    if (configLaunchVelocity)
    {
      double v_E = (double) config_setting_get_float_elem(configLaunchVelocity, 0);
      double v_N = (double) config_setting_get_float_elem(configLaunchVelocity, 1);
      double v_U = (double) config_setting_get_float_elem(configLaunchVelocity, 2);

      vec v_enu;
      v_enu.i = v_E;
      v_enu.j = v_N;
      v_enu.k = v_U;
      state initial_state_dummy = initialState;
      initial_state_dummy.s = cartesian(Re + alt, PI/2.0 - radians(lat), radians(lon));
      vec v_ecef = EnuToEcef(v_enu, initial_state_dummy);
      initialState.U = v_ecef;
    }
    
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
        double ingnition    = (double) config_setting_get_float_elem(stage, 1);
        double stageing     = (double) config_setting_get_float_elem(stage, 2);
        
        // Create Description
        stageDesc desc;
                
        /* Get motors */
        config_setting_t *configStageMotors = NULL;
        configStageMotors = config_setting_get_member(stage, "motors");            
        if (configStageMotors == NULL)
        {
            printf("Can't find stage %d motors\n", i + 1);
            exit(1);
        } 
        int numOfMotors = config_setting_length(configStageMotors);
        // Allocate Memory
        desc.motors = malloc(numOfMotors * sizeof(motor));
        for (j = 0; j < numOfMotors; j++)
        {
            config_setting_t *motor = NULL;
            motor =  config_setting_get_elem(configStageMotors, j);
            if (motor == NULL)
            {
                printf("Broke Motor\n");
                exit(1);
            }
            
            // Get stuff from config file
            const char *motorName       = config_setting_get_string_elem(motor, 0);
            double fuelMass    = (double) config_setting_get_float_elem(motor, 1);
            double isp         = (double) config_setting_get_float_elem(motor, 2);
            double thrust      = (double) config_setting_get_float_elem(motor, 3);
            const char *thrustCurveFileName = config_setting_get_string_elem(motor, 4);
  
            int dataLength;
            // Inject into motors collection
            desc.motors[j].name = motorName;
            desc.motors[j].fuelMass = fuelMass;
            desc.motors[j].isp = isp;
            vec2 *thrustCurve;
            if (thrustCurveFileName == NULL)
                dataLength = thrustCurve_noFile(&thrustCurve, thrust, fuelMass, isp);
            else if (thrust == 0)
                dataLength = thrustCurve_File(&thrustCurve, thrustCurveFileName, 1);
            else
                dataLength = thrustCurve_File(&thrustCurve, thrustCurveFileName, thrust); 

            desc.motors[j].thrustCurve = thrustCurve;
            desc.motors[j].curveLength = dataLength;
            double fakeIsp = AverageIsp(desc.motors[j]);
            printf("Average Isp: %f\n", fakeIsp);
            desc.motors[j].isp = fakeIsp;
        }

        /* Get parachutes */
        config_setting_t *configStageChutes = NULL;
        configStageChutes = config_setting_get_member(stage, "chutes");
        if (configStageChutes == NULL)
        {
            printf("Broke\n");
            exit(1);
        } 
        int numOfChutes = config_setting_length(configStageChutes);
        // Allocate Memory
        chute *chutes = malloc(numOfChutes * sizeof(chute));
        for (k = 0; k < numOfChutes; k++)
        {
            config_setting_t *chute = NULL;
            chute =  config_setting_get_elem(configStageChutes, k);
            if (chute == NULL)
            {
                printf("Broke Chute\n");
                exit(1);
            }
            
            // Get stuff from config file
            double cd            = (double) config_setting_get_float_elem(chute, 0);
            double area          = (double) config_setting_get_float_elem(chute, 1);
            const char *openMode = config_setting_get_string_elem(chute, 2);
            double agl           = (double) config_setting_get_float_elem(chute, 3);
            
            // Inject into chute collection
            chutes[k].cd = cd;
            chutes[k].area = area;
        } 
        
        // Inject data into stage description
        desc.stage = i;
        desc.emptyMass = emptyMass;
        desc.ignitionDelay = ingnition;
        desc.stageDelay = stageing;
        desc.numOfMotors = numOfMotors;
        desc.numOfChutes = numOfChutes;
        desc.chutes = chutes;
        
        /* Uncomment this to test reading the config
         *
        DumpDescription(desc);
         */
        
        /* Initilize Stage */
        stages[i].description = desc;
        stages[i].initialState = initialState;
        stages[i].currentState = initialState;    
        stages[i].burnoutState = initialState;    
        stages[i].separationState = initialState;    
        stages[i].apogeeState = initialState;    
        stages[i].splashdownState = initialState;    
        stages[i].mode = INIT;
    }// End Stages Loop

    /* There should now be a rocket with all the right stages but dummy initial
     * states. To actually start the rocket off we compute the initial state
     * here.*/
    state initialRocketState;
    initialRocketState = stages[0].initialState;
    initialRocketState.s = cartesian(Re + alt, PI/2.0 - radians(lat), radians(lon));
    initialRocketState.U = stages[0].initialState.U;
    initialRocketState.a = LinearAcceleration(initialRocketState, 0);
    
    launchState = initialRocketState;
}

/**
 * If there is no thrust curve specified then we make a straght line,
 * assumeing the same thrust thought the burn.
 */
static int thrustCurve_noFile(vec2 **curve, double thrust, double fuel, double isp)
{
    double burntime = (fuel * isp * g_0) / thrust;
    
    vec2 t_0;
    t_0.i = 0;
    t_0.j = thrust;
    
    vec2 t_bo;
    t_bo.i = burntime;
    t_bo.j = thrust;
    
    *curve = malloc(2 * sizeof(vec2));
    
    curve[0][0] = t_0;
    curve[0][1] = t_bo;
    
    return 2;
}

static int thrustCurve_File(vec2 **curve, const char *fileName, double thrust)
{    
    FILE *data;
    int dataLength = 0;
    char line[128];         // or other suitable maximum line size
    int i = 0;
    
    data = fopen(fileName, "r");
    if (data == NULL)
    {
        printf("Error reading Thrust Curve file\n");
        exit(1);
    }

    /* Read through once and figure out how long the file is, ignoring lines
     * that start with "#"
     */
    while ( fgets(line, sizeof line, data) != NULL )
    {
        if (line[0] != '#')
            dataLength++;
    }

    rewind(data);

    // Allocate data
    *curve = malloc(dataLength * sizeof(vec2));

    double impulse = 0;
    /* Put the data in an array */
    while ( fgets(line, sizeof line, data) != NULL )
    {
        float time, normalThrust;
        if (line[0] != '#')
        {
            sscanf(line, "%f,%f", &time, &normalThrust);
            vec2 point;
            curve[0][i].i = time;
            curve[0][i].j = normalThrust * thrust;
            double interval = 0;
            if (i == 0)
                interval = time;
            else
                interval = time - curve[0][i - 1].i;
            impulse += (normalThrust * thrust) * interval;
            i++;
        }
    }
    // Close the file
    fclose(data);

    return dataLength;
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

double initFuelMass(Rocket_Stage stage)
{
    int i;
    stageDesc desc = stage.description;
    int numOfMotors = desc.numOfMotors;
    int fuelMass = 0;
    for (i = 0; i < numOfMotors; i++)
        fuelMass += desc.motors[i].fuelMass;
    return fuelMass;
}

void printHelp()
{
    printf("ToOrbit Sim version %#.1f\n", VERSION);
    printf("©2009 Nathan Bergey availible under GPL v3\n\n");
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
