#define x 0
#define y 1
#define z 2

typedef struct {double i; double j; double k;} vec;
typedef struct {double s[3]; double U[3]; double a[3]; vec o; vec w; double m; int mode;} state;
typedef struct {double m[3][3];} matrix3;
