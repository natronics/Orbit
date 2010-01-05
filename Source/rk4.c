#include <stdio.h>
#include "structs.h"
#include "physics.h"
#include "vecmath.h"
#include "orbit.h"
#include "rk4.h"

#define ERR 1.0e-6
#define DOF 3

state updateState(state r, int previousStep, double point);
void stateToRk4(state r, double t);

double secondOrderDE[4][DOF];    //k, p, u
double firstOrderDE[4][DOF];     //l, q, v
double secondOrder[DOF];        //Vel
double firstOrder[DOF];         //Accel
double vel_n[DOF];
double pos_n[DOF];
double vel_n1[DOF];
double pos_n1[DOF];

/*!
 * A generic fourth-order Runge-Kutta numerical integration engine for second 
 * order differential equations.You are not expected to understand this.
 */
state rk4(state r, float h, double t)
{
    int i;
    double average;
    
    // Define Degrees of Freedom
    vel_n[0] = r.U.i;
    vel_n[1] = r.U.j;
    vel_n[2] = r.U.k;
    pos_n[0] = r.s.i;
    pos_n[1] = r.s.j;
    pos_n[2] = r.s.k;
    
    /* First Steps */
    stateToRk4(r, t);
    
    for (i = 0; i < DOF; i++)
    {
        secondOrderDE[0][i] = secondOrder[i];
        firstOrderDE[0][i] = firstOrder[i];
    }
    
    
    /* Second Steps */
    r = updateState(r, 0, 0.5*h);           //Midpoint
    stateToRk4(r, t + 0.5*h);               //Midpoint

    for (i = 0; i < DOF; i++)
    {
        secondOrderDE[1][i] = secondOrder[i];
        firstOrderDE[1][i] = firstOrder[i];
    }


    /* Third Steps */
    r = updateState(r, 1, 0.5*h);           //Midpoint
    stateToRk4(r, t + 0.5*h);               //Midpoint
    
    for (i = 0; i < DOF; i++)
    {
        secondOrderDE[2][i] = secondOrder[i];
        firstOrderDE[2][i] = firstOrder[i];
    }


    /* Fourth Steps */
    r = updateState(r, 2, h);               //Endpoint
    stateToRk4(r, t + h);                   //Endpoint
    
    for (i = 0; i < DOF; i++)
    {
        secondOrderDE[3][i] = secondOrder[i];
        firstOrderDE[3][i] = firstOrder[i];
    }
    
    /* Add it up */
    for (i = 0; i < DOF; i++)
    {
        average = firstOrderDE[0][i] 
                + 2*firstOrderDE[1][i] 
                + 2*firstOrderDE[2][i]
                + firstOrderDE[3][i];
                
        vel_n1[i] = vel_n[i] + (h*average)/6.0;
        
        average = secondOrderDE[0][i] 
                + 2*secondOrderDE[1][i] 
                + 2*secondOrderDE[2][i]
                + secondOrderDE[3][i];
        pos_n1[i] = pos_n[i] + (h*average)/6.0;
    }
    
    /* Update State */
    
    // Define Degrees of Freedom
    r.U.i = vel_n1[0];
    r.U.j = vel_n1[1];
    r.U.k = vel_n1[2];
    r.s.i = pos_n1[0];
    r.s.j = pos_n1[1];
    r.s.k = pos_n1[2];
    r.a = DoPhysics(r, t + h);
    
    r.m.fuel = updateFuelMass(r, t);
    
    if (hasNewMode() == 1)
    {
        r.mode = getNewMode();
    }

    return r;
}

state updateState(state r, int previousStep, double point)
{
    double vel[DOF], pos[DOF];
    int i;
    
    // Define Degrees of Freedom
    vel[0] = r.U.i;
    vel[1] = r.U.j;
    vel[2] = r.U.k;
    pos[0] = r.s.i;
    pos[1] = r.s.j;
    pos[2] = r.s.k;
    
    for (i = 0; i < DOF; i++)
    {
        vel[i] = vel_n[i] + point*firstOrderDE[previousStep][i];   //Eulers
        pos[i] = pos_n[i] + point*secondOrderDE[previousStep][i];  //Eulers
    }
    
    r.U.i = vel[0];
    r.U.j = vel[1];
    r.U.k = vel[2];
    r.s.i = pos[0];
    r.s.j = pos[1];
    r.s.k = pos[2];
    
    return r;
}

void stateToRk4(state r, double t)
{
    vec accel = DoPhysics(r, t);

    // Define Degrees of Freedom
    secondOrder[0] = r.U.i;
    secondOrder[1] = r.U.j;
    secondOrder[2] = r.U.k;
    firstOrder[0] = accel.i;
    firstOrder[1] = accel.j;
    firstOrder[2] = accel.k;
}

