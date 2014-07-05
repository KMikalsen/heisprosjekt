#include <stdio.h>
#include <unistd.h>
#include "elev.h"
#include "stateMachine.h"
#include "queueModule.h"

const int MAXSPEED = 300;
const int NOSPEED = 0;

state_t currentState;
void f_stopped( signal_t signal );

int isStopped() {
    if ( currentState == STOPPED )
        return 1;
    return 0;
}

int isStoppedObstruction() {
    if ( currentState == STOPPEDOBSTRUCTION )
        return 1;
    return 0;
}

void f_rideCar() {
    lastDirection = directionToGo( thisFloor, getNextOrder( thisFloor ) );
    elev_set_speed( directionToGo( thisFloor, getNextOrder( thisFloor ) )*MAXSPEED );
}

void f_stopMotor( direction_t rideDir ) {
    if ( rideDir == -1 ) {
        elev_set_speed( MAXSPEED );
//        usleep(100000);
        elev_set_speed( NOSPEED );
    } else {
        elev_set_speed( -MAXSPEED );
//        usleep(100000);
        elev_set_speed( NOSPEED );
    }
}

void f_clearLights() {
    int i = 0;
    for ( i = 0; i < ( N_FLOORS-1 ); ++i ) {
         elev_set_button_lamp( 0, i, 0 );
    }
    for ( i = 1; i < N_FLOORS; ++i ) {
         elev_set_button_lamp( 1, i, 0 );
    }
    for ( i = 0; i < N_FLOORS; ++i ) {
         elev_set_button_lamp( 2, i, 0 );
    }
}

void f_doorObstruction( signal_t signal ) {
    switch ( signal ) {
    case OBSTROFF:
        currentState = DOOROPEN;
        wait( 3.0 );
        break;
    default:
        break;
    }
}

void f_doorOpen( signal_t signal ) {
    if ( elev_get_floor_sensor_signal() == -1 ){
        initialize();
    }
    delOrder( thisFloor, direction );
    elev_set_door_open_lamp( 1 );
    if ( signal != TIMEROUT )
         wait( 3.0 );
    switch( signal ) {
        case TIMEROUT:
              elev_set_door_open_lamp( 0 );
              if( emptyQueue() == 1 )
                currentState = IDLE;
              else {
                currentState = MOVING;
                f_rideCar();
              }
              break;
        case OBSTRON:
              currentState = DOOROBSTRUCTION;
              break;
        case NEWORDERCURRENTFLOOR:
              currentState = DOOROPEN;
              break;
        case STOP:
            elev_set_door_open_lamp( 1 );
            currentState = STOPPED;
            f_stopped( signal );
            break;
        default:
            break;
    }
}

void f_idle( signal_t signal ) {
    switch( signal ) {
        case NEWORDEREMPTYQUEUE:
            f_rideCar();
            currentState = MOVING;
            break;
        case NEWORDERCURRENTFLOOR:
            currentState = DOOROPEN;
            break;
        case STOP:
            elev_set_door_open_lamp( 1 );
            currentState = STOPPED;
            f_stopped( signal );
            break;
        default:
            break;
    }
}

void initialize() {
    int floor = elev_get_floor_sensor_signal();
    elev_set_speed( -MAXSPEED );
    while ( floor == -1 )
        floor = elev_get_floor_sensor_signal();
    thisFloor = floor;
    f_stopMotor( DOWN );
    f_clearLights();
    clearOrders();
    direction = DOWN;
    currentState = IDLE;
}

void f_moveObstruction( signal_t signal ) {
    switch( signal ) {
         case OBSTROFF:
             elev_set_speed( lastDirection * MAXSPEED );
             currentState = MOVING;
             break;
        case STOP:
             elev_set_stop_lamp( 1 );
             clearOrders();
             f_clearLights();
             currentState = STOPPEDOBSTRUCTION;
             break;
        default:
             break;
    }
}

void f_moving( signal_t signal ) {
    switch( signal ) {
        case TIMEROUT:
            f_rideCar();
            break;
        case STOP:
            currentState = STOPPED;
            f_stopped( signal );
            break;
        case FLOORREACHED:
            if ( directionToGo( thisFloor, getNextOrder( thisFloor ) ) == 0 ) {
                 f_stopMotor( direction );
                 currentState = DOOROPEN;
            }
             break;
        case OBSTRON:
             currentState = MOVEOBSTRUCTION;
             f_stopMotor( direction );
             break;
        default:
             break;
    }
}

void f_stopped( signal_t signal ) {
    switch( signal ) {
        case OBSTRON:
            currentState = STOPPEDOBSTRUCTION;
            break;
        case NEWORDEREMPTYQUEUE:
            if ( directionToGo( lastFloorReached, getNextOrder( lastFloorReached) ) == 0 )
                elev_set_speed( lastDirection*-MAXSPEED );
            else
                elev_set_speed( directionToGo( lastFloorReached, getNextOrder( lastFloorReached) )*MAXSPEED );
            elev_set_door_open_lamp( 0 );
            elev_set_stop_lamp( 0 );
            currentState = MOVING;
            break;
        default:
            break;
    }
    if ( currentState == STOPPED ) {
    f_stopMotor( direction );
    elev_set_stop_lamp( 1 );
    clearOrders();
    f_clearLights();
    }
}

void f_stoppedObstruction( signal_t signal ) {
    switch( signal ) {
        case OBSTROFF:
            currentState = STOPPED;
            f_stopped( signal );
            break;
        case NEWORDEREMPTYQUEUE:
             if ( directionToGo( lastFloorReached, getNextOrder( lastFloorReached) ) == 0 )
                lastDirection *= -1;
             else
                lastDirection = directionToGo( lastFloorReached, getNextOrder( lastFloorReached) );
            currentState = MOVEOBSTRUCTION;
            elev_set_stop_lamp( 0 );
            break;
        default:
            break;
    }
}

void stateMachine( signal_t signal ) {
    switch( currentState ) {
        case IDLE:
            f_idle( signal );
            break;

        case MOVING:
            f_moving( signal );
            break;

        case MOVEOBSTRUCTION:
            f_moveObstruction( signal );
            break;

        case DOOROPEN:
            if( elev_get_floor_sensor_signal() == -1 ) {
                initialize();
            }
            f_doorOpen( signal );
            break;

        case DOOROBSTRUCTION:
            f_doorObstruction( signal );
            break;

        case STOPPED:
            f_stopped( signal );
            break;

        case STOPPEDOBSTRUCTION:
            f_stoppedObstruction( signal );
            break;

        default:
            break;
    }
}
