#define INIT 0
#define BURNING 1
#define COASING 2
#define SEPARATED 3

typedef struct {double i; double j; double k;} vec;
typedef struct {double i; double j;} vec2;
typedef struct {double m[3][3];} matrix3;

typedef struct {const char *name; 
                    double fuelMass; 
                    double isp; 
                    vec2* thrustCurve; 
                    int curveLength;} motor;
typedef struct {double cd; double area; double agl;} chute;
typedef struct {vec s; vec U; vec a; double fuelMass; double met;} state;
typedef struct {unsigned int stage;
                    double emptyMass;
                    double ignitionDelay;
                    double stageDelay;
                    motor *motors;
                    int numOfMotors;
                    chute *chutes;
                    int numOfChutes;} stageDesc;
typedef struct {stageDesc description;
                    state initialState;
                    state currentState;
                    state burnoutState;
                    state separationState;
                    state apogeeState;
                    state splashdownState;
                    unsigned int mode;} Rocket_Stage;

