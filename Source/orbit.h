#define VERSION 0.12

state InitialRocket();
double I_sp();
double mdot();
void setNewMode(unsigned int mode);
unsigned int getNewMode();
unsigned int hasNewMode();
double BeginTime();
