#include "combat_zone.hpp"

std::string map_object_str(map_object::enum_objects object) {
    switch (object) {
        case map_object::EMPTY:      return " ";
        case map_object::DESTRUCTOR: return "o";
        case map_object::OBSTACLE:   return "#";

        case map_object::VENATOR_DEBUG: return "x";

        case map_object::SCAN_ERROR: return "*";

        case map_object::CORNER:     return "+";
        case map_object::VERTICAL:   return "|";
        case map_object::HORIZONTAL: return "-"; 
    }
    return " ";
}

combat_zone* combat_zone::instance = nullptr;

combat_zone* combat_zone::get_instance() {
    if (instance == nullptr) {
        instance = new combat_zone();
    }
    return instance;
}

void combat_zone::init_map(SHORT size) {
    map_size = size;

    // corners
    area_map[0][0]                       = map_object::CORNER;
    area_map[0][map_size + 1]            = map_object::CORNER;
    area_map[map_size + 1][0]            = map_object::CORNER;
    area_map[map_size + 1][map_size + 1] = map_object::CORNER;

    for (int i = 1; i < map_size + 1; i++) {
        // vertical edges
        area_map[i][0]            = map_object::VERTICAL; 
        area_map[i][map_size + 1] = map_object::VERTICAL;

        // horizontal edges
        area_map[0][i]            = map_object::HORIZONTAL; 
        area_map[map_size + 1][i] = map_object::HORIZONTAL;
    }

    // generating obstacles randomly
    for (int i = 1; i <= map_size; i++) {
        for (int j = 1; j <= map_size; j++) {

            // percentage between 1 and 100
            if ((rand() % 100 + 1) <= OBSTACLE_CHANCE) {
                area_map[i][j] = map_object::OBSTACLE;
            } else {
                area_map[i][j] = map_object::EMPTY;
            }
        }
    }
}

map_object::enum_objects combat_zone::get_map_object(COORD position) {
    return area_map[position.X][position.Y];
}

SHORT combat_zone::get_map_size() {
    return map_size;
}

void combat_zone::set_map_object(COORD position, map_object::enum_objects object) {
    area_map[position.X][position.Y] = object;
}

void combat_zone::debug_print() {
    for (int i = 0; i < map_size + 2; i++) {
        for (int j = 0; j < map_size + 2; j++) {
            cursor_coords::get_instance()->print_debug(map_object_str(area_map[i][j]));
        }
        cursor_coords::get_instance()->print_debug("\n");
    }
    cursor_coords::get_instance()->print_debug("\n");
}