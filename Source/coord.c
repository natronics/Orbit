#include <math.h>
#include "structs.h"
#include "vecmath.h"
#include "physics.h"
#include "coord.h"


double altitude(state r)
{
    return position(r) - Re;
}

double latitude(state r)
{
    double a = r.s[z]/position(r);
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

