#include "structs.h"
#include "vecmath.h"
#include "physics.h"
#include "coord.h"
#include "orbit.h"

double Position(state r)
{
    return quad(r.s[x], r.s[y], r.s[z]);
}

double Velocity(state r)
{
    return quad(r.U[x], r.U[y], r.U[z]);
}

double Acceleration(state r)
{
    return quad(r.a[x], r.a[y], r.a[z]);
}

double altitude(state r)
{
    return Position(r) - Re;
}

double latitude(state r)
{
    double a = r.s[z]/Position(r);
    return PI/2.0 - acos(a);
}

double longitude(state r)
{
    double S, a, lon;
    
    S = sqrt(square(r.s[x]) + square(r.s[y]));
    a = r.s[y]/S;
    
    if (r.s[x] >= 0.0)
        lon = asin(a);
    else
        lon = PI - asin(a);
    return lon;
}

/**
 * Get a matix out that represents the local transformation from enu to ecef
 */
matrix3 enu(state r)
{
    matrix3 m3;
    
    // Make the call to lat. and lon. once
    double lat = latitude(r);
    double lon = longitude(r);
    
    // Make the calls to sin and cos a little as possible
    double sLat = sin(lat);
    double sLon = sin(lon);
    double cLat = cos(lat);
    double cLon = cos(lon);
    
    m3.m[0][0] = -sLon;
    m3.m[0][1] = cLon;
    m3.m[0][2] = 0;
    m3.m[1][0] = -sLat * cLon;
    m3.m[1][1] = -sLat * sLon;
    m3.m[1][2] = cLat;
    m3.m[2][0] = cLat * cLon;
    m3.m[2][1] = cLat * sLon;
    m3.m[2][2] = sLat;
    
    return m3;
}

vec ecefFromEnu(vec _enu, state r)
{
    matrix3 m;
    
    m = enu(r);
     
    return matrixMath(_enu, m);
}

vec matrixMath(vec v, matrix3 m)
{
    vec ret;
    
    ret.i = v.i*m.m[0][0]  + v.j*m.m[1][0] + v.k*m.m[2][0];
    ret.j = v.i*m.m[0][1]  + v.j*m.m[1][1] + v.k*m.m[2][1];
    ret.k = v.i*m.m[0][2]  + v.j*m.m[1][2] + v.k*m.m[2][2];
    
    return ret;
}

/**
 * rho = radius
 * theta = latitude : elevation
 * phi = longitude
 */
vec cartesian(double rho, double theta, double phi)
{
    vec cart;
    
    cart.i = rho * cos(phi) * sin(theta);
    cart.j = rho * sin(phi) * sin(theta);
    cart.k = rho * cos(theta);
    
    return cart;
}


/**
 * Law of spherical cosines:
 * http://www.movable-type.co.uk/scripts/latlong.html
 */
double downrange(state r)
{
    double lat2, lon2, a;
    double lat1, lon1;
    double d;
    state init;
    
    init = InitialRocket();
    
    lat1 = latitude(init);
    lon1 = longitude(init);
    
    lat2 = latitude(r);
    lon2 = longitude(r);
    
    a = sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(lon2-lon1);
    d = acos(a)*Re;
    
    return d;
}

double radians(double degrees)
{
    return (PI / 180.0) * degrees;
}

double degrees(double radians)
{
    double degree;

    degree = (180.0 / PI) * radians;
    if (degree > 180)
        return degree - 360;   
    return degree;
}

time_t JdToUnixTime(double JD)
{
    double unixTime;
    time_t unixT;
    
    unixTime = (JD - 2440587.5) * 86400;
    unixT = (time_t) unixTime;
    return unixT;
}

double SecondsToDecDay(double seconds)
{
    return seconds / 86400.0;
}

double DecDayToSeconds(double decDay)
{
    return decDay * 86400.0;
}
