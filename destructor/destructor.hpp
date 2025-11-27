#ifndef DESTRUCTOR_ACTIONS_HPP

#define DESTRUCTOR_ACTIONS_HPP

#include "../connect_to_remote_controller.hpp"
#include "../combat_zone/combat_zone.hpp"

// for checking input in move
const int DIRECTION_NO = 4;
const std::string DIRECTIONS = "wasd";
// effect of move
const COORD D_MOVE[] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

// could be higher in the future for heavy destructors
const int MAX_ENERGY_LEVEL = 10;

const int MAX_SCAN_RANGE      = 5;
const int MAX_SCAN_ADD_RANGE  = 3;
const int BASE_SCAN_RANGE     = 2;

// this is the destructor class
// technically, only one destructor makes sense to exist at a time
// however, since it only needs to exist inside mission_loop (and could be sent as a parameter if needed)
// no reason to make it a singleton
class destructor_class {
    // coordinates on the screen (fixed in place), coordinates on the game map
    COORD screen_coords, map_coords;

    SHORT curr_energy_level;

    // range, position (center), and map of last performed scan
    SHORT  scan_range;
    COORD scan_coords;
    char scan_map[MAX_SCAN_RANGE][MAX_SCAN_RANGE];

public:
    destructor_class();

    COORD get_screen_coords();

    void print_subsystem_status();

    // checks specifically if the destructor has enough energy for an action
    // and that its last cooldown has finished
    // parameter is for actions that can have an argument (scan, energy)
    // CHECK TYPE
    bool check_energy_cooldown(std::string type, int parameter = 0);

    // generates energy according to parameter
    void generate_energy(int add_charge = 0);

    // moves destructor if move is valid
    // returns energy consumption (1 if move happened, 0 otherwise)
    bool move(std::string command);

    void scan(int add_range = 0); 

    void print_scan();
};







#endif