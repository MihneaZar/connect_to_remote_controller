#ifndef DESTRUCTOR_ACTIONS_HPP

#define DESTRUCTOR_ACTIONS_HPP

#include "../connect_to_remote_controller.hpp"

// checks specifically if the destructor has enough energy for an action
// and that its last cooldown has finished
// parameter is for actions that can have an argument (scan, energy)
bool check_energy_cooldown(std::string command, int parameter = 0);

// could be higher in the future for heavy destructors
const int MAX_ENERGY_LEVEL = 10;
extern int curr_energy_level;
void generate_energy(int add_charge = 0);

// for checking input in move_destructor
const int DIRECTION_NO = 4;
const std::string DIRECTIONS = "wasd";
// effect of move
const COORD D_MOVE[] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};

// moves destructor if move is valid
// returns energy consumption (1 if move happened, 0 otherwise)
void move_destructor(int direction);

const int MAX_SCANNER_ADD_RANGE  = 3;
const int BASE_SCANNER_RANGE     = 2;

// every time a scan happens, the scan range is saved (2 + range), 
// and the position (position of destructor when scan happened) 
extern int last_scan_range; // default 0, aka no scan
extern COORD last_scan_position;

void destructor_scan(int add_range = 0); 

#endif