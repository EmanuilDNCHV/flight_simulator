#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// Константи
#define MAX_ALTITUDE     10000
#define MIN_ALTITUDE     0
#define MAX_SPEED        1000
#define MIN_SPEED        0
#define SPEED_INCREMENT  50

#define TARGET_LANDING_SPEED_MIN 150
#define TARGET_LANDING_SPEED_MAX 200

// Структура
typedef struct {
    double altitude;    
    double speed;       
    double pitch;   
    double fuel;        
    double heading;     
    bool engine_on;
    int time;          
} Aircraft;

#endif