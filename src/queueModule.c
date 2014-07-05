#include <stdio.h>
#include "elev.h"
#include "stateMachine.h"
#include "queueModule.h"

Queue_t queue;

void addOrder( elev_button_type_t button_type, int floor ) {
    switch( button_type )
    {
        case BUTTON_COMMAND:
            if ( queue.noDir[ floor ] == 0 ) {
                queue.noDir[ floor ] = 1;
                elev_set_button_lamp( button_type, floor, 1 );
            }
            break;
        case BUTTON_CALL_UP:
            if ( queue.upDir[ floor ] == 0 ){
                queue.upDir[ floor ] = 1;
                elev_set_button_lamp( button_type, floor, 1 );
            }
            break;
        case BUTTON_CALL_DOWN:
            if ( queue.downDir[ floor ] == 0 ) {
                queue.downDir[ floor ] = 1;
                elev_set_button_lamp( button_type, floor, 1 );
            }
            break;
        default:
            break;
    }
}

void clearOrders() {
    int i;
    for ( i = 0;  i < N_FLOORS; ++i )
       queue.downDir[ i ] = 0;
    for ( i=0; i < N_FLOORS; ++i )
       queue.upDir[ i ] = 0;
    for ( i=0; i< N_FLOORS; ++i )
       queue.noDir[ i ] = 0;
}

void delOrder( int floor, direction_t rideDir ) {
    queue.noDir[ floor ] = 0;
    queue.upDir[ floor ] = 0;
    queue.downDir[ floor ] = 0;
    if ( floor != (N_FLOORS - 1) )
        elev_set_button_lamp( BUTTON_CALL_UP, floor, 0 );
    if ( floor != 0 )
        elev_set_button_lamp( BUTTON_CALL_DOWN, floor, 0 );
    elev_set_button_lamp( BUTTON_COMMAND, floor, 0 );
}

int directionToGo( int floor, int nextOrder ) {
    if ( (floor - nextOrder) < 0 )
        return UP;
    if ( (floor - nextOrder) > 0 )
        return DOWN;
    if ( (floor - nextOrder) == 0 )
        return 0;
    return 0;
}

int emptyQueue() {
    int i;
    for ( i = 0;  i < N_FLOORS; ++i ) {
       if ( queue.downDir[ i ] == 1)
           return 0;
    }
    for ( i = 0; i < N_FLOORS; ++i ) {
       if ( queue.upDir[ i ] == 1 )
           return 0;
    }
    for ( i = 0; i < N_FLOORS; ++i ) {
       if ( queue.noDir[ i ] == 1 )
           return 0;
    }
    return 1;
}

int getNextOrder( int floor ) {
    int i;
    if ( lastDirection == UP ) {
        for ( i = floor; i < N_FLOORS; ++i ) {
            if( queue.noDir[ i ] || queue.upDir[ i ] )
                return i;
        }
        for ( i = N_FLOORS-1; i > floor; --i ) {
            if( queue.downDir[ i ] )
                return i;
        }
        for ( i = floor; i >= 0; --i ) {
            if( queue.downDir[ i ] || queue.upDir[ i ] || queue.noDir[ i ] )
                return i;
        }
        return 0;
    }
     else {
        for ( i = floor; i >= 0; --i ) {
            if( queue.noDir[ i ] || queue.downDir[ i ] ) {
                return i;
            }
        }
        for ( i = 0; i <= N_FLOORS; ++i ) {
            if( queue.upDir[ i ] )
                return i;
        }
        for ( i = floor; i <= N_FLOORS; ++i ) {
            if ( queue.downDir[ i ] || queue.upDir[ i ] || queue.noDir[ i ] )
                return i;
        }
        return 0;
     }
     return 0;
}

int order() {
    int i;
    int isOrder = 0;
    for ( i = 0;  i < N_FLOORS; ++i ) {
        if ( elev_get_button_signal( BUTTON_COMMAND, i ) ) {
            addOrder( BUTTON_COMMAND, i );
            isOrder = 1;
        }
    }
    if ( !isStopped() && !isStoppedObstruction() ) {
        for ( i=0; i < (N_FLOORS-1); ++i ) {
            if ( elev_get_button_signal( BUTTON_CALL_UP, i ) && !isStopped() ) {
                addOrder( BUTTON_CALL_UP, i );
                isOrder = 1;
            }
        }
        for ( i = 1; i < (N_FLOORS); ++i ) {
            if ( elev_get_button_signal( BUTTON_CALL_DOWN, i ) && !isStopped() ) {
                addOrder( BUTTON_CALL_DOWN, i );
                isOrder = 1;
            }
        }
    }
   return isOrder;
}
