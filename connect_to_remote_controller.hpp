#ifndef CONNECT_TO_REMOTE_CONTROLLER_HPP

#define CONNECT_TO_REMOTE_CONTROLLER_HPP

// tehnically, MD5 is deprecated, but this is just for fun
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "controller_console/controller_console.hpp"
#include "destructor/destructor_actions.hpp"
#include <bits/stdc++.h> // for count (checking if value is in vector) - goddamn another include y a y
#include <openssl/evp.h> // for the md5 hashing (jesus christ that took forever)
#include <cstdlib>       // good ol' rand()
#include <fstream>       // for reading and writting password
#include <vector>
#include <ctime>         // get time seed for rand()

extern std::ofstream debug;

// arrow keys are composed of two getch's
const int ARROW_KEY   =  -32;
const int ARROW_UP    =   72;
const int ARROW_LEFT  =   75;
const int ARROW_DOWN  =   80;
const int ARROW_RIGHT =   77;

// function that returns md5 hash of string in hexadec format
// taken from last answer on https://stackoverflow.com/questions/7860362/how-can-i-use-openssl-md5-in-c-to-hash-a-string
// holy sheet this took forever to find (i am stoopid)
std::string md5(const std::string content);

// in case parse_command_parameter can't parse the parameter
// and has printed an error
const int PARAMETER_ERROR = -1;

// list of possible commands for each phase
// important for commands with identical starting letters (see "s" for south and "sc" for scan):
// put first the one with less starting letters (if it's only s, it's south, if it's sc, s will fail, then scan will succeed)
const std::vector<std::string> INIT_COMMANDS      = {"start", "help", "exit"};
const std::vector<std::string> SIMULATOR_MOVEMENT = {"energy", "wasd", "scan", "help", "continue"};
const std::vector<std::string> OPERATION_COMMANDS = {"help", "start", "exit"};
// actions that can have a parameter
const std::vector<std::string> PARAMETER_ACTIONS  = {"energy", "scan"};
// get command from engineer through RC
std::string get_engineer_command();
// parse command to get type (returns name as in given list of accepted commands, or "error" if no type matches)
// if command is "", returns first command in accepted_commands
std::string parse_command_type(const std::string command, const std::vector<std::string> accepted_commands);
// parse the parameter for commands that can take it
// for now, we assume the parameter is a single digit, 
// separated from the command by at least one space
// returns PARAMETER_ERROR if it can't parse the parameter
int parse_command_parameter(std::string command);

// possible map sizes
const SHORT SMALL_MAP_SIZE  =  25;
const SHORT MEDIUM_MAP_SIZE =  50;
const SHORT LARGE_MAP_SIZE  = 100;

// percent chance of normal obstacles to spawn
const int OBSTACLE_CHANCE = 10;

// enum for character objects on the map
enum map_object {
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
    MAP_CORNER     = '+',
    MAP_VERTICAL   = '|',
    MAP_HORIZONTAL = '-' 
};

// this is a singleton for the combat zone (map of the game)
class combat_zone{

    // actual map, of map_objects
    // the +2 is for map edges
    // legal positions will be between (1, 1) and (map_size, map_size)
    // though map_size should be used only for initializations
    enum map_object area_map[LARGE_MAP_SIZE + 2][LARGE_MAP_SIZE + 2];

    // size of combat zone
    SHORT map_size;

    // static pointer to instance
    static combat_zone* instance;

    // private constructor
    combat_zone() {}

public:
    // static method to get instance
    static combat_zone* get_instance();

    void init_map(SHORT size = LARGE_MAP_SIZE);

    enum map_object get_map_object(SHORT line, SHORT column);

    SHORT get_map_size();

    void set_map_object(SHORT line, SHORT column, enum map_object object);
};

// for now, we ignore the venator
// yet to decide whether multiple venators can be on the map -> copy venator? (could do that by just moving other obstacles randomly)
extern COORD destructor, venator;

// this initializes the map:
// sets map_size to given parameter;
// sets map edges;
// sets obstacles randomly;
// sets destructor and venator position (!!! for now, only destructor)
void init_combat_zone(int size = LARGE_MAP_SIZE);

// function for creating a random remote controller name
// (purely flavor text, at least for now)
char rand_alfanum();

const int MAX_PASS_LENGTH = 100;
// function for creating a random password of at most MAX_PASS_LENGTH characters
std::string random_password();

// main mission loop
// accepted commands for simulators (and perhaps different types of destructors in the future)
void mission_loop(const std::vector<std::string> accepted_commands);

// this is basically the tutorial (with some flavor text sprinkled between phases)
void training_simulator();

// this bool is set to true when the engineer actually 
// begins the main controller loop (actual gameplay)
extern bool connect_to_destructor;
// setting controller interface parameters
void init_controller();
// resetting terminal to normal
void end_controller();

// main function
int main(int argc, char *argv[]);

#endif