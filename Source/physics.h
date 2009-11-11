#define G -6.6730e-11
#define Re 6378137.0 
#define Me 5.9742e24

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
double downrange(state r, double lat1, double lon1);
double updateMass(state r, double t);
