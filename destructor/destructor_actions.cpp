#include "destructor_actions.hpp"

int curr_energy_level = MAX_ENERGY_LEVEL;

int last_scan_range = 0;
COORD last_scan_position = {1, 1};

bool check_energy_cooldown(std::string command, int parameter) {
    // check cooldown
    return true;

    // if command is "energy", energy consumption is 0 (duh)
    // same goes for commands like "help" and "continue" (training simulators)
    int energy_consumption = 0;
    if (DIRECTIONS.find(command) != std::string::npos) {
        energy_consumption = 1;
    }

    if (command == "scan") {
        energy_consumption = 2 * parameter;
    }

    if (energy_consumption <= curr_energy_level) {
        print_by_char("Error: energy level is too low.\n", false, CONTROLLER_ERROR_STYLE);
        return false;
    }

    return true;
}

void generate_energy(int add_charge) {
    if (MAX_ENERGY_LEVEL < curr_energy_level + 1 + add_charge) {
        print_by_char("Error: energy level too high, generator may overload.\n", false, CONTROLLER_ERROR_STYLE);
        return;
    }
    curr_energy_level += 1 + add_charge;
}

void move_destructor(int direction) {
    // checking direction just for extra safety
    if (!(0 <= direction && direction < DIRECTION_NO)) {
        return;
    }
    
    COORD new_position;
    // doing it separately to avoid a warning
    new_position.X = destructor.X + D_MOVE[direction].X;
    new_position.Y = destructor.Y + D_MOVE[direction].Y;

    switch (combat_zone::get_instance()->get_map_object(new_position.X, new_position.Y)) {
        case EMPTY:
            // change last destructor position to EMPTY
            combat_zone::get_instance()->set_map_object(new_position.X, new_position.Y, EMPTY);

            // change destructor position
            destructor = new_position;

            // change new destructor position to DESTRUCTOR
            combat_zone::get_instance()->set_map_object(new_position.X, new_position.Y, DESTRUCTOR);
            curr_energy_level--;
            // set cooldown + timestamp
            break;

        case OBSTACLE: 
            print_by_char("Error: obstacle is blocking path.\n", false, CONTROLLER_ERROR_STYLE);
            break;

        // if space is not empty nor obstacle, then it's an edge (SCAN_ERRORS are not gonna be on the actual map)
        // NEED to change this if I add the ability to scan venators as 'x'
        default:
            print_by_char("Error: distance from RC critical. Moving farther risks connection loss.\n", false, CONTROLLER_ERROR_STYLE);
            
    }
}

void destructor_scan(int add_range) {
    if (MAX_SCANNER_ADD_RANGE < add_range) {
        print_by_char("Error: parameter too high, scanner risks overload.\n", false, CONTROLLER_ERROR_STYLE);
        return;
    }

    last_scan_position = destructor;
    last_scan_range = BASE_SCANNER_RANGE + add_range;
    curr_energy_level -= 2 * add_range;
    // set cooldown + timestamp
}