/*! 
 * \file rout.c
 * \brief Handles output
 *  
 * Has several functions called by the main program to assist in writting 
 * output both to the screen and to file.
 */
#include <stdio.h>
#include <string.h>
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
void makeLaunchMapPlt();

void PrintLine(FILE *outfile, double Mjd, double t, state r)
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
    strcat(format, exp);            //10     a_y
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
        ,   t                       //1     Time MET
        ,   Mjd                     //2     Time MJD
        ,   r.s[x]                  //3     X
        ,   r.s[y]                  //4     Y
        ,   r.s[z]                  //5     Z
        ,   r.U[x]                  //6     U_x
        ,   r.U[y]                  //7     U_y
        ,   r.U[z]                  //8     U_z
        ,   r.a[x]                  //9     a_x
        ,   r.a[y]                  //10    a_y
        ,   r.a[z]                  //11    a_z
        ,   TotalMass(r.m)          //12    mass
        ,   KE(r)                   //13    KE
        ,   PE(r)                   //14    PE
        ,   lat                     //15    Lat
        ,   lon                     //16    Lon
        ,   altitude(r)             //17    Alt
        ,   downrange(r));          //18    Downrange
        
}

void PrintHeader(FILE *outfile)
{
    char header[512] = "#";
    
    strcat(header, "Time(s)");                  //1
    strcat(header, "\tModified JD     ");       //2
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

void PrintForceLine(FILE *outfile, double jd, double t, state r)
{ 
    char format[512] = "";
    char exp[8] = "%0.10e\t";
    vec thrust = Force_Thrust(r,t);

    strcat(format, "%0.4f  \t");    //1     Time MET
    strcat(format, "%0.8f  \t");    //2     Time JD
    strcat(format, exp);            //3     Thrust_x
    strcat(format, exp);            //4     Thrust_y
    strcat(format, exp);            //5     Thrust_z
    strcat(format, "\n");
  
    fprintf(outfile, format
        ,   t                       //1     Time MET
        ,   jd                      //2     Time JD
        ,   thrust.i                //3     Thrust_x
        ,   thrust.j                //4     Thrust_y
        ,   thrust.k);              //5     Thrust_z
     
}

void PrintResult(state burnout, state apogee, double t_bo, double t_apogee)
{
    double burnTime = DecDayToSeconds(t_bo - BeginTime());
    double coastTime = DecDayToSeconds(t_apogee - t_bo);
    
    char launchTimeStringUtc[256];
    char launchTimeStringLoc[256];
    char burnoutTimeStringUtc[256];
    char apogeeTimeStringUtc[256];
    char localTimeZone[16];
    time_t launchTime;
    time_t burnoutTime;
    time_t apogeeTime;
    struct tm *launchTimeUtc;
    struct tm *launchTimeLoc;
    struct tm *burnoutTimeUtc;
    struct tm *apogeeTimeUtc;
    
    /* Get the times in unix time */
    launchTime      = JdToUnixTime(BeginTime());
    burnoutTime     = JdToUnixTime(t_bo);
    apogeeTime      = JdToUnixTime(t_apogee);

    /* Convert it to local or utc time representation */
    launchTimeUtc   = gmtime(&launchTime);
    strftime(launchTimeStringUtc, 256, "%m/%d/%Y %H:%M:%S", launchTimeUtc);
    
    launchTimeLoc   = localtime(&launchTime);
    strftime(launchTimeStringLoc, 256, "%m/%d/%Y %H:%M:%S", launchTimeLoc);
    
    burnoutTimeUtc  = gmtime(&burnoutTime);
    strftime(burnoutTimeStringUtc, 256, "%H:%M:%S", burnoutTimeUtc);
    
    apogeeTimeUtc   = gmtime(&apogeeTime);
    strftime(apogeeTimeStringUtc, 256, "%H:%M:%S", apogeeTimeUtc);
        
    launchTimeLoc      = localtime(&launchTime);
    strftime(localTimeZone, 16, "%Z", launchTimeLoc);

    
    printf("%s\t%s %s\n", "\t     Launch Time: ", launchTimeStringUtc, "[UTC]");
    printf("%s\t%s [%s]\n", "\t                   ", launchTimeStringLoc, localTimeZone);
    printf("\n");
    
    printf("%s%16.2f %s\n", "\t        Burn Time: ", burnTime, "s");
    printf("%s\t   %s %s\n","\t          Burnout: ", burnoutTimeStringUtc, "[UTC]");
    printf("%s%16.2f %s\n", "\t Burnout Velocity: ", Velocity(burnout), "m/s");
    printf("%s%16.2f %s\n", "\t Burnout Altitude: ", altitude(burnout) / 1000.0, "km");
    printf("%s%16.2f %s\n", "\tBurnout Downrange: ", downrange(burnout) / 1000.0, "km");
    printf("\n");
    printf("%s%16.2f %s\n", "\t       Coast Time: ", coastTime, "s");
    printf("%s\t   %s %s\n","\t           Apogee: ", apogeeTimeStringUtc, "[UTC]");
    printf("%s%16.2f %s\n", "\t  Apogee Velocity: ", Velocity(apogee), "m/s");
    printf("%s%16.2f %s\n", "\t  Apogee Altitude: ", altitude(apogee) / 1000.0, "km");
    printf("%s%16.2f %s\n", "\t Apogee Downrange: ", downrange(apogee) / 1000.0, "km");
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
    fprintf(outfile, "          %f,%f,%#.1f\n", lon, lat, altitude(r));
}

void PrintHtmlResult(state burnout, state apogee, double t_bo, double t_apogee, double runTime)
{
    FILE *htmlOut = NULL;
    state initRocket = InitialRocket(); 
    double lat = degrees(latitude(initRocket));
    double lon = degrees(longitude(initRocket));
    double massRatio = TotalMass(initRocket.m) / initRocket.m.structure;
    double TTWRatio = (g_0 * I_sp() * mdot()) / (TotalMass(initRocket.m) * g_0);
    double burnTime = DecDayToSeconds(t_bo - BeginTime());
    double coastTime = DecDayToSeconds(t_apogee - t_bo);
    
    char runTimeStringLoc[256];
    char launchTimeStringUtc[256];
    char launchTimeStringLoc[256];
    char burnoutTimeStringUtc[256];
    char apogeeTimeStringUtc[256];
    char localTimeZone[16];
    time_t curtime;
    time_t launchTime;
    time_t burnoutTime;
    time_t apogeeTime;
    struct tm *runTimeLoc;
    struct tm *launchTimeUtc;
    struct tm *launchTimeLoc;
    struct tm *burnoutTimeUtc;
    struct tm *apogeeTimeUtc;
    
    /* Get the times in unix time */
    curtime         = time(NULL);
    launchTime      = JdToUnixTime(BeginTime());
    burnoutTime     = JdToUnixTime(t_bo);
    apogeeTime      = JdToUnixTime(t_apogee);

    /* Convert it to local or utc time representation */
    runTimeLoc      = localtime(&curtime);
    strftime(runTimeStringLoc, 256, "%m/%d/%Y %H:%M:%S [%Z]", runTimeLoc);
    
    launchTimeUtc   = gmtime(&launchTime);
    strftime(launchTimeStringUtc, 256, "%m/%d/%Y %H:%M:%S", launchTimeUtc);
    
    launchTimeLoc   = localtime(&launchTime);
    strftime(launchTimeStringLoc, 256, "%m/%d/%Y %H:%M:%S", launchTimeLoc);
    
    burnoutTimeUtc  = gmtime(&burnoutTime);
    strftime(burnoutTimeStringUtc, 256, "%H:%M:%S", burnoutTimeUtc);
    
    apogeeTimeUtc   = gmtime(&apogeeTime);
    strftime(apogeeTimeStringUtc, 256, "%H:%M:%S", apogeeTimeUtc);
    
    launchTimeLoc   = localtime(&launchTime);
    strftime(localTimeZone, 16, "%Z", launchTimeLoc);
    
    htmlOut = fopen("Output/result.html", "w");
    
    if (htmlOut == NULL)
    {
        return;
    }
    
    printHtmlFileHeader(htmlOut);
    
    printHtmlHeader(htmlOut, "Run Details");
    fprintf(htmlOut, "  <p>Simulation run on %s and took %0.2f seconds</p>\n"
                ,   runTimeStringLoc
                ,   runTime);   
    
    printHtmlHeader(htmlOut, "Liftoff Configuration");

    /* Liftoff Mass Configuration Table */
    fprintf(htmlOut, "  <table class=\"data_table\" >\n");
    fprintf(htmlOut, "    <thead>\n");
    fprintf(htmlOut, "      <tr>\n");
    fprintf(htmlOut, "        <th>Stage [#]</th>\n");
    fprintf(htmlOut, "        <th>I<sub>sp</sub> [s]</th>\n");
    fprintf(htmlOut, "        <th>Fuel Mass [kg]</th>\n");
    fprintf(htmlOut, "        <th>Dry Mass [kg]</th>\n");
    fprintf(htmlOut, "        <th>Total Mass [kg]</th>\n");
    fprintf(htmlOut, "        <th>Mass Ratio</th>\n");
    fprintf(htmlOut, "        <th>Thrust to Weight Ratio</th>\n");
    fprintf(htmlOut, "      <tr>\n");
    fprintf(htmlOut, "    </thead>\n");
    fprintf(htmlOut, "    <tbody>\n");
    fprintf(htmlOut, "      <tr>\n");
    fprintf(htmlOut, "        <td>1</td>\n");
    fprintf(htmlOut, "        <td>%0.0f</td>\n", I_sp());
    fprintf(htmlOut, "        <td>%0.2f</td>\n", initRocket.m.fuel);
    fprintf(htmlOut, "        <td>%0.2f</td>\n", initRocket.m.structure);
    fprintf(htmlOut, "        <td>%0.2f</td>\n", TotalMass(initRocket.m));
    fprintf(htmlOut, "        <td>%0.1f</td>\n", massRatio);
    fprintf(htmlOut, "        <td>%0.1f</td>\n", TTWRatio);
    fprintf(htmlOut, "      </tr>\n");
    fprintf(htmlOut, "    </tbody>\n");
    fprintf(htmlOut, "    <tfoot class=\"total\">\n");
    fprintf(htmlOut, "      <tr>\n");
    fprintf(htmlOut, "        <td>Total</td>\n");
    fprintf(htmlOut, "        <td>&mdash;</td>\n");
    fprintf(htmlOut, "        <td>%0.2f</td>\n", initRocket.m.fuel);
    fprintf(htmlOut, "        <td>%0.2f</td>\n", initRocket.m.structure);
    fprintf(htmlOut, "        <td>%0.2f</td>\n", TotalMass(initRocket.m));
    fprintf(htmlOut, "        <td>%0.1f</td>\n", massRatio);
    fprintf(htmlOut, "        <td>%0.1f</td>\n", TTWRatio);
    fprintf(htmlOut, "      </tr>\n");
    fprintf(htmlOut, "    </tfoot>\n");
    fprintf(htmlOut, "  </table>\n");
    
    /* Liftoff Location Configuration Table */
    fprintf(htmlOut, "  <table class=\"data_table\" >\n");
    fprintf(htmlOut, "    <thead>\n");
    fprintf(htmlOut, "      <th>Launch Time [UTC]</th>\n");
    fprintf(htmlOut, "      <th>Launch Time [%s]</th>\n", localTimeZone);
    fprintf(htmlOut, "      <th>Location</th>\n");
    fprintf(htmlOut, "      <th>Launch Angle [&deg;]</th>\n");
    fprintf(htmlOut, "    </thead>\n");
    fprintf(htmlOut, "    <tr>\n");
    fprintf(htmlOut, "      <td><span class=\"time\">%s</span></td>\n"
                            , launchTimeStringUtc);
    fprintf(htmlOut, "      <td><span class=\"time\">%s</span></td>\n"
                            ,launchTimeStringLoc);
    fprintf(htmlOut, "      <td><span class=\"latlon\">%+0.5f, %+0.5f</span></td>\n", lat, lon);
    fprintf(htmlOut, "      <td>&mdash;</td>\n");
    fprintf(htmlOut, "    </tr>\n");
    fprintf(htmlOut, "  </table>\n");
    
    printHtmlHeader(htmlOut, "Burnout (stage 1)");
    
    /* Burnout */
    fprintf(htmlOut, "  <table class=\"data_table\" >\n");
    fprintf(htmlOut, "    <thead>\n");
    fprintf(htmlOut, "      <th>Burnout Time [UTC]</th>\n");
    fprintf(htmlOut, "      <th>Burn Time [s]</th>\n");
    fprintf(htmlOut, "      <th>Burnout Velocity [m/s]</th>\n");
    fprintf(htmlOut, "      <th>Burnout Altitude [km]</th>\n");
    fprintf(htmlOut, "      <th>Burnout Downrange [km]</th>\n");
    fprintf(htmlOut, "    </thead>\n");
    fprintf(htmlOut, "    <tr>\n");
    fprintf(htmlOut, "      <td><span class=\"time\">%s</span></td>\n"
                            , burnoutTimeStringUtc);
    fprintf(htmlOut, "      <td>%0.2f</td>\n", burnTime);
    fprintf(htmlOut, "      <td>%0.2f</td>\n", Velocity(burnout));
    fprintf(htmlOut, "      <td>%0.2f</td>\n", altitude(burnout) / 1000.0);
    fprintf(htmlOut, "      <td>%0.2f</td>\n", downrange(burnout) / 1000.0);
    fprintf(htmlOut, "    </tr>\n");
    fprintf(htmlOut, "  </table>\n");
    
    printHtmlHeader(htmlOut, "Apogee");
    
    fprintf(htmlOut, "  <table class=\"data_table\" >\n");
    fprintf(htmlOut, "    <thead>\n");
    fprintf(htmlOut, "      <tr>\n");
    fprintf(htmlOut, "        <th>Apogee Time [UTC]</th>\n");
    fprintf(htmlOut, "        <th>Coast Time [s]</th>\n");
    fprintf(htmlOut, "        <th>Apogee Velocity [m/s]</th>\n");
    fprintf(htmlOut, "        <th>Apogee Altitude [km]</th>\n");
    fprintf(htmlOut, "        <th>Apogee Downrange [km]</th>\n");
    fprintf(htmlOut, "      </tr>\n");
    fprintf(htmlOut, "    </thead>\n");
    fprintf(htmlOut, "    <tr>\n");
    fprintf(htmlOut, "      <td><span class=\"time\">%s</span></td>\n"
                            , apogeeTimeStringUtc);
    fprintf(htmlOut, "      <td>%0.2f</td>\n", coastTime);
    fprintf(htmlOut, "      <td>%0.2f</td>\n", Velocity(apogee));
    fprintf(htmlOut, "      <td>%0.2f</td>\n", altitude(apogee) / 1000.0);
    fprintf(htmlOut, "      <td>%0.2f</td>\n", downrange(apogee) / 1000.0);
    fprintf(htmlOut, "    </tr>\n");
    fprintf(htmlOut, "  </table>\n");
    //printHtmlImage(htmlOut, "ascent.png");
    
    printHtmlImage(htmlOut, "worldmap.png");
    printHtmlImage(htmlOut, "launchmap.png");
    
    printHtmlFileFooter(htmlOut);
    
    fclose(htmlOut);
}

void printHtmlFileHeader(FILE *out)
{
    fprintf(out, "<html>\n");
    fprintf(out, "<head>\n");
    fprintf(out, "  <title>Rocket Flight Analysis</title>\n");
    fprintf(out, "  <link href=\"css/style.css\" rel=\"stylesheet\" type=\"text/css\" media=\"all\" />\n");
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

void printHtmlFileFooter(FILE *out)
{
    fprintf(out, "</div>\n");
    fprintf(out, "</body>\n");
    fprintf(out, "</html>\n");
}

void MakePltFiles()
{
    makeLaunchMapPlt();
    //makeAscentPlt();
}

void makeLaunchMapPlt()
{
    FILE *pltOut = NULL;
    double lat0, lon0, lat1, lon1;
    double latLaunch, lonLaunch;
    state initRocket = InitialRocket();
    
    latLaunch = degrees(latitude(initRocket));
    lonLaunch = degrees(longitude(initRocket));
    
    lat0 = latLaunch - 5.0;
    lat1 = latLaunch + 5.0;
    lon0 = lonLaunch - 5.0;
    lon1 = lonLaunch + 5.0;
    
    pltOut = fopen("Output/Gnuplot/tmp/launchmap.plt", "w");
    
    if (pltOut == NULL)
    {
        return;
    }
    
    fprintf(pltOut, "#!/usr/bin/gnuplot -persist\n\n");
    fprintf(pltOut, "reset\n\n");
    fprintf(pltOut, "set xrange[%0.1f:%0.1f]\n", lon0, lon1);
    fprintf(pltOut, "set yrange[%0.1f:%0.1f]\n\n", lat0, lat1);
    fprintf(pltOut, "load \"./Output/Gnuplot/launchmap_base.plt\"\n");
    fprintf(pltOut, "#    EOF");
    
    fclose(pltOut);
}
