#ifndef DESTRUCTOR_ACTIONS_HPP

#define DESTRUCTOR_ACTIONS_HPP

#include "../controller_console/controller_console.hpp"
#include "../combat_zone/combat_zone.hpp"
#include "../timer/timer.hpp"
#include <vector>
#include <math.h> // for poooooow          
#include <map>  // for the cooldowns

// print style for destructor info
const print_style DESTRUCTOR_INFO_STYLE     =   {CYAN, BLUE, BRIGHT, NOT_BRIGHT};
// print style for destructor info
const print_style DESTRUCTOR_RED_STYLE      =    {RED, BLUE, BRIGHT, NOT_BRIGHT};
const print_style DESTRUCTOR_YELLOW_STYLE   = {YELLOW, BLUE, BRIGHT, NOT_BRIGHT};
const print_style DESTRUCTOR_GREEN_STYLE    =  {GREEN, BLUE, BRIGHT, NOT_BRIGHT};
// print style for obstacle on scan map
const print_style DESTRUCTOR_OBSTACLE_STYLE = {YELLOW, CYAN, BRIGHT, NOT_BRIGHT};
// print style for destructor on scan map
const print_style DESTRUCTOR_SELF_STYLE     =  {GREEN, CYAN, BRIGHT, NOT_BRIGHT};

const std::vector<std::string> DESTRUCTOR_SYSTEMS = {"Generator", "Engine", "Scanner"};

// for checking input in move
const int DIRECTION_NO = 4;
const std::string DIRECTIONS = "wasd";
// effect of move
const COORD D_MOVE[] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

// could be higher in the future for heavy destructors
const int MAX_ENERGY_LEVEL = 10;

// structure to keep track of cooldowns
struct command_cooldown {
    // the time when ability was last used (in ms)
    uint64_t    time_stamp;
    // the amount of time of cooldown (in ms)
    uint64_t cooldown_time;
};

const int MAX_SCAN_RANGE     = 5;
const int MAX_SCAN_ADD_RANGE = 3;
const int BASE_SCAN_RANGE    = 2;

// this is the destructor class
// technically, only one destructor makes sense to exist at a time
// however, since it only needs to exist inside mission_loop (and could be sent as a parameter if needed)
// no reason to make it a singleton
class destructor_class {
    // coordinates on the screen (fixed in place), coordinates on the game map
    COORD screen_coords, map_coords;

    std::map<std::string, command_cooldown> cooldown;

    SHORT curr_energy_level;

    // range, position (center), and map of last performed scan
    SHORT  scan_range;
    COORD scan_coords;
    map_object::enum_objects scan_map[2 * MAX_SCAN_RANGE + 1][2 * MAX_SCAN_RANGE + 1];

public:
    destructor_class();

    COORD get_screen_coords();

    static std::string command_type_to_system(std::string type);

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

    // printing energy and cooldowns on screen
    void print_subsystem_status();

    // printing last scan on screen
    void print_scan();
};







#endif