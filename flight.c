#include "data.h"
#include "flight.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>  

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <unistd.h>
#endif

// Основни надписи

void welcome_message(void) {
    printf("=================================================\n");
    printf("                 FLIGHT SIMULATOR \n");
    printf("=================================================\n\n");
    
    printf("MISSION: Land safely at the airport!\n");
    printf("AIRPORT: Heading 90° (EAST)\n\n");
    
    printf("LANDING REQUIREMENTS:\n");
    printf("* Altitude: 0 meters\n");
    printf("* Speed: 150-200 km/h\n");
    printf("* Heading: As close to 90° as possible\n\n");
    
    printf("Starting altitude: 3000 m\n");
    printf("Starting speed: 400 km/h\n");
    printf("Starting heading: 150°(SOUTHEAST)\n\n");
    
    printf("Press Enter to start your flight...");
    getchar();
    printf("\n\nEngines starting...\n");
}

char get_input_char(void) {
    #ifdef _WIN32
        return _getch();
    #else
        char c;
        system("stty raw -echo");
        c = getchar();
        system("stty cooked echo");
        return c;
    #endif
}

void game_over_message(int reason, const Aircraft *plane) {
    printf("\n=================================================\n");
    
    switch (reason) {
        case 0:  // Успешно приземяване
            printf("        SUCCESSFUL LANDING!\n\n");
            printf("Congratulations, pilot!\n");
            printf("You landed the plane safely.\n\n");
            printf("Final stats:\n");
            printf("* Landing speed: %.0f km/h\n", plane->speed);
            printf("* Final heading: %.0f°\n", plane->heading);
            printf("* Fuel remaining: %.1f%%\n", plane->fuel);
            printf("* Flight time: %d seconds\n", plane->time);
            printf("\nExcellent work!\n");
            break;
            
        case 1:  // Твърде бързо
            printf("        CRASH LANDING!\n\n");
            printf("You crashed on landing!\n");
            printf("Reason: TOO FAST - %.0f km/h\n\n", plane->speed);
            printf("Landing speed should be 150-200 km/h\n");
            printf("Try descending more gradually next time.\n");
            break;
            
        case 2:  // Твърде бавно
            printf("        CRASH LANDING!\n\n");
            printf("You crashed on landing!\n");
            printf("Reason: TOO SLOW - %.0f km/h\n\n", plane->speed);
            printf("Landing speed should be 150-200 km/h\n");
            printf("The plane stalled and dropped.\n");
            break;
            
        case 3:  // Свършване на горивото
            printf("        OUT OF FUEL!\n\n");
            printf("The plane ran out of fuel!\n");
            printf("You crashed into the ocean.\n\n");
            printf("Always monitor your fuel gauge!\n");
            break;
    }
    
    printf("=================================================\n");
}

// Визуализация на хоризонта

void display_artificial_horizon(double pitch, double altitude) {
    printf("=================================================\n");
    printf("                     HORIZON\n");
    printf("=================================================\n");
    
    const int ROWS = 13;
    const int COLS = 25;
    char horizon[ROWS][COLS];
    
    // Инициализация
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            horizon[i][j] = ' ';
        }
    }
    
    // Очертания
    for (int i = 0; i < ROWS; i++) {
        horizon[i][0] = '|';
        horizon[i][COLS-1] = '|';
    }
    for (int j = 0; j < COLS; j++) {
        horizon[0][j] = '-';
        horizon[ROWS-1][j] = '-';
    }
    
    // Ъгли
    horizon[0][0] = '+';
    horizon[0][COLS-1] = '+';
    horizon[ROWS-1][0] = '+';
    horizon[ROWS-1][COLS-1] = '+';
    
    // Визуализация
    int plane_row;
    if (pitch >= 30) plane_row = 1;
    else if (pitch <= -30) plane_row = 11;
    else plane_row = 6 - (int)(pitch / 30.0 * 5.0);
    
    if (plane_row < 1) plane_row = 1;
    if (plane_row > 11) plane_row = 11;
    
    const int CENTER_COL = COLS / 2;
    horizon[plane_row][CENTER_COL] = 'O';
    
    // Небе и земя
    for (int i = 1; i < ROWS-1; i++) {
        for (int j = 1; j < COLS-1; j++) {
            if (i == plane_row && j == CENTER_COL) continue;
            
            if (i < plane_row) horizon[i][j] = '.';
            else if (i > plane_row) horizon[i][j] = '=';
            else horizon[i][j] = '-';
        }
    }
    
    // Показване
    printf("\n");
    for (int i = 0; i < ROWS; i++) {
        printf("     ");
        for (int j = 0; j < COLS; j++) printf("%c", horizon[i][j]);
        
        if (i == 1) printf("   30° UP");
        else if (i == 3) printf("   20° UP");
        else if (i == 5) printf("   10° UP");
        else if (i == 6) printf("   LEVEL");
        else if (i == 7) printf("   10° DOWN");
        else if (i == 9) printf("   20° DOWN");
        else if (i == 11) printf("   30° DOWN");
        else printf("           ");
        
        printf("\n");
    }
    
    // Индикатор
    printf("\n     ");
    for (int i = 0; i < COLS; i++) {
        if (i == CENTER_COL) printf("^");
        else printf(" ");
    }
    printf("   Aircraft position\n");
    
    // Наклон
    printf("\n     ");
    if (pitch > 0) printf("     AIRCRAFT POINTING UP");
    else if (pitch < 0) printf("     AIRCRAFT POINTING DOWN");
    else printf(" AIRCRAFT LEVEL - BALANCED");
    
    printf("\n=================================================\n");
    
    char status[20];
    if (pitch > 15) strcpy(status, "STEEP CLIMB");
    else if (pitch > 5) strcpy(status, "CLIMBING");
    else if (pitch > 2) strcpy(status, "GENTLE CLIMB");
    else if (pitch < -15) strcpy(status, "STEEP DESCENT");
    else if (pitch < -5) strcpy(status, "DESCENDING");
    else if (pitch < -2) strcpy(status, "GENTLE DESCENT");
    else strcpy(status, "LEVEL FLIGHT");
    
    printf("  Status:  %-20s\n", status);
    printf("  Pitch:   %6.1f°\n", pitch);
    printf("  Altitude: %6.0f meters\n", altitude);
    printf("=================================================\n");
}

void display_instruments(const Aircraft *plane) {
    printf("\n=================================================\n");
    printf("              INSTRUMENTS\n");
    printf("-------------------------------------------------\n");
    
    // Височина
    printf(" ALTITUDE: %6.0f m  ", plane->altitude);
    int alt_bar = (int)(plane->altitude / MAX_ALTITUDE * 20);
    printf("[");
    for (int i = 0; i < 20; i++) i < alt_bar ? printf("#") : printf(".");
    printf("]\n");
    
    // Скорост
    printf(" SPEED:    %6.0f km/h ", plane->speed);
    int speed_bar = (int)(plane->speed / MAX_SPEED * 20);
    if (speed_bar > 20) speed_bar = 20;
    if (speed_bar < 0) speed_bar = 0;
    printf("[");
    for (int i = 0; i < 20; i++) i < speed_bar ? printf("=") : printf(".");
    printf("]\n");
    
    // Посока
    printf(" HEADING:  %6.0f° ", plane->heading);
    if (plane->heading >= 337.5 || plane->heading < 22.5) printf("NORTH       ");
    else if (plane->heading < 67.5) printf("NORTHEAST   ");
    else if (plane->heading < 112.5) printf("EAST        ");
    else if (plane->heading < 157.5) printf("SOUTHEAST   ");
    else if (plane->heading < 202.5) printf("SOUTH       ");
    else if (plane->heading < 247.5) printf("SOUTHWEST   ");
    else if (plane->heading < 292.5) printf("WEST        ");
    else printf("NORTHWEST   ");
    printf("\n");
    
    // Гориво и двигател
    printf(" FUEL:     %6.1f%%  ", plane->fuel);
    printf("[");
    for (int i = 0; i < 10; i++) i < (int)plane->fuel/10 ? printf("@") : printf(".");
    printf("] ");
    plane->engine_on ? printf("ENGINE: ON\n") : printf("ENGINE: OFF\n");
    
    printf("=================================================\n");
}

// Навигация
void display_radar(const Aircraft *plane) {
    printf("\n=================================================\n");
    printf("                NAVIGATION\n");
    printf("-------------------------------------------------\n");
    
    double airport_heading = 90.0;
    double heading_diff = fabs(plane->heading - airport_heading);
    if (heading_diff > 180) heading_diff = 360 - heading_diff;
    
    printf(" Current heading: %6.0f°\n", plane->heading);
    printf(" Airport heading: %6.0f° (EAST)\n", airport_heading);
    printf("\n");
    
    printf(" Deviation: %5.0f° ", heading_diff);
    if (heading_diff < 5) printf("[ PERFECT ]\n");
    else if (heading_diff < 20) printf("[ GOOD ]\n");
    else if (heading_diff < 45) printf("[ FAIR ]\n");
    else printf("[ POOR ]\n");
    
    printf("\n");
    if (plane->heading < 80) printf(" Turn RIGHT -> 90°\n");
    else if (plane->heading > 100) printf(" Turn LEFT <- 90°\n");
    else printf(" On course for airport\n");
    
    printf("=================================================\n");
}

//Указание за игра
void display_control_help(void) {
    printf("\n=================================================\n");
    printf("                CONTROLS\n");
    printf("-------------------------------------------------\n");
    printf("    W        - Climb (Nose Up)\n");
    printf("    S        - Descend (Nose Down)\n");
    printf("    A        - Turn Left\n");
    printf("    D        - Turn Right\n");
    printf("    E        - Engine On/Off\n");
    printf("    +        - Increase Speed (+50 km/h)\n");
    printf("    -        - Decrease Speed (-50 km/h)\n");
    printf("    Q        - Quit Game\n");
    printf("=================================================\n");
}

// Предупреждения 
void display_warnings(const Aircraft *plane) {
    bool any_warnings = false;
    
    printf("\nWARNINGS: ");
    
    if (plane->fuel < 20) { printf("LOW FUEL "); any_warnings = true; }
    if (plane->speed > 800) { printf("TOO FAST "); any_warnings = true; }
    if (plane->speed < 180 && plane->altitude > 1000) { printf("LOW SPEED "); any_warnings = true; }
    if (!plane->engine_on && plane->altitude > 500) { printf("ENGINE OFF "); any_warnings = true; }
    if (plane->pitch > 20) { printf("STEEP CLIMB "); any_warnings = true; }
    if (plane->pitch < -20) { printf("STEEP DESCENT "); any_warnings = true; }
    
    if (!any_warnings) printf("All systems normal");
    printf("\n");
}

// Напътствия за кацане
void show_landing_guidance(const Aircraft *plane) {
    printf("\n=================================================\n");
    printf("              LANDING GUIDANCE\n");
    printf("=================================================\n");
    
    printf("Altitude: %.0f m\n", plane->altitude);
    printf("Speed: %.0f km/h", plane->speed);
    
    if (plane->speed >= TARGET_LANDING_SPEED_MIN && plane->speed <= TARGET_LANDING_SPEED_MAX) {
        printf(" PERFECT\n");
    } else if (plane->speed > TARGET_LANDING_SPEED_MAX) {
        printf(" TOO FAST - Reduce speed!\n");
    } else {
        printf(" TOO SLOW - Increase speed!\n");
    }
    
    printf("Heading: %.0f°", plane->heading);
    fabs(plane->heading - 90) < 10 ? printf(" ALIGNED\n") : printf(" Adjust to 90°\n");
    
    printf("\nACTIONS NEEDED:\n");
    if (plane->speed > TARGET_LANDING_SPEED_MAX) {
        printf("* Press '-' to reduce speed (-50 km/h)\n");
        printf("* Press 'S' to descend gently\n");
    } else if (plane->speed < TARGET_LANDING_SPEED_MIN) {
        printf("* Press '+' to increase speed (+50 km/h)\n");
        printf("* Press 'W' to climb slightly\n");
    }
    
    if (plane->heading < 80) printf("* Press 'D' to turn right\n");
    else if (plane->heading > 100) printf("* Press 'A' to turn left\n");
    
    printf("=================================================\n");
}

// Физики

void update_aircraft(Aircraft *plane) {
    // Консумация на гориво
    if (plane->engine_on) {
        plane->fuel -= 0.5;
        if (plane->fuel < 0) plane->fuel = 0;
    }
    
    // Промяна на височината
    plane->altitude += plane->pitch * 5;
    if (plane->altitude < 0) plane->altitude = 0;
    if (plane->altitude > MAX_ALTITUDE) plane->altitude = MAX_ALTITUDE;
    
    // Промяна на скоростта
    if (!plane->engine_on) {
        plane->speed += 50;
        plane->pitch -= 2.5;
    }
    
    // Ефект от наклона
    if (plane->pitch > 0) plane->speed -= plane->pitch * 0.3;
    else if (plane->pitch < 0) plane->speed -= plane->pitch * 0.2;
    
    // Ограничения
    if (plane->speed < MIN_SPEED) plane->speed = MIN_SPEED;
    if (plane->speed > MAX_SPEED) plane->speed = MAX_SPEED;
    
    // Нормализиране на курса
    if (plane->heading >= 360) plane->heading -= 360;
    if (plane->heading < 0) plane->heading += 360;
}

void process_command(Aircraft *plane, char command) {
    // Преобразуване в малка буква
    if (command >= 'A' && command <= 'Z') command = command + 32;
    
    switch (command) {
        case 'w':  // Издигане
            plane->pitch += 2.5;
            if (plane->pitch > 30) plane->pitch = 30;
            printf(">>> Nose UP - Climbing\n");
            break;
            
        case 's':  // Спускане
            plane->pitch -= 2.5;
            if (plane->pitch < -30) plane->pitch = -30;
            printf(">>> Nose DOWN - Descending\n");
            break;
            
        case 'a':  // Наляво
            plane->heading -= 15;
            printf(">>> Turning LEFT\n");
            break;
            
        case 'd':  // Надясно
            plane->heading += 15;
            printf(">>> Turning RIGHT\n");
            break;
            
        case 'e':  // Двигател
            plane->engine_on = !plane->engine_on;
            plane->engine_on ? printf(">>> Engine STARTED\n") : printf(">>> Engine STOPPED\n");
            break;
            
        case '+':  // Увеличаване на скоростта
            plane->speed += SPEED_INCREMENT;
            if (plane->speed > MAX_SPEED) plane->speed = MAX_SPEED;
            printf(">>> Increasing speed (+50 km/h)\n");
            break;
            
        case '-':  // Намаляване на скоростта
            plane->speed -= SPEED_INCREMENT;
            if (plane->speed < MIN_SPEED) plane->speed = MIN_SPEED;
            printf(">>> Decreasing speed (-50 km/h)\n");
            break;
            
        default:
            printf(">>> Unknown command. Use W/A/S/D/E/+/-\n");
            break;
    }
}