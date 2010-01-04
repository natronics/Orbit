#include <stdio.h>

#include "structs.h"
#include "physics.h"
#include "rk4.h"

#define ERR 1.0e-6

state rk4(state r, float h, double t)
{
    double k[4], l[4], p[4], q[4], u[4], v[4], U_n[3], s_n[3];
    vec phys;
    
    /* Fisrt Steps */
    U_n[x] = r.U[x];                    
    s_n[x] = r.s[x];
    U_n[y] = r.U[y];                    
    s_n[y] = r.s[y];
    U_n[z] = r.U[z];                    
    s_n[z] = r.s[z];
    
    phys = DoPhysics(r, t);
    
    //X
    k[0] = U_n[x];                      
    l[0] = phys.i;             
    
    //Y
    p[0] = U_n[y];                      
    q[0] = phys.j;         
    
    //Z
    u[0] = U_n[z];                      
    v[0] = phys.k;            
    
    /* Second Steps */
    r.U[x] = U_n[x] + 0.5*h*l[0];       
    r.s[x] = s_n[x] + 0.5*h*k[0];
    r.U[y] = U_n[y] + 0.5*h*q[0];       
    r.s[y] = s_n[y] + 0.5*h*p[0];
    r.U[z] = U_n[z] + 0.5*h*v[0];       
    r.s[z] = s_n[z] + 0.5*h*u[0];
    
    phys = DoPhysics(r, t + 0.5*h);
    
    //X
    k[1] = r.U[x];
    l[1] = phys.i;             
   
    //Y
    p[1] = r.U[y];
    q[1] = phys.j;      
    
    //Z
    u[1] = r.U[z];
    v[1] = phys.k;          
    
    /* Thid Steps */
    r.U[x] = U_n[x] + 0.5*h*l[1];
    r.s[x] = s_n[x] + 0.5*h*k[1];
    r.U[y] = U_n[y] + 0.5*h*q[1];
    r.s[y] = s_n[y] + 0.5*h*p[1];
    r.U[z] = U_n[z] + 0.5*h*v[1];
    r.s[z] = s_n[z] + 0.5*h*u[1];
    
    phys = DoPhysics(r, t + 0.5*h);
    
    //X
    k[2] = r.U[x];
    l[2] = phys.i;             
    
    //Y
    p[2] = r.U[y];
    q[2] = phys.j;         
    
    //Z
    u[2] = r.U[z];
    v[2] = phys.k; 
    
    /* Fourth Steps */
    r.U[x] = U_n[x] + h*l[2];
    r.s[x] = s_n[x] + h*k[2];
    r.U[y] = U_n[y] + h*q[2];
    r.s[y] = s_n[y] + h*p[2];
    r.U[z] = U_n[z] + h*v[2];
    r.s[z] = s_n[z] + h*u[2];
    
    phys = DoPhysics(r, t + h);
    
    //X
    k[3] = r.U[x];
    l[3] = phys.i;     
    
    //Y
    p[3] = r.U[y];
    q[3] = phys.j;      
    
    //Z
    u[3] = r.U[z];
    v[3] = phys.k;   
    
    /* Add it up */
    r.U[x] = U_n[x] + 1/6.0*h*(l[0] + 2*l[1] + 2*l[2] + l[3]);
    r.s[x] = s_n[x] + 1/6.0*h*(k[0] + 2*k[1] + 2*k[2] + k[3]);
    
    r.U[y] = U_n[y] + 1/6.0*h*(q[0] + 2*q[1] + 2*q[2] + q[3]);
    r.s[y] = s_n[y] + 1/6.0*h*(p[0] + 2*p[1] + 2*p[2] + p[3]);
    
    r.U[z] = U_n[z] + 1/6.0*h*(v[0] + 2*v[1] + 2*v[2] + v[3]);
    r.s[z] = s_n[z] + 1/6.0*h*(u[0] + 2*u[1] + 2*u[2] + u[3]);
    
    phys = DoPhysics(r, t + h);
    
    r.a[x] = phys.i;
    r.a[y] = phys.j;
    r.a[z] = phys.k;
    
    r.m.fuel = updateFuelMass(r, t);

    if (hasNewMode() == 1)
    {
        //printf("new mode! %d:", r.mode);
        r.mode = getNewMode();
        //printf("%d\n", r.mode);
    }

    return r;
}
