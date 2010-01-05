#define x 0
#define y 1
#define z 2
#define INIT 0
#define BURNING 1
#define COASING 2

typedef struct {double i; double j; double k;} vec;
typedef struct {double fuel; double structure;} mass;
typedef struct {vec s; vec U; vec a; mass m; unsigned int mode;} state;
typedef struct {double m[3][3];} matrix3;
