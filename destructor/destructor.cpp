#include "destructor.hpp"

destructor_class::destructor_class() {// setting it smack in the middle on the horizontal
    // and 5 lines up from bottom to have space for max scans
    screen_coords.X = (SCREEN_WIDTH + 1) / 2;
    screen_coords.Y = SCREEN_HEIGHT - 1 - MAX_SCAN_RANGE;

    // spawning destructor roughly in the middle of the map
    // values here chosen arbitrarilly (maybe should change magic numbers)
    map_coords.X = combat_zone::get_instance()->get_map_size() / 2 + rand() % 11 - 5;
    map_coords.Y = combat_zone::get_instance()->get_map_size() / 2 + rand() % 11 - 5;
    combat_zone::get_instance()->set_map_object(map_coords, map_object::DESTRUCTOR);

    curr_energy_level = MAX_ENERGY_LEVEL;
    
    // setting initial scan at range 0, with center in destructor map_coords
    // this is just so that we don't accidentally read bad zones (and also avoid automatic scan at start)
    scan_range  =          0;
    scan_coords = map_coords;
}

COORD destructor_class::get_screen_coords() {
    return screen_coords;
}

void destructor_class::print_subsystem_status() {
    std::string energy_level = "Energy level: [" + std::string(curr_energy_level, '|') + std::string(MAX_ENERGY_LEVEL - curr_energy_level, '-') + "]\n";
    print_by_char(energy_level, false, CONTROLLER_INFO_STYLE);
}

bool destructor_class::check_energy_cooldown(std::string type, int parameter) {

    // if command is "energy", energy consumption is 0 (duh)
    // same goes for commands like "help" and "continue" (training simulators)
    int energy_consumption = 0;
    if (DIRECTIONS.find(type) != std::string::npos) {
        energy_consumption = 1;
    }

    if (type == "scan") {
        energy_consumption = 2 * parameter;
    }

    if (curr_energy_level < energy_consumption) {
        print_by_char("Error: energy level is too low.\n", false, CONTROLLER_ERROR_STYLE);
        return false;
    }

    return true;
}

void destructor_class::generate_energy(int add_charge) {
    if (MAX_ENERGY_LEVEL < curr_energy_level + 1 + add_charge) {
        print_by_char("Error: energy level too high, generator may overload.\n", false, CONTROLLER_ERROR_STYLE);
        return;
    }
    curr_energy_level += 1 + add_charge;
}

bool destructor_class::move(std::string command) {
    
    int direction = DIRECTIONS.find(command);

    if (direction == std::string::npos) {
        return false;
    }
    
    COORD new_position;
    // doing it separately to avoid a warning
    new_position.X = map_coords.X + D_MOVE[direction].X;
    new_position.Y = map_coords.Y + D_MOVE[direction].Y;

    switch (combat_zone::get_instance()->get_map_object(new_position)) {
        case map_object::EMPTY:
            // change last destructor position to EMPTY
            combat_zone::get_instance()->set_map_object(map_coords, map_object::EMPTY);

            // change destructor position
            map_coords = new_position;

            // change new destructor position to DESTRUCTOR
            combat_zone::get_instance()->set_map_object(new_position, map_object::DESTRUCTOR);
            curr_energy_level--;
            // set cooldown + timestamp
            break;

        case map_object::OBSTACLE: 
            print_by_char("Error: obstacle is blocking path.\n", false, CONTROLLER_ERROR_STYLE);
            break;

        // if space is not empty nor obstacle, then it's an edge (SCAN_ERRORS are not gonna be on the actual map)
        // NEED to change this if I add the ability to scan venators as 'x'
        default:
            print_by_char("Error: distance from RC critical. Moving farther risks connection loss.\n", false, CONTROLLER_ERROR_STYLE);        
    }

    return true;
}

void destructor_class::scan(int add_range) {
    if (MAX_SCAN_ADD_RANGE < add_range) {
        print_by_char("Error: parameter too high, scanner risks overload.\n", false, CONTROLLER_ERROR_STYLE);
        return;
    }

    scan_coords = map_coords;
    scan_range = BASE_SCAN_RANGE + add_range;

    // position of scan top left corner on the map
    COORD top_left;
    top_left.X = scan_coords.X - scan_range;
    top_left.Y = scan_coords.Y - scan_range;

    // for illegal positions on the combat map 
    // we put an empty space (' ') so that legal positions
    // keep their expected position in the scan map
    // in printing, spaces won't have any effect
    for (int i = 0; i < 2 * scan_range + 1; i++) {
        for (int j = 0; j < 2 * scan_range + 1; j++) {
            // checking that position is legal in map
            // including map edges
            if ((0 < i + top_left.X && i + top_left.X < combat_zone::get_instance()->get_map_size() + 2) 
                && (0 < j + top_left.Y && j + top_left.Y < combat_zone::get_instance()->get_map_size() + 2)) {
                
                COORD map_pos;
                map_pos.X = i + top_left.X;
                map_pos.Y = j + top_left.Y;
                scan_map[i][j] = combat_zone::get_instance()->get_map_object(map_pos);
                if (scan_map[i][j] == map_object::DESTRUCTOR) {
                    scan_map[i][j] = map_object::EMPTY;
                } 
            } else {
                scan_map[i][j] = map_object::EMPTY;
            }
        }
    }

    curr_energy_level -= 2 * add_range;
    // set cooldown + timestamp
}

void destructor_class::print_scan() {
    COORD d_move;
    d_move.X = scan_coords.X - map_coords.X;
    d_move.Y = scan_coords.Y - map_coords.X;

    COORD new_coords;
    new_coords.X = screen_coords.X + d_move.X - scan_range;
    new_coords.Y = screen_coords.X + d_move.Y - scan_range;
    cursor_coords::get_instance()->set_screen_coords(new_coords);
    cursor_coords::get_instance()->set_cursor();

    set_term_color(CONTROLLER_INFO_STYLE);

    for (int i = std::max({0, (int) d_move.X}); i < std::min({(int) d_move.X, 2 * scan_range + 1}); i++) {
        for (int j = std::max({0, (int) d_move.Y}); j < std::min({(int) d_move.Y, 2 * scan_range + 1}); j++) {
            // style for scan map???
            // dont print in destructor position
            std::cout << scan_map[i][j];
        }
        new_coords.Y++;
        cursor_coords::get_instance()->set_cursor(new_coords);
    }
}

