double altitude(state r);
double latitude(state r);
double longitude(state r);
matrix3 enu(state r);
vec matrixMath(vec v, matrix3 m);
vec ecefFromEnu(vec _enu, state r);
vec cartesian(double rho, double theta, double phi);
double radians(double degrees);
double degrees(double radians);

