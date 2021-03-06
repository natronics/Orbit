#define G -6.6730e-11
#define Re 6378137.0 
#define Me 5.9742e24
#define g_0 9.80665

vec LinearAcceleration(state r, double t);
vec AngularAcceleration(state r, double t);
vec Force_Drag(state r, double t);
vec Force_Thrust(state r, double t);
double KE(state r, double met);
double PE(state r, double met);
double RocketMass(state r, double met);
double MDot(state r, double met);
