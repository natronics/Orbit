/*! 
 * \file rout.c
 * \brief Handles output
 *  
 * Has several functions called by the main program to assist in writting 
 * output both to the screen and to file.
 */
#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "physics.h"
#include "coord.h"
#include "orbit.h"
#include "rout.h"


void printHtmlFileHeader(FILE *out);
void printHtmlHeader(FILE *out, char *header);
void printHtmlFileFooter(FILE *out);
void printHtmlImage(FILE *out, char *img);

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
    /*
    strcat(format, "%f\t");         //17    a
    strcat(format, "%f\t");         //18    e
    strcat(format, "%f\t");         //19    i
    strcat(format, "%f\t");         //20    omega
    strcat(format, "%f\t");         //21    ohm
    strcat(format, "%f");           //22    theta
    */
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
        ,   totalMass(r.m)          //12    mass
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
    strcat(header, "\tModified JD     ");       //1
    strcat(header, "\tX(m)            ");       //2
    strcat(header, "\tY(m)            ");       //3
    strcat(header, "\tZ(m)            ");       //4
    strcat(header, "\tVel_x(m/s)      ");       //5
    strcat(header, "\tVel_y(m/s)      ");       //6
    strcat(header, "\tVel_z(m/s)      ");       //7
    strcat(header, "\tAccel_x(m/s^2)  ");       //8
    strcat(header, "\tAccel_y(m/s^2)  ");       //9
    strcat(header, "\tAccel_z(m/s^2)  ");       //10
    strcat(header, "\tMass            ");       //11
    strcat(header, "\tKE(J)           ");       //12
    strcat(header, "\tPE(J)           ");       //13
    strcat(header, "\tLat(°)         ");          //14
    strcat(header, "\tLon(°)         ");          //15
    strcat(header, "\tAlt(m MSL)     ");       //16
    strcat(header, "\tDownrange(m)");           //17
    /*
    strcat(header, "\ta   ");       //17
    strcat(header, "\te   ");       //18
    strcat(header, "\ti   ");       //19
    strcat(header, "\tomega");      //20
    strcat(header, "\tOhm ");       //21
    strcat(header, "\tTheta");      //22
    */
    strcat(header, "\n");
    
    fprintf(outfile, "%s", header);
}

void PrintResult(state burnout, state apogee, double t_bo, double t_apogee)
{
    TimeHMS beginUtc = MjdToUtc(BeginTime());
    TimeHMS beginCivil = MjdToCivilTime(BeginTime(), -8);
    TimeHMS burnoutUtc = MjdToUtc(t_bo);
    TimeHMS apogeeUtc = MjdToUtc(t_apogee);
    double burnTime = DecDayToSeconds(t_bo - BeginTime());
    double coastTime = DecDayToSeconds(t_apogee - t_bo);
    
    printf("%s\t%2d:%02d:%05.2f %s\n", "\t       Begin Time: "
                                        , beginUtc.hour
                                        , beginUtc.minute
                                        , beginUtc.second
                                        , "(UTC)");
    printf("%s\t%2d:%02d:%05.2f %s\n", "\t                   "
                                        , beginCivil.hour
                                        , beginCivil.minute
                                        , beginCivil.second
                                        , "(local)");
    printf("\n");
    
    printf("%s%16.2f %s\n", "\t        Burn Time: ", burnTime, "s");
    printf("%s\t%2d:%02d:%05.2f %s\n", "\t          Burnout: "
                                        , burnoutUtc.hour
                                        , burnoutUtc.minute
                                        , burnoutUtc.second
                                        , "(UTC)");
    printf("%s%16.2f %s\n", "\t Burnout Velocity: ", velocity(burnout), "m/s");
    printf("%s%16.2f %s\n", "\t Burnout Altitude: ", altitude(burnout) / 1000.0, "km");
    printf("%s%16.2f %s\n", "\tBurnout Downrange: ", downrange(burnout) / 1000.0, "km");
    printf("\n");
    printf("%s%16.2f %s\n", "\t       Coast Time: ", coastTime, "s");
    printf("%s\t%2d:%02d:%05.2f %s\n", "\t       Apogee Time: "
                                        , apogeeUtc.hour
                                        , apogeeUtc.minute
                                        , apogeeUtc.second
                                        , "(UTC)");
    printf("%s%16.2f %s\n", "\t  Apogee Velocity: ", velocity(apogee), "m/s");
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

void PrintHtmlResult(state burnout, state apogee, double t_bo, double t_apogee)
{
    FILE *htmlOut = NULL;
    state initRocket = initialRocket(); 
    double lat = degrees(latitude(initRocket));
    double lon = degrees(longitude(initRocket));
    double massRatio = totalMass(initRocket.m) / initRocket.m.structure;
    double TTWRatio = (g_0 * I_sp() * mdot()) / (totalMass(initRocket.m) * g_0);
    TimeHMS beginUtc = MjdToUtc(BeginTime());
    TimeHMS beginCivil = MjdToCivilTime(BeginTime(), -8);
    TimeHMS burnoutUtc = MjdToUtc(t_bo);
    double burnTime = DecDayToSeconds(t_bo - BeginTime());
    TimeHMS apogeeUtc = MjdToUtc(t_apogee);
    double coastTime = DecDayToSeconds(t_apogee - t_bo);
    
    htmlOut = fopen("Output/result.html", "w");
    
    if (htmlOut == NULL)
    {
        return;
    }
    
    printHtmlFileHeader(htmlOut);
    
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
    fprintf(htmlOut, "        <td>%0.2f</td>\n", totalMass(initRocket.m));
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
    fprintf(htmlOut, "        <td>%0.2f</td>\n", totalMass(initRocket.m));
    fprintf(htmlOut, "        <td>%0.1f</td>\n", massRatio);
    fprintf(htmlOut, "        <td>%0.1f</td>\n", TTWRatio);
    fprintf(htmlOut, "      </tr>\n");
    fprintf(htmlOut, "    </tfoot>\n");
    fprintf(htmlOut, "  </table>\n");
    
    /* Liftoff Location Configuration Table */
    fprintf(htmlOut, "  <table class=\"data_table\" >\n");
    fprintf(htmlOut, "    <thead>\n");
    fprintf(htmlOut, "      <th>Launch Time [UTC]</th>\n");
    fprintf(htmlOut, "      <th>Launch Time [local]</th>\n");
    fprintf(htmlOut, "      <th>Location</th>\n");
    fprintf(htmlOut, "      <th>Launch Angle [&deg;]</th>\n");
    fprintf(htmlOut, "    </thead>\n");
    fprintf(htmlOut, "    <tr>\n");
    fprintf(htmlOut, "      <td><span class=\"time\">%2d:%02d:%05.2f</class></td>\n"
                            , beginUtc.hour
                            , beginUtc.minute
                            , beginUtc.second);
    fprintf(htmlOut, "      <td><span class=\"time\">%2d:%02d:%05.2f</class></td>\n"
                            , beginCivil.hour
                            , beginCivil.minute
                            , beginCivil.second);
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
    fprintf(htmlOut, "      <td><span class=\"time\">%2d:%02d:%05.2f</class></td>\n"
                            , burnoutUtc.hour
                            , burnoutUtc.minute
                            , burnoutUtc.second);
    fprintf(htmlOut, "      <td>%0.2f</td>\n", burnTime);
    fprintf(htmlOut, "      <td>%0.2f</td>\n", velocity(burnout));
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
    fprintf(htmlOut, "      <td><span class=\"time\">%2d:%02d:%05.2f</class></td>\n"
                            , apogeeUtc.hour
                            , apogeeUtc.minute
                            , apogeeUtc.second);
    fprintf(htmlOut, "      <td>%0.2f</td>\n", coastTime);
    fprintf(htmlOut, "      <td>%0.2f</td>\n", velocity(apogee));
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
    fprintf(out, "  <title>Rocket Analysis</title>\n");
    fprintf(out, "  <link href=\"css/style.css\" rel=\"stylesheet\" type=\"text/css\" media=\"all\" />\n");
    fprintf(out, "</head>\n");
    fprintf(out, "<body>\n");
    fprintf(out, "  <h1>Rocket Analysis</h1>\n");
    fprintf(out, "  <hr />\n");
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
    fprintf(out, "</body>\n");
    fprintf(out, "</html>\n");
}

