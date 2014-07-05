#ifndef STATEMACHINE_H
#define STATEMACHINE_H
#include <time.h>

typedef enum { DOWN=-1, UP=1 } direction_t;
typedef enum { TIMEROUT, EMPTYQUEUE, NEWORDEREMPTYQUEUE, NEWORDERCURRENTFLOOR, OBSTRON, OBSTROFF, STOP, FLOORREACHED } signal_t;
typedef enum { IDLE, MOVING, MOVEOBSTRUCTION, DOOROPEN, DOOROBSTRUCTION, STOPPED, STOPPEDOBSTRUCTION } state_t;

direction_t direction;
direction_t lastDirection;

void initialize();
int isStopped();
int isStoppedObstruction();
void stateMachine( signal_t signal );
int timerIsActive();
void wait( double sec );

#endif
