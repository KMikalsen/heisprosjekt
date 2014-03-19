#include <stdio.h>
#include <time.h>
#include "elev.h"
#include "stateMachine.h"
#include "queueModule.h"

int timerActive = 0;
int queueIsEmpty; 
int obstructionSignal;
int stoppedSignal;
int lastObstructionSignal = 0;
int lastStoppedSignal = 0;
clock_t duration = 0; 
clock_t start;  
clock_t seconds;
 
void wait( double sec ) {
    timerActive = 1;
    start = clock();  
    seconds = sec*CLOCKS_PER_SEC;
}

int main()
{ 
    // Initialize hardware
    if (!elev_init()) {
        printf(__FILE__ ": Unable to initialize elevator hardware\n");
        return 1;
    } 
    printf("Best Elevator Software,  version: 2.27.14\n" ); 
    initialize();
    lastFloorReached = N_FLOORS+1;
    while (1) { 
        queueIsEmpty = emptyQueue();
        obstructionSignal = elev_get_obstruction_signal();
        stoppedSignal = elev_get_stop_signal();
        thisFloor = elev_get_floor_sensor_signal();
        duration = clock()-start;
        
        if ( timerActive && duration > seconds ) {
            timerActive = 0;
            duration = 0;        
            stateMachine( TIMEROUT );
        }

        if ( obstructionSignal != lastObstructionSignal ) {
            if ( obstructionSignal == 1 )
                stateMachine( OBSTRON );
            if ( obstructionSignal == 0 )
                stateMachine( OBSTROFF );
            lastObstructionSignal = obstructionSignal;
        }

        if ( stoppedSignal != lastStoppedSignal ) {
            if ( stoppedSignal == 1 )
                stateMachine( STOP );
            lastStoppedSignal = stoppedSignal;
        }

        if ( thisFloor != -1 && ( lastFloorReached != thisFloor ) ) {
            if ( thisFloor-lastFloorReached < 0 )
                direction = DOWN;
            if ( thisFloor-lastFloorReached > 0 ) 
                direction = UP;
            lastFloorReached = thisFloor;
            elev_set_floor_indicator( thisFloor );
        }

        if ( order() && directionToGo( thisFloor, getNextOrder( thisFloor ) ) == 0 )
            stateMachine( NEWORDERCURRENTFLOOR );

        if ( thisFloor != -1 && directionToGo( thisFloor , getNextOrder( thisFloor ) ) == 0 && !timerActive )
            stateMachine( FLOORREACHED );

        if( queueIsEmpty == 1 && order() ) 
            stateMachine( NEWORDEREMPTYQUEUE );
    }
    return 0;
}
