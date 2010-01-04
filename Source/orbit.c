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

struct config_t cfg;
state initRocket;
double Isp;
double thrust_init;
double beginTime;
float h;
char *outputFileName;
unsigned int hasNewModeBool = 0;
unsigned int newMode = 0;
FILE *outBurn;
FILE *outCoast;
FILE *outKml;
FILE *outForce;

void printHelp();
void printVersion();
void readConfigFile(char *filename);
void run();

/**
 * ToOrbit Sim
 */
int main(int argc, char **argv)
{
    char *file = "orbit.cfg"; //Default config file name
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
    readConfigFile(file);
    
    // Open output files for editing
    outBurn = fopen("Output/out-burn.dat", "w");
    outCoast = fopen("Output/out-coast.dat", "w");
    outKml = fopen("Output/out.kml", "w");
    outForce = fopen("Output/out-force.dat", "w");
    
    if (   outBurn == NULL 
        || outCoast == NULL 
        || outKml == NULL
        || outForce == NULL )
    {
        printf("File Handle error.");
        exit(1);
    }
    
    // Print Headers
    PrintHeader(outBurn);
    PrintHeader(outCoast);
    PrintKmlHeader(outKml);
    
    // Do it
    run();
    
    // Print Footers
    PrintKmlFooter(outKml);
    
    // close file
    fclose(outBurn);
    fclose(outCoast);
    fclose(outKml);
    
    // exit
    return 0;
}

/**
 * Handles the actual running of the program
 */
void run()
{   
    double lastTime;
    double Jd, Mjd, Met;
    double alt, lastAlt;
    unsigned int mode, lastMode;
    int i;
    state rocket, lastRocket;
    state burnoutRocket;
    state apogeeRocket;
    double time_bo;         //JD
    double time_apogee;     //JD
    clock_t start, end;
    double elapsed;

    start = clock();

    /*
    The Julian date for CE  2010 May  6 14:15:38.2 UT is
    JD 2455323.09419
    */

    Jd = 2455323.09419;
    beginTime = Jd;
    
    rocket = InitialRocket();
    lastRocket = InitialRocket();
    
    lastTime = 0;
    for (Met = 0; Met < 10000; Met += h)
    {
        Jd += SecondsToDecDay(h);
        
        alt = altitude(rocket);
        lastAlt = altitude(lastRocket);
        
        mode = rocket.mode;
        lastMode = lastRocket.mode;
        
        if (lastAlt < alt)
        {
            apogeeRocket = rocket;
            time_apogee = Jd;
        }
        
        if (alt < -1) // hit ground
        {
            break;
        }
        
        if (mode == BURNING)
        {
            PrintLine(outBurn, Jd, Met, rocket);       //Print File
            PrintForceLine(outForce, Jd, Met, rocket); //Print File
        }
        else
        {
            if (lastMode == BURNING)
            {
                burnoutRocket = lastRocket;
                time_bo = Jd;
            }
            PrintLine(outCoast, Jd, Met, rocket);      //Print File
        }  
        
        if ( (Met - lastTime) > 1.0 )
        {
            PrintKmlLine(outKml, rocket);
            lastTime = Met;
        }
        
        lastRocket = rocket;                //LastRocket
        rocket = rk4(rocket, h, Met);       //NewRocket
    }

    end = clock();
    elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

    PrintResult(burnoutRocket, apogeeRocket, time_bo, time_apogee);
    PrintHtmlResult(burnoutRocket, apogeeRocket, time_bo, time_apogee, elapsed);
    MakePltFiles();
}

void readConfigFile(char *filename)
{
    vec U_enu, U_ecef;

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
        config_setting_t *tstep         = NULL;
        config_setting_t *outFile       = NULL;
        
        config_setting_t *pos           = NULL;
        config_setting_t *vel           = NULL;
        
        config_setting_t *emass         = NULL;
        config_setting_t *fmass         = NULL;
        config_setting_t *isp           = NULL;   
        config_setting_t *thrust        = NULL;     
	    config_setting_t *leng          = NULL;
	    config_setting_t *OD            = NULL;
        
        tstep       = config_lookup(&cfg, "timeStep");
        outFile     = config_lookup(&cfg, "outputFile");
        
        pos         = config_lookup(&cfg, "rocketInit.position");
        vel         = config_lookup(&cfg, "rocketInit.velocity");
            
        emass       = config_lookup(&cfg, "rocketDesc.emptyMass");
        fmass       = config_lookup(&cfg, "rocketDesc.fuelMass");
        isp         = config_lookup(&cfg, "rocketDesc.Isp");
        thrust      = config_lookup(&cfg, "rocketDesc.thrust");
	    leng        = config_lookup(&cfg, "rocketDesc.length");
	    OD          = config_lookup(&cfg, "rocketDesc.OD");
	
	    /* Make sure values are found in the config file */
        if ( !tstep || !outFile || !pos || !vel 
            || !emass || !fmass || !isp || !thrust
            || !leng || !OD ) 
        {
            printf("failed config_lookup\n");
            exit(1);
        }
        else
        {
            // Time Step
            h = config_setting_get_float(tstep);
            
            // output file name
            outputFileName = (char*) config_setting_get_string(outFile);
            
            // Position
            double lat = (double) config_setting_get_float_elem(pos, 0);
            double lon = (double) config_setting_get_float_elem(pos, 1);
            double alt = (double) config_setting_get_float_elem(pos, 2);
            
            // Velocity
            double U_x = (double) config_setting_get_float_elem(vel, 0);
            double U_y = (double) config_setting_get_float_elem(vel, 1);
            double U_z = (double) config_setting_get_float_elem(vel, 2);
            
            // Rocket
            double emptyMass = (double) config_setting_get_float(emass);
            double fuelMass = (double) config_setting_get_float(fmass);
            Isp = (double) config_setting_get_float(isp);
            thrust_init = (double) config_setting_get_float(thrust);
            double length = (double) config_setting_get_float(leng);
            double outerDiameter = (double) config_setting_get_float(OD);
            
            vec cart = cartesian(Re + alt, PI/2.0 - radians(lat), radians(lon));
            
            initRocket.s[x] = cart.i;
            initRocket.s[y] = cart.j;
            initRocket.s[z] = cart.k;
            
            U_enu.i = U_x;
            U_enu.j = U_y;
            U_enu.k = U_z;
            
            U_ecef = ecefFromEnu(U_enu, initRocket);
            
            initRocket.U[x] = U_ecef.i;
            initRocket.U[y] = U_ecef.j;
            initRocket.U[z] = U_ecef.k;

            initRocket.m.structure = emptyMass;
            initRocket.m.fuel = fuelMass;

            vec accel = DoPhysics(initRocket, 0);
            initRocket.a[x] = accel.i;
            initRocket.a[y] = accel.j;
            initRocket.a[z] = accel.k;
            
            initRocket.mode = BURNING;
        }
    }
}

state InitialRocket()
{
    return initRocket;
}

double I_sp()
{
    return Isp;
}

double mdot()
{
    double mdot;
    mdot = thrust_init / (g_0 * Isp);
    return mdot;
}

void setNewMode(unsigned int mode)
{
    newMode = mode;
    hasNewModeBool = 1;
}

unsigned int getNewMode()
{
    hasNewModeBool = 0;
    return newMode;
}

unsigned int hasNewMode()
{
    return hasNewModeBool;
}

double BeginTime()
{
    return beginTime;
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
