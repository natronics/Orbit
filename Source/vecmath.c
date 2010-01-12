#include <math.h>
#include "structs.h"
#include "vecmath.h"

vec UnitVec(vec v)
{
    double magnitude;
    vec unitVector;
    
    magnitude = Norm(v);
    
    if (magnitude == 0)
    {
        unitVector.i = 0.0;
        unitVector.j = 0.0;
        unitVector.k = 0.0;
        return unitVector;
    }
    unitVector.i = v.i / magnitude;
    unitVector.j = v.j / magnitude;
    unitVector.k = v.k / magnitude;
    
    return unitVector;
}

double Square(double val)
{
    return val*val;
}

double Norm(vec v)
{
    return sqrt(Square(v.i) + Square(v.j) + Square(v.k));
}

double DotProd(vec a, vec b)
{
    double X, Y, Z;
    
    X = a.i * b.i;
    Y = a.j * b.j;
    Z = a.k * b.k;
    
    return X + Y + Z;
}

double AngleVec(vec a, vec b)
{
    double A, B;
    double AdotB = DotProd(a, b);
    double angle;
    
    angle = acos( (AdotB) / (A*B) );
    
    return angle;
}

vec ZeroVec()
{
    vec v;
    
    v.i = 0.0;
    v.j = 0.0;
    v.k = 0.0;
    
    return v;
}
