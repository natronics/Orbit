#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "physics.h"
#include "coord.h"
#include "rout.h"

void printLine(state r, double t, double lat_0, double lon_0)
{
    char format[256] = "";
    char exp[8] = "%0.10e\t";
    double lat = degrees(latitude(r));
    double lon = degrees(longitude(r));
    
    strcat(format, "%0.4f  \t");      //1     Time
    strcat(format, exp);            //2     X
    strcat(format, exp);            //3     Y
    strcat(format, exp);            //4     Z
    strcat(format, exp);            //5     U_x
    strcat(format, exp);            //6     U_y
    strcat(format, exp);            //7     U_z
    strcat(format, exp);            //8     a_x
    strcat(format, exp);            //9     a_y
    strcat(format, exp);            //10    a_z
    strcat(format, exp);            //11    KE
    strcat(format, exp);            //12    PE
    strcat(format, "%0.6f\t");      //13    Lat
    strcat(format, "%0.6f\t");      //14    Lon
    strcat(format, exp);            //15    Alt
    strcat(format, "%0.10f\t");         //16    Downrange
    /*
    strcat(format, "%f\t");         //17    a
    strcat(format, "%f\t");         //18    e
    strcat(format, "%f\t");         //19    i
    strcat(format, "%f\t");         //20    omega
    strcat(format, "%f\t");         //21    ohm
    strcat(format, "%f");           //22    theta
    */
    strcat(format, "\n");
    
    printf( format
        ,   t                       //1     Time
        ,   r.s[x]                  //2     X
        ,   r.s[y]                  //3     Y
        ,   r.s[z]                  //4     Z
        ,   r.U[x]                  //5     U_x
        ,   r.U[y]                  //6     U_y
        ,   r.U[z]                  //7     U_z
        ,   r.a[x]                  //8     a_x
        ,   r.a[y]                  //9     a_y
        ,   r.a[z]                  //10    a_z
        ,   KE(r)                   //11    KE
        ,   PE(r)                   //12    PE
        ,   lat                     //13    Lat
        ,   lon                     //14    Lon
        ,   altitude(r)             //15    Alt
        ,   downrange(r, lat_0, lon_0));          //16    Downrange
}

void printHeader()
{
    char header[256] = "#";
    
    strcat(header, "Time(s)");                  //1
    strcat(header, "\tX(m)            ");       //2
    strcat(header, "\tY(m)            ");       //3
    strcat(header, "\tZ(m)            ");       //4
    strcat(header, "\tVel_x(m/s)      ");       //5
    strcat(header, "\tVel_y(m/s)      ");       //6
    strcat(header, "\tVel_z(m/s)      ");       //7
    strcat(header, "\tAccel_x(m/s^2)  ");       //8
    strcat(header, "\tAccel_y(m/s^2)  ");       //9
    strcat(header, "\tAccel_z(m/s^2)  ");       //10
    strcat(header, "\tKE(J)           ");      //11
    strcat(header, "\tPE(J)           ");      //12
    strcat(header, "\tLat(°)   ");      //13
    strcat(header, "\tLon(°)   ");      //14
    strcat(header, "\tAlt(m MSL)      ");      //15
    strcat(header, "\tDownrange(m)");  //16
    /*
    strcat(header, "\ta   ");       //17
    strcat(header, "\te   ");       //18
    strcat(header, "\ti   ");       //19
    strcat(header, "\tomega");      //20
    strcat(header, "\tOhm ");       //21
    strcat(header, "\tTheta");      //22
    */
    strcat(header, "\n");
    
    printf("%s", header);
}
