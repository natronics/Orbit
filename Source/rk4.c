#include "structs.h"
#include "physics.h"
#include "rk4.h"

#define ERR 1.0e-6

state rk4(state r, float h, double t)
{
    double k[4], l[4], p[4], q[4], u[4], v[4], U_n[3], s_n[3];
    
    /* Fisrt Steps */
    U_n[x] = r.U[x];                    
    s_n[x] = r.s[x];
    U_n[y] = r.U[y];                    
    s_n[y] = r.s[y];
    U_n[z] = r.U[z];                    
    s_n[z] = r.s[z];
    
    //X
    k[0] = U_n[x];                      
    l[0] = physics(r, t).i;             
    
    //Y
    p[0] = U_n[y];                      
    q[0] = physics(r, t).j;         
    
    //Z
    u[0] = U_n[z];                      
    v[0] = physics(r, t).k;            
    
    /* Second Steps */
    r.U[x] = U_n[x] + 0.5*h*l[0];       
    r.s[x] = s_n[x] + 0.5*h*k[0];
    r.U[y] = U_n[y] + 0.5*h*q[0];       
    r.s[y] = s_n[y] + 0.5*h*p[0];
    r.U[z] = U_n[z] + 0.5*h*v[0];       
    r.s[z] = s_n[z] + 0.5*h*u[0];
    
    //X
    k[1] = r.U[x];
    l[1] = physics(r, t + 0.5*h).i;             
   
    //Y
    p[1] = r.U[y];
    q[1] = physics(r, t + 0.5*h).j;      
    
    //Z
    u[1] = r.U[z];
    v[1] = physics(r, t + 0.5*h).k;          
    
    /* Thid Steps */
    r.U[x] = U_n[x] + 0.5*h*l[1];
    r.s[x] = s_n[x] + 0.5*h*k[1];
    r.U[y] = U_n[y] + 0.5*h*q[1];
    r.s[y] = s_n[y] + 0.5*h*p[1];
    r.U[z] = U_n[z] + 0.5*h*v[1];
    r.s[z] = s_n[z] + 0.5*h*u[1];
    
    //X
    k[2] = r.U[x];
    l[2] = physics(r, t + 0.5*h).i;             
    
    //Y
    p[2] = r.U[y];
    q[2] = physics(r, t + 0.5*h).j;         
    
    //Z
    u[2] = r.U[z];
    v[2] = physics(r, t + 0.5*h).k; 
    
    /* Fourth Steps */
    r.U[x] = U_n[x] + h*l[2];
    r.s[x] = s_n[x] + h*k[2];
    r.U[y] = U_n[y] + h*q[2];
    r.s[y] = s_n[y] + h*p[2];
    r.U[z] = U_n[z] + h*v[2];
    r.s[z] = s_n[z] + h*u[2];
    
    //X
    k[3] = r.U[x];
    l[3] = physics(r, t + h).i;     
    
    //Y
    p[3] = r.U[y];
    q[3] = physics(r, t + h).j;      
    
    //Z
    u[3] = r.U[z];
    v[3] = physics(r, t + h).k;   
    
    /* Add it up */
    r.U[x] = U_n[x] + 1/6.0*h*(l[0] + 2*l[1] + 2*l[2] + l[3]);
    r.s[x] = s_n[x] + 1/6.0*h*(k[0] + 2*k[1] + 2*k[2] + k[3]);
    
    r.U[y] = U_n[y] + 1/6.0*h*(q[0] + 2*q[1] + 2*q[2] + q[3]);
    r.s[y] = s_n[y] + 1/6.0*h*(p[0] + 2*p[1] + 2*p[2] + p[3]);
    
    r.U[z] = U_n[z] + 1/6.0*h*(v[0] + 2*v[1] + 2*v[2] + v[3]);
    r.s[z] = s_n[z] + 1/6.0*h*(u[0] + 2*u[1] + 2*u[2] + u[3]);
    
    r.a[x] = physics(r, t + h).i;
    r.a[y] = physics(r, t + h).j;
    r.a[z] = physics(r, t + h).k;
    
    return r;
}
