#include <stdio.h>
#include <stdbool.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include "data.h"
#include "flight.h"

int main(void) {
    Aircraft plane = {
        .altitude = 3000,
        .speed = 400,
        .pitch = 0,
        .fuel = 100,
        .heading = 150,
        .engine_on = true,
        .time = 0
    };
    
    char command;
    bool game_running = true;
    
    welcome_message();
    
    // Продължителност на играта
    while (game_running && plane.fuel > 0) {
        plane.time++;
        
        // Показване на продължителността
        printf("\n\n\n\n\n=================================================\n");
        printf("          FLIGHT TIME: %d seconds\n", plane.time);
        printf("=================================================\n\n");
        
        // Показване на информация
        display_artificial_horizon(plane.pitch, plane.altitude);
        display_instruments(&plane);
        display_radar(&plane);
        display_warnings(&plane);
        
        // Проверка за кацане
        if (plane.altitude <= 100) show_landing_guidance(&plane);
        
        display_control_help();
        
        // Четене на команда
        printf("\n>>> Enter command: ");
        command = get_input_char();
        printf("%c\n", command);
        
        // Проверка за изход
        if (command == 'q' || command == 'Q') {
            printf("\n>>> Ending flight...\n");
            break;
        }
        
        // Обработка на командата
        process_command(&plane, command);
        
        // Обновяване на състоянието
        update_aircraft(&plane);
        
        // Проверка за край на играта
        if (plane.altitude <= 0) {
            if (plane.speed >= TARGET_LANDING_SPEED_MIN && plane.speed <= TARGET_LANDING_SPEED_MAX) {
                game_over_message(0, &plane);  // Успешно кацане
            } else if (plane.speed > TARGET_LANDING_SPEED_MAX) {
                game_over_message(1, &plane);  // Твърде бързо
            } else {
                game_over_message(2, &plane);  // Твърде бавно
            }
            game_running = false;
        }
        
        
        #ifdef _WIN32
            Sleep(150);
        #else
            usleep(150000);
        #endif
    }
    
    if (plane.fuel <= 0 && game_running) {
        game_over_message(3, &plane);  // Свършило гориво
    }
    
    printf("\nPress Enter to exit...");
    getchar();
    getchar();
    
    return 0;
}