#define G -6.6730e-11
#define Re 6378137.0 
#define Me 5.9742e24
#define g_0 9.80665

vec DoPhysics(state r, double t);
vec Force_Drag(state r, double t);
vec Force_Thrust(state r, double t);
double TotalMass(mass m);
double updateFuelMass(state r, double t);
double KE(state r);
double PE(state r);
