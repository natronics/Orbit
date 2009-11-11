#include <stdio.h>
#include <string.h>
#include <libconfig.h>
#include "structs.h"
#include "coord.h"
#include "physics.h"
#include "vecmath.h"
#include "rout.h"
#include "rk4.h"

void run();

struct config_t cfg;
state initRocket;
double lon_0, lat_0;
float h;

int main(int argc, char **argv)
{
    char header[256] = "#";
    char *file = "orbit.cfg";
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
		        case 'c':	
		            file = argv[i+1];
				    break;
		        default:	
		            fprintf(stderr, "Unknown switch %s\n", argv[i]);
	        }
	    }
    }
        
    /* Initialize the configuration */
    config_init(&cfg);
    
    /* Load the file */
    if (!config_read_file(&cfg, file))
    {
        printf("failed config_read_file \"%s\"\n", file);
        return 1;
    }
    else
    {
        config_setting_t *pos = NULL;
        config_setting_t *vel = NULL;
        config_setting_t *mass = NULL;
        config_setting_t *step = NULL;
	    config_setting_t *leng = NULL;
	    config_setting_t *OD = NULL;
	    config_setting_t *Cd = NULL;
        
        pos     = config_lookup(&cfg, "rocketInit.position");
        vel     = config_lookup(&cfg, "rocketInit.velocity");
        mass    = config_lookup(&cfg, "rocketInit.mass");
        step    = config_lookup(&cfg, "timeStep");
	    leng    = config_lookup(&cfg, "rocketDesc.length");
	    OD      = config_lookup(&cfg, "rocketDesc.OD");
	    Cd      = config_lookup(&cfg, "rocketDesc.Cd");
	
	    if (leng)
	    {
	        double length = config_setting_get_float(leng);
	        //printf("%f\n", length);
	    }
	    if (OD)
	    {
	        double outerDiameter = config_setting_get_float(OD);
	        //printf("%f\n", outerDiameter);
	    }
	    if (Cd)
	    {
	        double coefDrag = config_setting_get_float(Cd);
	        //printf("%f\n", coefDrag);
	    }
        
        if (!pos || !vel || !mass || !step)
        {
            printf("failed config_lookup\n");
            return 1;
        }
        else
        {
            double lat = config_setting_get_float_elem(pos, 0);
            double lon = config_setting_get_float_elem(pos, 1);
            double alt = config_setting_get_float_elem(pos, 2);
            double U_x = config_setting_get_float_elem(vel, 0);
            double U_y = config_setting_get_float_elem(vel, 1);
            double U_z = config_setting_get_float_elem(vel, 2);
            double m = config_setting_get_float(mass);
            
            vec U_enu;
            vec U_ecef;
            
            h = config_setting_get_float(step);
           
            lon_0 = radians(lon);
            lat_0 = radians(lat);
            vec cart = cartesian(Re + alt, PI/2.0 - lat_0, lon_0);
            
            initRocket.s[x] = cart.i;
            initRocket.s[y] = cart.j;
            initRocket.s[z] = cart.k;
            
            /*
            initRocket.U[x] = U_x;
            initRocket.U[y] = U_y;
            initRocket.U[z] = U_z;
            */
            
            U_enu.i = U_x;
            U_enu.j = U_y;
            U_enu.k = U_z;
            
            U_ecef = ecefFromEnu(U_enu, initRocket);
            
            initRocket.U[x] = U_ecef.i;
            initRocket.U[y] = U_ecef.j;
            initRocket.U[z] = U_ecef.k;
            
            
            initRocket.m = m;
            
            vec accel = physics(initRocket, 0);
            initRocket.a[x] = accel.i;
            initRocket.a[y] = accel.j;
            initRocket.a[z] = accel.k;
        }
    }

    printHeader();
    
    run();
    
    return 0;
}

void run()
{
    double t;
    double altitueEvents[3] = {10.4, 100.0, 60.5};
    double alt;
    int i;
    state rocket, lastRocket;
    
    rocket = initRocket;
    
    
    for (t = 0; t < 100; t += h)
    {
        alt = altitude(rocket);
        for (i = 0; i < sizeof(altitueEvents); i++)
        {
            
            if (alt > altitueEvents[i])
            {
                //maybe something?
            }
        }
        
        if (alt < -10) // hit ground
        {
            break;
        }
        printLine(rocket, t, lat_0, lon_0);
        rocket = rk4(rocket, h, t);
    }
}
