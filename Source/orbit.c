#include <stdio.h>
#include <libconfig.h>
#include <stdlib.h>
#include "structs.h"
#include "coord.h"
#include "physics.h"
#include "vecmath.h"
#include "rout.h"
#include "rk4.h"
#include "orbit.h"

#define VERSION 0.1

struct config_t cfg;
state initRocket;
state burnoutRocket;
double apogee;
double tburnout;
double fuelMass;
double Isp;
double thrust_init;
double emptyMass;
double lastMass;
double t_bo;
float h;
char *outputFileName;
FILE *out;

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
    
    // Open output file for editing
    out = fopen(outputFileName, "w");
    
    // Print header
    printHeader(out);
    
    /* Do it */
    run();
    
    // close file
    fclose(out);
    
    /* exit */
    return 0;
}

/**
 * Handles the actual running of the program
 */
void run()
{
    double t;
    double alt, lastAlt, mass_f;
    int i;
    state rocket, lastRocket;
    
    rocket = initRocket;
    lastRocket = rocket;
    
    for (t = 0; t < 6580; t += h)
    {
        alt = altitude(rocket);
        lastAlt = altitude(lastRocket);
        
        lastMass = lastRocket.m;
        
        if (lastAlt < alt)
        {
            apogee = lastAlt;
        }
        
        if (alt < -1) // hit ground
        {
            break;
        }
        
        if ((rocket.m == EmptyMass()) && (lastMass > EmptyMass()))
        {
            t_bo = t;
            burnoutRocket = rocket;
        }
        
        printLine(out, rocket, t);      //Print File
        //printLineKml(outKml, rocket);   //Print KML File
        
        lastRocket = rocket;            //LastRocket
        rocket = rk4(rocket, h, t);     //NewRocket
    }

    printf("%s\t%f %s\n", "\t          Apogee: ", apogee, "m");
    printf("%s\t%f %s\n", "\t       Downrange: ", downrange(rocket), "m");
    printf("%s\t%f %s\n", "\t    Burnout Time: ", t_bo, "s");
    printf("%s\t%f %s\n", "\tBurnout Velocity: ", velocity(burnoutRocket), "m/s");
    
    //BuildBurnPlt(t_bo);
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
            emptyMass = (double) config_setting_get_float(emass);
            fuelMass = (double) config_setting_get_float(fmass);
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
            
            initRocket.m = emptyMass + fuelMass;
            
            vec accel = physics(initRocket, 0);
            initRocket.a[x] = accel.i;
            initRocket.a[y] = accel.j;
            initRocket.a[z] = accel.k;
            
            initRocket.mode = 0;
        }
    }
}

state initialRocket()
{
    return initRocket;
}

double FuelMass()
{
    return fuelMass;
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

double EmptyMass()
{
    return emptyMass;
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
