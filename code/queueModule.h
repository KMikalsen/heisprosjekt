#ifndef QUEUEMODULE_H
#define QUEUEMODULE_H

int thisFloor;
int lastFloorReached;

void clearOrders();
void delOrder( int floor, direction_t rideDir );
int directionToGo(int floor, int nextOrder);
int emptyQueue();
int getNextOrder( int floor );
int order();

typedef struct {
    int upDir[N_FLOORS];
    int downDir[N_FLOORS];
    int noDir[N_FLOORS];
} Queue_t;

#endif
