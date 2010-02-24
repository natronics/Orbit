#include <stdio.h>
#include "structs.h"
#include "physics.h"
#include "vecmath.h"
#include "orbit.h"
#include "rk4.h"

#define ERR 1.0e-6
#define DOF 3

static state updateState(state r, int previousStep, double point);
static void initilize(state r);
static void evalSecondDeriv(state r, double t);
static void evalFirstDeriv(state r, double t);
static state setFirstDeriv(state r, double *firstDerivative);
static state setFunction(state r, double *function);

double function_n[DOF];
double firstDeriv_n[DOF];
double function_n1[DOF];
double firstDeriv_n1[DOF];

double firstDeriv[DOF];
double secondDeriv[DOF];

double rk4firstDeriv[4][DOF];
double rk4secondDeriv[4][DOF];

/*!
 * A generic fourth-order Runge-Kutta numerical integration engine for second 
 * order differential equations.
 * 
 * Second Derivative (eg, acceleration) = secondDeriv
 * First Derivative (eg, velocity)      = firstDeriv
 * Function (eg, position               = function
 *
 * initial values   = *_n
 * next guess       = *_n1
 *
 * One Eulers step:
 * secondDeriv = ::get from program::
 * firstDeriv_n1 = firstDeriv_n + secondDeriv*timestep
 * function_n1 = function_n + firstDeriv*timestep
 *
 * rk4firstDeriv = Guesses for the first Derivitive
 * rk4secondDeriv = Guesses for the second Derivitve
 *
 * You are not expected to understand this.
 */
state rk4(state r, float h)
{
    int i;
    double average;
    double t = r.met;
    
    // Prime the pump
    initilize(r);
     
    /* First Steps */
    evalSecondDeriv(r, t);                  //Begining
    
    for (i = 0; i < DOF; i++)
    {
        rk4firstDeriv[0][i] = firstDeriv_n[i];      //Using initial values
        rk4secondDeriv[0][i] = secondDeriv[i];
    }    
    
    /* Second Steps */
    r = updateState(r, 0, 0.5*h);           //Midpoint
    evalFirstDeriv(r, t + 0.5*h);           //Midpoint
    evalSecondDeriv(r, t + 0.5*h);          //Midpoint

    for (i = 0; i < DOF; i++)
    {
        rk4firstDeriv[1][i] = firstDeriv[i];
        rk4secondDeriv[1][i] = secondDeriv[i];
    }


    /* Third Steps */
    r = updateState(r, 1, 0.5*h);           //Midpoint
    evalFirstDeriv(r, t + 0.5*h);           //Midpoint
    evalSecondDeriv(r, t + 0.5*h);          //Midpoint

    for (i = 0; i < DOF; i++)
    {
        rk4firstDeriv[2][i] = firstDeriv[i];
        rk4secondDeriv[2][i] = secondDeriv[i];
    }


    /* Fourth Steps */
    r = updateState(r, 2, h);               //Endpoint
    evalFirstDeriv(r, t + h);               //Endpoint
    evalSecondDeriv(r, t + h);              //Endpoint
    
    for (i = 0; i < DOF; i++)
    {
        rk4firstDeriv[3][i] = firstDeriv[i];
        rk4secondDeriv[3][i] = secondDeriv[i];
    }
    
    
    /* Add it up */
    for (i = 0; i < DOF; i++)
    {
        average = rk4firstDeriv[0][i] 
                + 2*rk4firstDeriv[1][i] 
                + 2*rk4firstDeriv[2][i]
                + rk4firstDeriv[3][i];
                
        function_n1[i] = function_n[i] + (h*average)/6.0;
        
        
        average = rk4secondDeriv[0][i] 
                + 2*rk4secondDeriv[1][i] 
                + 2*rk4secondDeriv[2][i]
                + rk4secondDeriv[3][i];
                
        firstDeriv_n1[i] = firstDeriv_n[i] + (h*average)/6.0;
    }
    
    /* Update State */
    r = setFirstDeriv(r, firstDeriv_n1);
    r = setFunction(r, function_n1);
    r.a = LinearAcceleration(r, t + h);

    return r;
}

/*******************************************************************************
 * BEGIN Setting up Degree of Freedom mapping
 ******************************************************************************/

static void initilize(state r)
{
    function_n[0] = r.s.i;
    function_n[1] = r.s.j;
    function_n[2] = r.s.k;
    
    firstDeriv_n[0] = r.U.i;
    firstDeriv_n[1] = r.U.j;
    firstDeriv_n[2] = r.U.k;
}

static state setFirstDeriv(state r, double *firstDerivative)
{
    r.U.i = firstDerivative[0];
    r.U.j = firstDerivative[1];
    r.U.k = firstDerivative[2];
    
    return r;
}

static state setFunction(state r, double *function)
{
    r.s.i = function[0];
    r.s.j = function[1];
    r.s.k = function[2];
    
    return r;
}

static void evalSecondDeriv(state r, double t)
{
    vec accel = LinearAcceleration(r, t);
    secondDeriv[0] = accel.i;
    secondDeriv[1] = accel.j;
    secondDeriv[2] = accel.k;
}

static void evalFirstDeriv(state r, double t)
{
    firstDeriv[0] = r.U.i;
    firstDeriv[1] = r.U.j;
    firstDeriv[2] = r.U.k;
}

/*******************************************************************************
 * END Setting up Degree of Freedom mapping
 ******************************************************************************/

static state updateState(state r, int previousStep, double point)
{
    double firstDerivative[DOF];
    double function[DOF];
    int i;
    
    for (i = 0; i < DOF; i++)
    {
        firstDerivative[i] = firstDeriv_n[i] + point*rk4secondDeriv[previousStep][i];
        function[i] = function_n[i] + point*rk4firstDeriv[previousStep][i];
    }
    
    r = setFirstDeriv(r, firstDerivative);
    r = setFunction(r, function);
    
    return r;
}

