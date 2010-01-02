#define G -6.6730e-11
#define Re 6378137.0 
#define Me 5.9742e24
#define g_0 9.80665

vec physics(state r, double t);
vec gravity(state r);
vec drag(state r, double t);
double rho(double h);
double zTemperature(double h);
vec thrust(state r, double t);

double velocity(state r);
double position(state r);
double acceleration(state r);
double KE(state r);
double PE(state r);
double updateFuelMass(state r, double t);
double totalMass(mass m);
