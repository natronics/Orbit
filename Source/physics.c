#include <math.h>
#include <stdio.h>
#include "structs.h"
#include "vecmath.h"
#include "coord.h"
#include "orbit.h"
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
    double Cd = 0.75;
    double A = 0.01539;
    double totalDrag, h;
    
    /*
    v = unitVec(r.U);
    h = altitude(r);
    
    totalDrag = -(0.5 * rho(h) * velocity(r)*velocity(r) * A  * Cd)/totalMass(r.m);
    
    d.i = totalDrag * v.i;
    d.j = totalDrag * v.j;
    d.k = totalDrag * v.k;
    
    */
    d.i = 0.0;
    d.j = 0.0;
    d.k = 0.0;
    
    
    return d;
}

/**
 * Thrust on the rocket
 */
vec thrust(state r, double t)
{
    vec Ft;
    vec Ft_enu, Ft_ecef;
    double thrust;
    double phi;
    double rate = radians(40.0) / 100.0;

    if (r.mode == BURNING)
    {
        //phi = rate * t + phi_init;
        phi = radians(6.0);
        thrust =  g_0 * I_sp() * mdot();
        
        thrust = thrust / totalMass(r.m);
        
        Ft_enu.i = thrust * sin(phi);
        Ft_enu.j = 0.0;
        Ft_enu.k = thrust * cos(phi);
        
        Ft_ecef = ecefFromEnu(Ft_enu, r);
        
        Ft.i = Ft_ecef.i;
        Ft.j = Ft_ecef.j;
        Ft.k = Ft_ecef.k;
    }
    else
    {
        Ft.i = 0.0;
        Ft.j = 0.0;
        Ft.k = 0.0;
    }
    return Ft;
}

double updateFuelMass(state r, double t)
{
    double newFuelMass;
    double currentFuelMass = r.m.fuel;
    
    if (currentFuelMass > 0.0)
    {
        newFuelMass = initialRocket().m.fuel - mdot()*t;
        if (newFuelMass > 0.0)
        {
            return newFuelMass;
        }
        else
        {
            setNewMode(COASING);
            return 0.0;
        }
    }
    
    return r.m.fuel;
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
    return 0.5 * totalMass(r.m) * square(velocity(r));
}

double PE(state r)
{
    return (G * Me * totalMass(r.m))/position(r) - (G * Me * totalMass(r.m))/Re;
}

double totalMass(mass m)
{
    return m.fuel + m.structure;
    //return 10.0;
}

