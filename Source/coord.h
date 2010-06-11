#include <math.h>
#include <stdio.h>
#include <time.h>

double Velocity(state r);
double Position(state r);
double Acceleration(state r);
double Altitude(state r);
double latitude(state r);
double longitude(state r);
matrix3 enu(state r);
vec matrixMath(vec v, matrix3 m);
vec EnuToEcef(vec _enu, state r);
vec EulerToEcef(vec v, vec rot);
vec cartesian(double rho, double theta, double phi);
double Downrange(state r);
double radians(double degrees);
double degrees(double radians);
time_t JdToUnixTime(double JD);
double SecondsToDecDay(double seconds);
double DecDayToSeconds(double decDay);
void SecondsToHmsString(double seconds, char *buffer);
double Interpolat1D(vec2 *sample, double value, int dataLength);
double IntegrateVec2Array(vec2 *curve, int len);
double AverageThrust(motor m);
double AverageMdot(motor m);
double AverageIsp(motor m);
double Impulse(motor m);
