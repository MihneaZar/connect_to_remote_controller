#ifndef CONNECT_TO_REMOTE_CONTROLLER_HPP

#define CONNECT_TO_REMOTE_CONTROLLER_HPP

// tehnically, MD5 is deprecated, but this is just for fun
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "controller_console/controller_console.hpp"
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
// blank command for destructor gameplay, in order to get an updated status
const std::vector<std::string> INIT_COMMANDS      = {"start", "help", "exit"};
const std::vector<std::string> SIMULATOR_MOVEMENT = {"", "energy", "wasd", "scan", "help", "continue"};
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
// I need to check the TYPE here, duh...
int parse_command_parameter(std::string command, std::string type);

// function for creating a random remote controller name
// (purely flavor text, at least for now)
char rand_alfanum();

const int MAX_PASS_LENGTH = 128;
const int MIN_PASS_LENGTH = 16;
// function for creating a random password of at most MAX_PASS_LENGTH characters
std::string random_password();

// function for getting the progress state from a given password
int get_progress_state(std::string pass);

// main mission loop
// accepted commands for simulators (and perhaps different types of destructors in the future)
// receiving destructor and venator from outside so that the destructor type and venator type can be established there
// if venator is nullptr, then there is no venator (for simulator movement for now, perhaps very low chance of there not being a venator)
// void mission_loop(const std::vector<std::string> accepted_commands, destructor_class *destructor, venator_class *venator = nullptr);

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