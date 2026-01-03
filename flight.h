#ifndef FLIGHT_H
#define FLIGHT_H

#include "data.h"


void welcome_message(void);
void display_artificial_horizon(double pitch, double altitude);
void display_instruments(const Aircraft *plane);
void display_radar(const Aircraft *plane);
void display_warnings(const Aircraft *plane);
void show_landing_guidance(const Aircraft *plane);
void display_control_help(void);
char get_input_char(void);
void process_command(Aircraft *plane, char command);
void update_aircraft(Aircraft *plane);
void game_over_message(int reason, const Aircraft *plane);

#endif