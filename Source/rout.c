/*! 
 * \file rout.c
 * \brief Handles output
 *  
 * Has several functions called by the main program to assist in writting 
 * output both to the screen and to file.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "structs.h"
#include "physics.h"
#include "coord.h"
#include "orbit.h"
#include "rout.h"

void printHtmlFileHeader(FILE *out);
void printHtmlHeader(FILE *out, char *header);
void printHtmlFileFooter(FILE *out);
void printHtmlImage(FILE *out, char *img);
void printHtmlOverviewTable(FILE *out, Rocket_Stage *stages, int numOfStages);
void printHtmlDetailDiv(FILE *out, FILE *pltOut, Rocket_Stage stage);
void makeLaunchMapPlt(state burnout);
void makeLaunch_3dPlt();
void makeAltDownPlt(state apogee);
void makeOverviewPlt(state apogee, state burnout);
void makeStageBurnPltHeader(FILE *pltOut);
void makeStageBurnPltSection(FILE *pltOut, state ignition, state burnout, int stage);
void makeStageBurnPltFooter(FILE *pltOut);
char nar(double impulse);

void PrintStateLine(FILE *outfile, double jd, state r)
{
    char format[512] = "";
    char exp[8] = "%0.10e\t";
    double lat = degrees(latitude(r));
    double lon = degrees(longitude(r));
    
    strcat(format, "%0.4f  \t");    //1     Time MET
    strcat(format, "%0.8f  \t");    //2     Time MJD
    strcat(format, exp);            //3     X
    strcat(format, exp);            //4     Y
    strcat(format, exp);            //5     Z
    strcat(format, exp);            //6     U_x
    strcat(format, exp);            //7     U_y
    strcat(format, exp);            //8     U_z
    strcat(format, exp);            //9     a_x
    strcat(format, exp);            //10    a_y
    strcat(format, exp);            //11    a_z
    strcat(format, exp);            //12    mass
    strcat(format, exp);            //13    KE
    strcat(format, exp);            //14    PE
    strcat(format, "%0.12f\t");     //15    Lat
    strcat(format, "%0.12f\t");     //16    Lon
    strcat(format, exp);            //17    Alt
    strcat(format, "%0.10f\t");     //18    Downrange
    strcat(format, "\n");
    
    
    fprintf(outfile, format
        ,   r.met                   //1     Time MET
        ,   jd                      //2     Time JD
        ,   r.s.i                   //3     X
        ,   r.s.j                   //4     Y
        ,   r.s.k                   //5     Z
        ,   r.U.i                   //6     U_x
        ,   r.U.j                   //7     U_y
        ,   r.U.k                   //8     U_z
        ,   r.a.i                   //9     a_x
        ,   r.a.j                   //10    a_y
        ,   r.a.k                   //11    a_z
        ,   RocketMass(r, r.met)    //12    mass
        ,   KE(r, r.met)            //13    KE
        ,   PE(r, r.met)            //14    PE
        ,   lat                     //15    Lat
        ,   lon                     //16    Lon
        ,   Altitude(r)             //17    Alt
        ,   Downrange(r));          //18    Downrange
        
}

void PrintHeader(FILE *outfile)
{
    char header[512] = "#";
    
    strcat(header, "Time(s)");                  //1
    strcat(header, "\tJulian Date     ");       //2
    strcat(header, "\tX(m)            ");       //3
    strcat(header, "\tY(m)            ");       //4
    strcat(header, "\tZ(m)            ");       //5
    strcat(header, "\tVel_x(m/s)      ");       //6
    strcat(header, "\tVel_y(m/s)      ");       //7
    strcat(header, "\tVel_z(m/s)      ");       //8
    strcat(header, "\tAccel_x(m/s^2)  ");       //9
    strcat(header, "\tAccel_y(m/s^2)  ");       //10
    strcat(header, "\tAccel_z(m/s^2)  ");       //11
    strcat(header, "\tMass            ");       //12
    strcat(header, "\tKE(J)           ");       //13
    strcat(header, "\tPE(J)           ");       //14
    strcat(header, "\tLat(°)          ");       //15
    strcat(header, "\tLon(°)          ");       //16
    strcat(header, "\tAlt(m MSL)      ");       //17
    strcat(header, "\tDownrange(m)");           //18
    strcat(header, "\n");
    
    fprintf(outfile, "%s", header);
}

void PrintForceLine(FILE *outfile, double jd, state r)
{
    char format[512] = "";
    char exp[8] = "%0.10e\t";
    vec thrust = Force_Thrust(r, r.met);
    double mdot = MDot(r, r.met);

    strcat(format, "%0.4f  \t");    //1     Time MET
    strcat(format, "%0.8f  \t");    //2     Time JD
    strcat(format, exp);            //3     Thrust_x
    strcat(format, exp);            //4     Thrust_y
    strcat(format, exp);            //5     Thrust_z
    strcat(format, exp);            //6     Mdot
    strcat(format, exp);            //7     Mass
    strcat(format, "\n");

    fprintf(outfile, format
        ,   r.met                   //1     Time MET
        ,   jd                      //2     Time JD
        ,   thrust.i                //3     Thrust_x
        ,   thrust.j                //4     Thrust_y
        ,   thrust.k                //5     Thrust_z
        ,   mdot                    //6     Mdot
        ,   r.fuelMass);            //6     Mass

}

void PrintSimResult(Rocket_Stage stage)
{
    state burnout = stage.burnoutState;
    double burnTime = burnout.met - stage.initialState.met;
    
    printf("%s%16.2f %s\n", "\t        Burn Time: ", burnTime, "s");
    printf("%s%16.2f %s\n", "\t Burnout Velocity: ", Velocity(burnout), "m/s");
    printf("\n");
}

void PrintKmlHeader(FILE *outfile)
{
    /* KML Header */
    fprintf(outfile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(outfile, "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
    fprintf(outfile, "  <Document>\n");
    fprintf(outfile, "    <name>Paths</name>\n");
    fprintf(outfile, "    <description>Examples of paths. Note that the tessellate tag is by default\n");
    fprintf(outfile, "      set to 0. If you want to create tessellated lines, they must be authored\n");
    fprintf(outfile, "      (or edited) directly in KML.</description>\n");
    fprintf(outfile, "    <Style id=\"yellowLineGreenPoly\">\n");
    fprintf(outfile, "      <LineStyle>\n");
    fprintf(outfile, "        <color>7f00ffff</color>\n");
    fprintf(outfile, "        <width>4</width>\n");
    fprintf(outfile, "      </LineStyle>\n");
    fprintf(outfile, "      <PolyStyle>\n");
    fprintf(outfile, "        <color>7f00ff00</color>\n");
    fprintf(outfile, "      </PolyStyle>\n");
    fprintf(outfile, "    </Style>\n");
    fprintf(outfile, "    <Placemark>\n");
    fprintf(outfile, "      <name>Absolute Extruded</name>\n");
    fprintf(outfile, "      <description>Transparent green wall with yellow outlines</description>\n");
    fprintf(outfile, "      <styleUrl>#yellowLineGreenPoly</styleUrl>\n");
    fprintf(outfile, "      <LineString>\n");
    fprintf(outfile, "       <extrude>1</extrude>\n");
    fprintf(outfile, "        <tessellate>1</tessellate>\n");
    fprintf(outfile, "        <altitudeMode>absolute</altitudeMode>\n");
    fprintf(outfile, "        <coordinates>\n");
}

void PrintKmlFooter(FILE *outfile)
{
    fprintf(outfile, "        </coordinates>\n");
    fprintf(outfile, "      </LineString>\n");
    fprintf(outfile, "    </Placemark>\n");
    fprintf(outfile, "  </Document>\n");
    fprintf(outfile, "</kml>\n");
}

void PrintKmlLine(FILE *outfile, state r)
{
    double lat = degrees(latitude(r));
    double lon = degrees(longitude(r));
    fprintf(outfile, "          %f,%f,%#.1f\n", lon, lat, Altitude(r));
}

void PrintHtmlResult(Rocket_Stage *stages)
{
    FILE *htmlOut = NULL;
    state launchState = LaunchState(); 
    int i;
    double rocketTotalMass = 0;
    double rocketTotalFuelMass = 0;
    double rocketTotalStructureMass = 0;
    double rocketTotalImpulse = 0;
    int numOfStages = NumberOfStages();
    
    char runTimeStringLoc[256];
    char launchTimeStringUtc[256];
    char launchTimeStringLoc[256];
    char localTimeZone[16];
    time_t curtime;
    time_t launchTime;
    struct tm *runTimeLoc;
    struct tm *launchTimeUtc;
    struct tm *launchTimeLoc;
    
    /* Get the times in unix time */
    curtime         = time(NULL);
    launchTime      = JdToUnixTime(BeginTime());

    /* Convert it to local or utc time representation */
    runTimeLoc      = localtime(&curtime);
    strftime(runTimeStringLoc, 256, "%m/%d/%Y %H:%M:%S [%Z]", runTimeLoc);
    
    launchTimeUtc   = gmtime(&launchTime);
    strftime(launchTimeStringUtc, 256, "%m/%d/%Y %H:%M:%S", launchTimeUtc);
    
    launchTimeLoc   = localtime(&launchTime);
    strftime(launchTimeStringLoc, 256, "%m/%d/%Y %H:%M:%S", launchTimeLoc);
    
    launchTimeLoc   = localtime(&launchTime);
    strftime(localTimeZone, 16, "%Z", launchTimeLoc);
    
    /* Open the file to write */
    htmlOut = fopen("Output/result.html", "w");
    
    // Check if fopen was successfull
    if (htmlOut == NULL)
    {
        return;
    }
    
    /* Print the file header */
    printHtmlFileHeader(htmlOut);
    
    /* Print the results */
    
    // Start with when it was run
    printHtmlHeader(htmlOut, "Run Details");
    fprintf(htmlOut, "  <div>\n");
    fprintf(htmlOut, "  <p>Simulation run on %s and took %0.2f seconds</p>\n"
                ,   runTimeStringLoc
                ,   RunTime());   
    fprintf(htmlOut, "  </div>\n");
    
    // Liftoff Mass Configuration Table
    printHtmlHeader(htmlOut, "Rocket Configuration");
    fprintf(htmlOut, "  <div>\n");

    fprintf(htmlOut, "  <table class=\"data_table\" id=\"physical_overview\">\n");
    fprintf(htmlOut, "    <thead>\n");
    fprintf(htmlOut, "      <tr>\n");
    fprintf(htmlOut, "        <th>Stage [#]</th>\n");
    fprintf(htmlOut, "        <th>I<sub>sp</sub> [s]</th>\n");
    fprintf(htmlOut, "        <th>Fuel Mass [kg]</th>\n");
    fprintf(htmlOut, "        <th>Dry Mass [kg]</th>\n");
    fprintf(htmlOut, "        <th>Stage Mass [kg]</th>\n");
    fprintf(htmlOut, "        <th>Total Mass [kg]</th>\n");
    fprintf(htmlOut, "        <th>Average Thust [N]</th>\n");
    fprintf(htmlOut, "        <th>Mass Ratio (stage)</th>\n");
    fprintf(htmlOut, "        <th>Thrust to Weight Ratio</th>\n");
    fprintf(htmlOut, "        <th>Impulse [N&middot;s]</th>\n");
    fprintf(htmlOut, "      <tr>\n");
    fprintf(htmlOut, "    </thead>\n");
    fprintf(htmlOut, "  <tbody>\n");
    
    double rocketMassStage[numOfStages];
    for (i = 0; i < numOfStages; i++)
    {
        Rocket_Stage stage = stages[i];
        int j;
        double fuelMass = 0;
        for (j = 0; j < stage.description.numOfMotors; j++)
            fuelMass += stage.description.motors[j].fuelMass;
        double emptyMass = stage.description.emptyMass;
        
        rocketMassStage[i] = fuelMass + emptyMass;
    }
    
    // Loop through stages
    for (i = 0; i < numOfStages; i++)
    {
        int j;
        Rocket_Stage stage      = stages[i];
        motor stageMotor        = stage.description.motors[0];
        double isp              = stageMotor.isp;
        double fuelMass         = stageMotor.fuelMass;
        double emptyMass        = stage.description.emptyMass;
        double stageTotalMass   = fuelMass + emptyMass;
        double massRatio        = stageTotalMass / emptyMass;
        double thrust           = AverageThrust(stageMotor);
        double impulse          = Impulse(stageMotor);
        
        double rocketMass = 0;
        for(j = i; j < numOfStages; j++)
        {
            rocketMass += rocketMassStage[j];
        }
        
        double TTWRatio = thrust / (g_0 * rocketMass);
        
        
        fprintf(htmlOut, "      <tr>\n");
        fprintf(htmlOut, "        <td>%d</td>\n", i + 1);
        fprintf(htmlOut, "        <td>%0.0f</td>\n", isp);
        fprintf(htmlOut, "        <td>%0.2f</td>\n", fuelMass);
        fprintf(htmlOut, "        <td>%0.2f</td>\n", emptyMass);
        fprintf(htmlOut, "        <td>%0.2f</td>\n", stageTotalMass);
        fprintf(htmlOut, "        <td>%0.2f</td>\n", rocketMass);
        fprintf(htmlOut, "        <td>%0.0f</td>\n", thrust);
        fprintf(htmlOut, "        <td>%0.1f</td>\n", massRatio);
        fprintf(htmlOut, "        <td>%0.1f</td>\n", TTWRatio);
        fprintf(htmlOut, "        <td>%0.0f (%c)</td>\n", impulse, nar(impulse));
        fprintf(htmlOut, "      </tr>\n");
        
        // Add up the rocket totals
        rocketTotalMass += stageTotalMass;
        rocketTotalFuelMass += fuelMass;
        rocketTotalStructureMass += emptyMass;
        rocketTotalImpulse += impulse;
    }
    
    double massRatio = rocketTotalMass / rocketTotalStructureMass;
    double liftoffThrust = 0;
    for (i = 0; i < stages[0].description.numOfMotors; i++)
        liftoffThrust += AverageThrust(stages[0].description.motors[i]);
    double TTWRatio = liftoffThrust / (g_0 * rocketTotalMass);
    
    
    fprintf(htmlOut, "    </tbody>\n");
    fprintf(htmlOut, "    <tfoot class=\"total\">\n");
    fprintf(htmlOut, "      <tr>\n");
    fprintf(htmlOut, "        <td>Total</td>\n");
    fprintf(htmlOut, "        <td>&mdash;</td>\n");
    fprintf(htmlOut, "        <td>%0.2f</td>\n", rocketTotalFuelMass);
    fprintf(htmlOut, "        <td>%0.2f</td>\n", rocketTotalStructureMass);
    fprintf(htmlOut, "        <td>&mdash;</td>\n");
    fprintf(htmlOut, "        <td><span class=\"highlight\" title=\"GLOW - %0.0f lbs (%0.2f tons)\">%0.2f</span></td>\n", rocketTotalMass * 2.20462262, rocketTotalMass * 0.00110231131, rocketTotalMass);
    fprintf(htmlOut, "        <td>&mdash;</td>\n");
    fprintf(htmlOut, "        <td>%0.1f</td>\n", massRatio);
    fprintf(htmlOut, "        <td>%0.1f</td>\n", TTWRatio);
    fprintf(htmlOut, "        <td>%0.0f (%c)</td>\n", rocketTotalImpulse, nar(rocketTotalImpulse));
    fprintf(htmlOut, "      </tr>\n");
    fprintf(htmlOut, "    </tfoot>\n");
    fprintf(htmlOut, "  </table>\n");
    
    
    /* Liftoff Location Configuration Table */
    fprintf(htmlOut, "  <table class=\"data_table\" >\n");
    fprintf(htmlOut, "    <thead>\n");
    fprintf(htmlOut, "      <th>Launch Time [UTC]</th>\n");
    fprintf(htmlOut, "      <th>Launch Time [%s]</th>\n", localTimeZone);
    fprintf(htmlOut, "      <th>Location</th>\n");
    fprintf(htmlOut, "    </thead>\n");
    fprintf(htmlOut, "    <tr>\n");
    fprintf(htmlOut, "      <td><span class=\"time\">%s</span></td>\n"
                            , launchTimeStringUtc);
    fprintf(htmlOut, "      <td><span class=\"time\">%s</span></td>\n"
                            ,launchTimeStringLoc);
    fprintf(htmlOut, "      <td><span class=\"latlon\">%+0.5f, %+0.5f</span></td>\n"
                            , degrees(latitude(launchState))
                            , degrees(longitude(launchState)));
    fprintf(htmlOut, "    </tr>\n");
    fprintf(htmlOut, "  </table>\n");
    fprintf(htmlOut, "  </div>\n");
    
    //Overview
    printHtmlHeader(htmlOut, "Flight Overview");
    fprintf(htmlOut, "  <div>\n");
    
    fprintf(htmlOut, "  <table class=\"data_table\" id=\"flight_overview\" >\n");
    fprintf(htmlOut, "    <thead>\n");
    fprintf(htmlOut, "      <tr>\n");
    fprintf(htmlOut, "        <th>Time [MET]</th>\n");
    fprintf(htmlOut, "        <th>Event</th>\n");
    fprintf(htmlOut, "        <th>Stage</th>\n");
    fprintf(htmlOut, "        <th>Altitude [km]</th>\n");
    fprintf(htmlOut, "        <th>Downrange [km]</th>\n");
    fprintf(htmlOut, "        <th>Velocity [m/s]</th>\n");
    fprintf(htmlOut, "      </tr>\n");
    fprintf(htmlOut, "    </thead>\n");
    fprintf(htmlOut, "    <tbody>\n");


    printHtmlOverviewTable(htmlOut, stages, numOfStages);
    fprintf(htmlOut, "  </div>\n");
    fprintf(htmlOut, "  <hr />\n");
    
    FILE *pltOut = NULL;
    pltOut = fopen("Output/Gnuplot/tmp/burn.plt", "w");
    
    if (pltOut != NULL)
    {
        makeStageBurnPltHeader(pltOut);
    }
    for (i = 0; i < numOfStages; i++)
    {
        printHtmlDetailDiv(htmlOut, pltOut, stages[i]);
    }
    if (pltOut != NULL)
    {
        makeStageBurnPltFooter(pltOut);
        fclose(pltOut);
    }
    
    printHtmlHeader(htmlOut, "Launch Map");
    printHtmlImage(htmlOut, "launchmap.png");
    printHtmlImage(htmlOut, "launch-3d.png");
    
    printHtmlHeader(htmlOut, "World Map");
    printHtmlImage(htmlOut, "worldmap.png");
    
    printHtmlFileFooter(htmlOut);
    
    fclose(htmlOut);
}

void printHtmlFileHeader(FILE *out)
{
    fprintf(out, "<html>\n");
    fprintf(out, "<head>\n");
    fprintf(out, "  <title>Rocket Flight Analysis</title>\n");
    fprintf(out, "  <link href=\"css/style.css\" rel=\"stylesheet\" type=\"text/css\" media=\"all\" />\n");
    fprintf(out, "  <link href=\"css/tablestyle.css\" rel=\"stylesheet\" type=\"text/css\" media=\"all\" />\n");
    fprintf(out, "  <script type=\"text/javascript\" src=\"js/jquery-1.4.2.min.js\"></script>\n");
    fprintf(out, "  <script type=\"text/javascript\" src=\"js/jquery.tablesorter.min.js\"></script>\n");
    fprintf(out, "  <script type=\"text/javascript\">\n");
    fprintf(out, "    $(document).ready(function() \n");
    fprintf(out, "      { \n");
    fprintf(out, "          $(\"#physical_overview\").tablesorter( {sortList: [[0,0]]} );\n");
    fprintf(out, "          $(\"#flight_overview\").tablesorter( {sortList: [[0,0]]} );\n");
    fprintf(out, "          $(\"h2\").click(function () {\n");
    fprintf(out, "            box = $(this).next();\n");
    fprintf(out, "            if (box.is(\":hidden\")) {\n");
    fprintf(out, "              box.slideDown(\"fast\");\n");
    fprintf(out, "            } else {\n");
    fprintf(out, "              box.slideUp(\"fast\");\n");
    fprintf(out, "            }\n");
    fprintf(out, "            return true;\n");
    fprintf(out, "         });\n");
    fprintf(out, "      } \n");
    fprintf(out, "  ); \n");
    fprintf(out, "  </script>\n");
    fprintf(out, "</head>\n");
    fprintf(out, "<body>\n");
    fprintf(out, "  <div id=\"header\">\n");
    fprintf(out, "    <h1>Rocket Flight Analysis</h1>\n");
    fprintf(out, "    <p><em>ToOrbit</em> Version %0.2f</p>\n", VERSION);
    fprintf(out, "  </div>\n");
    fprintf(out, "  <hr />\n");
    fprintf(out, "  <div id=\"content\">\n");
}

void printHtmlHeader(FILE *out, char *header)
{
    fprintf(out, "  <h2>%s</h2>\n", header);
}

void printHtmlImage(FILE *out, char *img)
{
    fprintf(out, "  <img src=\"%s\" />\n", img);
}

void printHtmlOverviewTable(FILE *out, Rocket_Stage *stages, int numOfStages)
{
    int i;
    
    /* Loop Through Stages */
    for (i = 0; i < numOfStages; i++)
    {
        state burnout    = stages[i].burnoutState;
        state apogee     = stages[i].apogeeState;
        state ignition   = stages[i].initialState;
        state separation = stages[i].separationState;
        state splashdown = stages[i].splashdownState;
        char igniteTime[12];
        char burnoutTime[12];
        char separationTime[12];
        char apogeeTime[12];
        char splashdownTime[12];
        SecondsToHmsString(ignition.met, igniteTime);
        SecondsToHmsString(burnout.met, burnoutTime);
        SecondsToHmsString(separation.met, separationTime);
        SecondsToHmsString(apogee.met, apogeeTime);
        SecondsToHmsString(splashdown.met, splashdownTime);
        
        //Stage Light
        fprintf(out, "      <tr>\n");
        fprintf(out, "        <td>%s</td>\n", igniteTime);
        fprintf(out, "        <td><em>Ignition</em></td>\n");
        fprintf(out, "        <td>%d</td>\n", i + 1);
        fprintf(out, "        <td>%0.2f</td>\n", Altitude(ignition) / 1000.0);
        fprintf(out, "        <td>%0.2f</td>\n", Downrange(ignition) / 1000.0);
        fprintf(out, "        <td>%0.2f</td>\n", Velocity(ignition));
        fprintf(out, "      </tr>\n");
        
        if (burnoutTime > 0)
        {
            //Burnout
            fprintf(out, "      <tr>\n");
            fprintf(out, "        <td>%s</td>\n", burnoutTime);
            fprintf(out, "        <td><em>Burnout</em></td>\n");
            fprintf(out, "        <td>%d</td>\n", i + 1);
            fprintf(out, "        <td>%0.2f</td>\n", Altitude(burnout) / 1000.0);
            fprintf(out, "        <td>%0.2f</td>\n", Downrange(burnout) / 1000.0);
            if ((i + 1) == numOfStages)
                fprintf(out, "        <td><span class=\"highlight\">%0.2f</span></td>\n", Velocity(burnout));
            else
                fprintf(out, "        <td>%0.2f</td>\n", Velocity(burnout));
            fprintf(out, "      </tr>\n");
        }
        
        if (separation.met > 0)
        {
            //Separation
            fprintf(out, "      <tr>\n");
            fprintf(out, "        <td>%s</td>\n", separationTime);
            fprintf(out, "        <td><em>Separation</em></td>\n");
            fprintf(out, "        <td>%d</td>\n", i + 1);
            fprintf(out, "        <td>%0.2f</td>\n", Altitude(separation) / 1000.0);
            fprintf(out, "        <td>%0.2f</td>\n", Downrange(separation) / 1000.0);
            fprintf(out, "        <td>%0.2f</td>\n", Velocity(separation));
            fprintf(out, "      </tr>\n");
        }
        
        if (apogee.met > 0)
        {
            //Apogee
            fprintf(out, "      <tr>\n");
            fprintf(out, "        <td>%s</td>\n", apogeeTime);
            fprintf(out, "        <td><em>Apogee</em></td>\n");
            fprintf(out, "        <td>%d</td>\n", i + 1);
            fprintf(out, "        <td>%0.2f</td>\n", Altitude(apogee) / 1000.0);
            fprintf(out, "        <td>%0.2f</td>\n", Downrange(apogee) / 1000.0);
            fprintf(out, "        <td>%0.2f</td>\n", Velocity(apogee));
            fprintf(out, "      </tr>\n");
        }
         
        if (splashdown.met > 0)
        {
            //Splashdown
            fprintf(out, "      <tr>\n");
            fprintf(out, "        <td>%s</td>\n", splashdownTime);
            fprintf(out, "        <td><em>Splashdown</em></td>\n");
            fprintf(out, "        <td>%d</td>\n", i + 1);
            fprintf(out, "        <td>%0.2f</td>\n", Altitude(splashdown) / 1000.0);
            fprintf(out, "        <td>%0.2f</td>\n", Downrange(splashdown) / 1000.0);
            fprintf(out, "        <td>%0.2f</td>\n", Velocity(splashdown));
            fprintf(out, "      </tr>\n");
        }
    }
    fprintf(out, "    </tbody>\n");
    fprintf(out, "  </table>\n");
    
    printHtmlImage(out, "overview.png");
    printHtmlImage(out, "overview.vel.accel.png");
}

void printHtmlDetailDiv(FILE *out, FILE *pltOut, Rocket_Stage stage)
{
    int stageNum = stage.description.stage + 1;
    fprintf(out, "<div class=\"stage_detail\">\n");
    fprintf(out, "<h2>Stage %d Detail</h2>\n", stageNum);

    state burnout    = stage.burnoutState;
    state apogee     = stage.apogeeState;
    state ignition   = stage.initialState;
    state separation = stage.separationState;
    state splashdown = stage.splashdownState;

    double burnoutTimeJd = BeginTime() + SecondsToDecDay(burnout.met);
    double apogeeTimeJd = BeginTime() + SecondsToDecDay(apogee.met);
    char burnoutTimeStringUtc[256];
    char apogeeTimeStringUtc[256];
    time_t time;
    struct tm *timeUtc;
    
    time = JdToUnixTime(burnoutTimeJd);
    timeUtc   = gmtime(&time);
    strftime(burnoutTimeStringUtc, 256, "%m/%d/%Y %H:%M:%S", timeUtc);
    time = JdToUnixTime(apogeeTimeJd);
    timeUtc   = gmtime(&time);
    strftime(apogeeTimeStringUtc, 256, "%m/%d/%Y %H:%M:%S", timeUtc);
    
    double coastTime = apogee.met - burnout.met;
    double burnTime = burnout.met - ignition.met;
    burnTime -= stage.description.ignitionDelay;
    
    
    if (pltOut != NULL)
        makeStageBurnPltSection(pltOut, ignition, burnout, stageNum);

    // Burnout
    fprintf(out, "  <h3>Burn</h3>\n");
    fprintf(out, "  <table class=\"data_table\" >\n");
    fprintf(out, "    <thead>\n");
    fprintf(out, "      <th>Burnout Time [UTC]</th>\n");
    fprintf(out, "      <th>Burn Time [s]</th>\n");
    fprintf(out, "      <th>Burnout Velocity [m/s]</th>\n");
    fprintf(out, "      <th>Burnout Altitude [km]</th>\n");
    fprintf(out, "      <th>Burnout Downrange [km]</th>\n");
    fprintf(out, "    </thead>\n");
    fprintf(out, "    <tr>\n");
    fprintf(out, "      <td><span class=\"time\">%s</span></td>\n" , burnoutTimeStringUtc);
    fprintf(out, "      <td>%0.2f</td>\n", burnTime);
    fprintf(out, "      <td>%0.2f</td>\n", Velocity(burnout));
    fprintf(out, "      <td>%0.2f</td>\n", Altitude(burnout) / 1000.0);
    fprintf(out, "      <td>%0.2f</td>\n", Downrange(burnout) / 1000.0);
    fprintf(out, "    </tr>\n");
    fprintf(out, "  </table>\n");
    
    fprintf(out, "<img src=\"burnout_%d.png\" />", stageNum);
    
    // Apogee
    fprintf(out, "  <h3>Apogee</h3>\n");
    fprintf(out, "  <table class=\"data_table\" >\n");
    fprintf(out, "    <thead>\n");
    fprintf(out, "      <tr>\n");
    fprintf(out, "        <th>Apogee Time [UTC]</th>\n");
    fprintf(out, "        <th>Time since Burnout [s]</th>\n");
    fprintf(out, "        <th>Apogee Velocity [m/s]</th>\n");
    fprintf(out, "        <th>Apogee Altitude [km]</th>\n");
    fprintf(out, "        <th>Apogee Downrange [km]</th>\n");
    fprintf(out, "      </tr>\n");
    fprintf(out, "    </thead>\n");
    fprintf(out, "    <tr>\n");
    fprintf(out, "      <td><span class=\"time\">%s</span></td>\n", apogeeTimeStringUtc);
    fprintf(out, "      <td>%0.2f</td>\n", coastTime);
    fprintf(out, "      <td>%0.2f</td>\n", Velocity(apogee));
    fprintf(out, "      <td>%0.2f</td>\n", Altitude(apogee) / 1000.0);
    fprintf(out, "      <td>%0.2f</td>\n", Downrange(apogee) / 1000.0);
    fprintf(out, "    </tr>\n");
    fprintf(out, "  </table>\n");
    
    fprintf(out, "  </div>\n");
}

void printHtmlFileFooter(FILE *out)
{
    fprintf(out, "</div>\n");
    fprintf(out, "</body>\n");
    fprintf(out, "</html>\n");
}

void MakePltFiles(Rocket_Stage finalStage)
{
    state apogee = finalStage.apogeeState;
    state burnout = finalStage.burnoutState;

    makeLaunchMapPlt(burnout);
    makeLaunch_3dPlt();
    makeAltDownPlt(apogee);
    makeOverviewPlt(apogee, burnout);
}

void makeLaunchMapPlt(state burnout)
{
    FILE *pltOut = NULL;
    double lat0, lon0, lat1, lon1;
    double latLaunch, lonLaunch;
    state launchState = LaunchState();
    double timeEnd = burnout.met + 600;
    
    latLaunch = degrees(latitude(launchState));
    lonLaunch = degrees(longitude(launchState));
    
    lat0 = latLaunch - 7.0;
    lat1 = latLaunch + 7.0;
    lon0 = lonLaunch - 7.0;
    lon1 = lonLaunch + 7.0;
    
    pltOut = fopen("Output/Gnuplot/tmp/launchmap.plt", "w");
    
    if (pltOut == NULL)
    {
        return;
    }
    
    fprintf(pltOut, "#!/usr/bin/gnuplot -persist\n\n");
    fprintf(pltOut, "reset\n\n");
    fprintf(pltOut, "set xrange[%0.1f:%0.1f]\n", lon0, lon1);
    fprintf(pltOut, "set yrange[%0.1f:%0.1f]\n\n", lat0, lat1);
    fprintf(pltOut, "l = %0.0f\n", timeEnd * 2);
    fprintf(pltOut, "load \"./Output/Gnuplot/launchmap_base.plt\"\n");
    fprintf(pltOut, "#    EOF");
    
    fclose(pltOut);
}

void makeLaunch_3dPlt()
{
    FILE *pltOut;
    double lat0, lon0, lat1, lon1;
    double latLaunch, lonLaunch;
    state launchState = LaunchState();
    latLaunch = degrees(latitude(launchState));
    lonLaunch = degrees(longitude(launchState));
    
    pltOut = fopen("Output/Gnuplot/tmp/launch-3d.plt", "w");
    
    if (pltOut == NULL)
    {
        return;
    }
    
    lat0 = latLaunch - 30.0;
    lat1 = latLaunch + 30.0;
    lon0 = lonLaunch - 20.0;
    lon1 = lonLaunch + 40.0;
    
    fprintf(pltOut, "#!/usr/bin/gnuplot -persist\n\n");
    fprintf(pltOut, "reset\n\n");
    fprintf(pltOut, "set xrange[%0.1f:%0.1f]\n", lon0, lon1);
    fprintf(pltOut, "set yrange[%0.1f:%0.1f]\n\n", lat0, lat1);
    //fprintf(pltOut, "l = %0.0f\n", timeEnd * 2);
    fprintf(pltOut, "load \"./Output/Gnuplot/launch-3d_base.plt\"\n");
    fprintf(pltOut, "#    EOF");

    fclose(pltOut);
}

void makeAltDownPlt(state apogee)
{
    FILE *pltOut = NULL;
    double dr = Downrange(apogee);
    double alt = Altitude(apogee);
    
    double xrange, yrange;
    
    if (dr > alt)
    {
        xrange = dr / 1000.0;
        yrange = dr / 1000.0;
    }
    else
    {
        xrange = alt / 1000.0;
        yrange = alt / 1000.0;
    }
    
    pltOut = fopen("Output/Gnuplot/tmp/ascent-alt-down.plt", "w");
    
    if (pltOut == NULL)
        return;
    
    fprintf(pltOut, "#!/usr/bin/gnuplot -persist\n\n");
    fprintf(pltOut, "reset\n\n");
    fprintf(pltOut, "set xrange[0:%0.1f]\n", xrange + 1);
    fprintf(pltOut, "set yrange[0:%0.1f]\n\n", yrange + 1);
    fprintf(pltOut, "load \"./Output/Gnuplot/ascent-alt-down_base.plt\"\n");
    fprintf(pltOut, "#    EOF");

    fclose(pltOut);
}

void makeOverviewPlt(state apogee, state burnout)
{
    FILE *pltOut = NULL;
    double dr, alt;
    double dr_apo = Downrange(apogee);
    double alt_apo = Altitude(apogee);
    double dr_burn = Downrange(burnout);
    double alt_burn = Altitude(burnout);
    double timeEnd = burnout.met + 100;
    double aspect = 1.6283;
    
    double xrange, yrange;
    
    if ( (dr_burn / dr_apo) < 0.1)
    {
        dr = dr_burn;
        alt = alt_burn;
    }
    
    if ((dr / aspect) > alt)
    {
        xrange = dr / 1000.0;
        yrange = dr / (1000.0 * aspect);
    }
    else
    {
        xrange = alt / (1000.0 / aspect);
        yrange = alt / 1000.0;
    }
    
    pltOut = fopen("Output/Gnuplot/tmp/overview.plt", "w");
    
    if (pltOut == NULL)
        return;
    
    fprintf(pltOut, "#!/usr/bin/gnuplot -persist\n\n");
    fprintf(pltOut, "reset\n\n");
    fprintf(pltOut, "set xrange[0:%0.1f]\n", xrange + 1);
    fprintf(pltOut, "set yrange[0:%0.1f]\n\n", yrange + 1);
    fprintf(pltOut, "l = %0.0f\n", timeEnd * 2);
    fprintf(pltOut, "load \"./Output/Gnuplot/overview_base.plt\"\n");
    fprintf(pltOut, "set autoscale y\n");
    fprintf(pltOut, "load \"./Output/Gnuplot/overview_vel.accel_base.plt\"\n");
    
    fprintf(pltOut, "#    EOF");

    fclose(pltOut);
}

void makeStageBurnPltHeader(FILE *pltOut)
{
    fprintf(pltOut, "#!/usr/bin/gnuplot -persist\n\n");
    fprintf(pltOut, "reset\n\n");

}


void makeStageBurnPltSection(FILE *pltOut, state ignition, state burnout, int stage)
{
    float xrange1, xrange2;
    float ignintionTime = ignition.met;
    float burnoutTime = burnout.met;
    char outName[256];
    
    sprintf(outName, "Output/burnout_%d.png", stage);
    
    xrange1 = ignintionTime;
    xrange2 = burnoutTime;
    
    fprintf(pltOut, "set xrange[%0.1f:%0.1f]\n", xrange1, xrange2);
    fprintf(pltOut, "set out \"%s\"\n\n", outName);
    fprintf(pltOut, "load \"./Output/Gnuplot/burn_base.plt\"\n\n");}

void makeStageBurnPltFooter(FILE *pltOut)
{
    fprintf(pltOut, "#EOF");
}

void DumpState(state dump)
{
    printf("State Dump:\n");
    printf("  s: %0.2f, %0.2f, %0.2f - Alt: %0.2f\n"
        , dump.s.i / 1000.0
        , dump.s.j / 1000.0
        , dump.s.k / 1000.0
        , Altitude(dump) / 1000.0);
    printf("  U: %0.2f, %0.2f, %0.2f - Vel: %0.2f\n"
        , dump.U.i
        , dump.U.j
        , dump.U.k
        , Velocity(dump));
    printf("  a: %0.2f, %0.2f, %0.2f\n"
        , dump.a.i
        , dump.a.j
        , dump.a.k);
    printf("  Fuel %0.2f:\n", dump.fuelMass);
    printf("  Time %0.2f:\n", dump.met);
    
}

void DumpDescription(stageDesc desc)
{
    int i;
    
    printf("  Stage %d Description:\n", desc.stage + 1);
    printf("    Empty Mass:      %0.2f\n", desc.emptyMass);
    printf("    Ignition Delay:  %0.1f\n", desc.ignitionDelay);
    printf("    Staging Delay:   %0.1f\n", desc.stageDelay);
    
    for (i = 0; i < desc.numOfMotors; i++)
    {
        motor m = desc.motors[i];
        printf("    Motor %d:\n", i + 1);
        printf("      Name: %s\n", m.name);
        printf("      Fuel Mass:  %0.2f\n", m.fuelMass);
        printf("      Isp:        %0.0f\n", m.isp);
    }
    
    for (i = 0; i < desc.numOfChutes; i++)
    {
        chute c = desc.chutes[i];
        printf("    Parachute %d:\n", i + 1);
        printf("      Cd:    %0.1f\n", c.cd);
        printf("      Area:  %0.1f\n", c.area);
    }
    
    printf("\n");
}

char nar(double impulse)
{
    if (impulse < 2.50)
        return 'A';
    else if (impulse < 5.00)
        return 'B';
    else if (impulse < 10.00)
        return 'C';
    else if (impulse < 20.00)
        return 'D';
    else if (impulse < 40.00)
        return 'E';
    else if (impulse < 80.00)
        return 'F';
    else if (impulse < 160.00)
        return 'G';
    else if (impulse < 320.00)
        return 'H';
    else if (impulse < 640.00)
        return 'I';
    else if (impulse < 1280.00)
        return 'J';
    else if (impulse < 2560.00)
        return 'K';
    else if (impulse < 5120.00)
        return 'L';
    else if (impulse < 10240.00)
        return 'M';
    else if (impulse < 20480.00)
        return 'N';
    else if (impulse < 40960.00)
        return 'O';
    else if (impulse < 81920.00)
        return 'P';
    else if (impulse < 163840.00)
        return 'Q';
    else if (impulse < 327680.00)
        return 'R';
    else if (impulse < 655360.00)
        return 'S';
    else if (impulse < 1310720.00)
        return 'T';
    else if (impulse < 2621440.00)
        return 'U';
    else if (impulse < 5242880.00)
        return 'V';
    else if (impulse < 10485760.00)
        return 'W';
    else if (impulse < 20971520.00)
        return 'X';
    else if (impulse < 41943040.00)
        return 'Y';
    else if (impulse < 83886080.00)
        return 'Z';
        
   return '-';
}

