#include <math.h>
#include "structs.h"
#include "vecmath.h"

vec unitVec(double *vector)
{
    double magnitude;
    vec unitVector;
    
    magnitude = quad(vector[x], vector[y], vector[z]);
    
    if (magnitude == 0)
    {
        unitVector.i = 0.0;
        unitVector.j = 0.0;
        unitVector.k = 0.0;
        return unitVector;
    }
    unitVector.i = vector[x] / magnitude;
    unitVector.j = vector[y] / magnitude;
    unitVector.k = vector[z] / magnitude;
    
    return unitVector;
}

double square(double val)
{
    return val*val;
}

double quad(double X, double Y, double Z)
{
    return sqrt(square(X) + square(Y) + square(Z));
}
