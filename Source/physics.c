#include <math.h>
#include <stdio.h>
#include "structs.h"
#include "vecmath.h"
#include "coord.h"
#include "orbit.h"
#include "physics.h"

double currentMass;

vec force_Gravity(state r);
double rho(double h);
double zTemperature(double h);

vec LinearAcceleration(state r, double t)
{
    vec g, d, th, physics;
    currentMass = RocketMass(r, t);
    
    g = force_Gravity(r);
    d = Force_Drag(r, t);
    th = Force_Thrust(r, t);
    
    physics.i = (g.i + d.i + th.i) / currentMass;
    physics.j = (g.j + d.j + th.j) / currentMass;
    physics.k = (g.k + d.k + th.k) / currentMass;
    
    return physics;
}

vec AngularAcceleration(state r, double t)
{
    vec alpha;
    
    alpha.i = 0;
    alpha.j = 0;
    alpha.k = 0;
    
    return alpha;
}

vec force_Gravity(state r)
{
    vec g, e;
    double gravity;
    
    gravity = G * ((Me * currentMass)/Square(Position(r)));
    e = UnitVec(r.s);

    g.i = gravity * e.i;
    g.j = gravity * e.j;
    g.k = gravity * e.k;

    return g;
}

vec Force_Drag(state r , double t)
{
    vec d, v;
    double Cd = 0.75;
    double A = 0.01539;
    double totalDrag, h;
    
    v = UnitVec(r.U);
    h = Altitude(r);
    
    totalDrag = -(0.5 * rho(h) * Velocity(r)*Velocity(r) * A  * Cd);
    
    d.i = totalDrag * v.i;
    d.j = totalDrag * v.j;
    d.k = totalDrag * v.k;
    
    //d = ZeroVec();
     
    return d;
}

/**
 * Thrust on the rocket
 */
vec Force_Thrust(state r, double t)
{
    vec Ft;
    vec Ft_enu, Ft_ecef;
    vec velocityHat;
    double thrust;
    double phi;
    double rate = radians(40.0) / 100.0;

    Ft = ZeroVec();

    thrust = r.m.thrust;
    phi = radians(3.0);
    
    if (CurrentStage().mode == BURNING)
    {
        Ft_enu.i = thrust * sin(phi);
        Ft_enu.j = 0.0;
        Ft_enu.k = thrust * cos(phi);

        Ft_ecef = EnuToEcef(Ft_enu, r);
        Ft = Ft_ecef;
        
        if ( Norm(r.U) > 50.0 ) 
        {
            velocityHat = UnitVec(r.U);
            
            Ft.i = thrust * velocityHat.i;
            Ft.j = thrust * velocityHat.j;
            Ft.k = thrust * velocityHat.k;
        }
    }

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

double KE(state r, double met)
{
    return 0.5 * RocketMass(r, met) * Square(Velocity(r));
}

double PE(state r, double met)
{
    return (G * Me * RocketMass(r, met))/Position(r) - (G * Me * RocketMass(r, met))/Re;
}

double RocketMass(state r, double met)
{
    Rocket_Stage currentStage = CurrentStage();
    Rocket_Stage *rocket = WholeRocket();
    double totalMass = 0;
    int i;
    
    // If we are atteched to the rocket
    if (currentStage.mode < SEPARATED)
    {
        // Get attached stages
        for (i = currentStage.description.stage; i < NumberOfStages(); i++)
        {
            // Add empty mass's
            totalMass += rocket[i].description.emptyMass;
            // Get the fuel of the unlit stages
            if (i > currentStage.description.stage)
            {   
                totalMass += rocket[i].initialState.m.fuelMass;
            }
        }
    }
    else
    {
        totalMass += currentStage.description.emptyMass;
    }
    
    // Get Current fuel mass
    totalMass += r.m.fuelMass;
    
    return totalMass;
}

