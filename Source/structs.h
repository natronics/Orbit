#define INIT 0
#define BURNING 1
#define COASING 2
#define SEPARATED 3

typedef struct {double i; double j; double k;} vec;
typedef struct {double fuelMass; double isp; double thrust;} motor;
typedef struct {vec s; vec U; vec a; motor m; double met;} state;
typedef struct {unsigned int stage; double emptyMass; double stageDelay;} rocketDesc;
typedef struct {rocketDesc description; state currentState; 
                    state initialState;
                    state burnoutState;
                    state separationState;
                    state apogeeState;
                    state splashdownState;
                    unsigned int mode;} Rocket_Stage;
typedef struct {double m[3][3];} matrix3;
