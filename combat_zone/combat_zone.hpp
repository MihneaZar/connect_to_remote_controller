#ifndef COMBAT_ZONE_HPP

#define COMBAT_ZONE_HPP

#include "../controller_console/controller_console.hpp"
#include <cstdlib>   // good ol' rand()

namespace map_object {
    // enum for character objects on the map
    enum enum_objects {
        EMPTY      = ' ',
        DESTRUCTOR = 'o',
        OBSTACLE   = '#',

        // debugging venator behaviour
        // could be a stronger scanner in the future
        // and counter to this would be the illusion/copy venator
        VENATOR_DEBUG = 'x',

        // we'll see how to implement this (errors should stay identical in the same scan)
        SCAN_ERROR = '*',

        // map edges
        // consider if u should use terminal edges
        CORNER     = '+',
        VERTICAL   = '|',
        HORIZONTAL = '-' 
    };
} // namespace map_object

std::string map_object_str(map_object::enum_objects);

// possible map sizes
// 100 was WAYYYY too big
const SHORT SMALL_MAP_SIZE  = 10;
const SHORT MEDIUM_MAP_SIZE = 20;
const SHORT LARGE_MAP_SIZE  = 30;

// percent chance of normal obstacles to spawn
const int OBSTACLE_CHANCE = 10;

// this is a singleton for the combat zone (map of the game)
class combat_zone {

    // actual map, of map_object::enum_objects
    // the +2 is for map edges
    // legal positions will be between (1, 1) and (map_size, map_size)
    // though map_size should be used only for initializations
    map_object::enum_objects area_map[LARGE_MAP_SIZE + 2][LARGE_MAP_SIZE + 2];

    // size of combat zone
    SHORT map_size;

    // static pointer to instance
    static combat_zone* instance;

    // private constructor
    combat_zone() {}

    // checks if there is an obstacle in the previous positions
    // (the three above positions and the behind one)
    // this is to keep at least a distance of one between obstacles
    // (makes for a much easier venator seeking algorithm)
    bool check_previous_positions(int line, int column);

public:
    // Deleting the copy constructor to prevent copies
    combat_zone(const combat_zone& obj) = delete;

    // static method to get instance
    static combat_zone* get_instance();

    void init_map(SHORT size = LARGE_MAP_SIZE);

    map_object::enum_objects get_map_object(COORD position);

    SHORT get_map_size();

    void set_map_object(COORD position, map_object::enum_objects object);

    // printing map separately to make sure all is good
    void debug_print();
};

#endif