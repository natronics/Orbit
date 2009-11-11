#include <math.h>
#include "structs.h"
#include "vecmath.h"
#include "coord.h"
#include "physics.h"

vec physics(state r, double t)
{
    vec g, d, th, physics;
    
    g = gravity(r);
    d = drag(r, t);
    th = thrust(r, t);
    
    physics.i = g.i + d.i + th.i;
    physics.j = g.j + d.j + th.j;
    physics.k = g.k + d.k + th.k;
    
    return physics;
}

vec gravity(state r)
{
    vec g, e;
    double gravity;
    
    gravity = G * (Me/square(position(r)));
    e = unitVec(r.s);

    g.i = gravity * e.i;
    g.j = gravity * e.j;
    g.k = gravity * e.k;

    return g;
}

vec drag(state r , double t)
{
    vec d, v;
    double Cd = 1.0;
    double A = 0.01539;
    double totalDrag, h;
    
    // Cd for closed module: 1
    // Droge Cd 0.62
    // Main Cd 0.62
    
    // A for closed: 0.01539m^2
    // A for Droge: 1.04m^2
    // A for Main: 14.4
    
    /*
    v = unitVec(r.U);
    h = altitude(r);
    
    if (h >= 594.3)
    {
        A = 0.016;
        Cd = 1.0;
    }
    else if (h >= 365.75)
    {
        A = 1.04;
        Cd = 0.62;
    }
    else 
    {
        A = 14.4;
        Cd = 0.62;
    }
    
    totalDrag = -(0.5 * rho(h) * velocity(r)*velocity(r) * A  * Cd)/r.m;
    
    d.i = totalDrag * v.i;
    d.j = totalDrag * v.j;
    d.k = totalDrag * v.k;
    
    */
    
    d.i = 0.0;
    d.j = 0.0;
    d.k = 0.0;

    return d;
}

vec thrust(state r, double t)
{
    /*
    vec Ft, earth;
    double thrust, tilt, i = 68.0;
    
    earth = unitVev(r.s);
    tilt = 0.0;
    
    Ft.i = 0.0;
    Ft.j = 0.0;
    Ft.k = 0.0;
    
    if (t < 500.0)
    {
        thrust = 30.0;
        
        Ft.k = 20.0;
    }
    return Ft;
    */
    vec Ft;
    Ft.i = 0.0;
    Ft.j = 0.0;
    Ft.k = 0.0;
    return Ft;
}

double rho(double h)
{
    double p, T, rho, R = 287.05;
    
    p = 100*pow((44331.5 - h)/11880.516, 1.0/0.190263);
    T = zTemperature(h) + 273; //K
    
    //return 1.293;
    rho = p/(R*T);
    
    if (rho > 1.0e-5)
        return rho;
    return 0.0;
}

double zTemperature(double h)
{
    if (h < 11.019e3)
        return -0.0065*h + 15.0;
    if (h >= 11.019e3 && h < 20.063e3)
        return -56.5;
    if (h >= 20.063e3 && h < 32.162e3)
        return 0.001*h - 76.563;
    if (h >= 32.162e3 && h < 47.350e3)
        return 0.0028*h - 134.554;
    if (h >= 47.350e3 && h < 51.413e3)
        return -1.974;
    if (h >= 51.413e3 && h < 71.802e3)
        return -0.0028*h + 141.982;
   return -100.0;
}


double velocity(state r)
{
    return quad(r.U[x], r.U[y], r.U[z]);
}

double position(state r)
{
    return quad(r.s[x], r.s[y], r.s[z]);
}

double acceleration(state r)
{
    return quad(r.a[x], r.a[y], r.a[z]);
}

double KE(state r)
{
    return 0.5 * r.m * square(velocity(r));
}

double PE(state r)
{
    return (G * Me * r.m)/position(r) - (G * Me * r.m)/Re;
}

/**
 * Law of spherical cosines:
 * http://www.movable-type.co.uk/scripts/latlong.html
 */
double downrange(state r, double lat1, double lon1)
{
    double lat2, lon2, a;
    double d;
    
    lat2 = latitude(r);
    lon2 = longitude(r);
    
    a = sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(lon2-lon1);
    d = acos(a)*Re;
    
    return d;
}
