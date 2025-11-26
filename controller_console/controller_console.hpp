#ifndef CONTROLLER_CONSOLE_HPP

#define CONTROLLER_CONSOLE_HPP

// computer screen edges (default ones don't work on mobile)
#if defined(_WIN32) || defined(_LINUX)
const char TOP_RIGHT    = (char) 218; // ┌
const char TOP_LEFT     = (char) 191; // ┐
const char BOTTOM_RIGHT = (char) 192; // └
const char BOTTOM_LEFT  = (char) 217; // ┘
const char VERTICAL     = (char) 179; // │
const char HORIZONTAL   = (char) 196; // ─
#else
const char TOP_RIGHT    = '|'; // ┌
const char TOP_LEFT     = '|'; // ┐
const char BOTTOM_RIGHT = '|'; // └
const char BOTTOM_LEFT  = '|'; // ┘
const char VERTICAL     = '|'; // │
const char HORIZONTAL   = '-'; // ─
#endif

#include <windows.h>     // for biutiuful calor
#include <iostream>
#include <conio.h>       // getch()
#include <chrono>        // < these two are for
#include <thread>        // < waiting in milisex

// screen size
// screen width should be odd so that destructor is not off center
const SHORT SCREEN_WIDTH  = 105;
const SHORT SCREEN_HEIGHT = 25;

// special keys
const int ENTER      =   13;
const int BACKSPACE  =    8;

// text and background color values
const int BG_COLOR_MULT = 16; // multiplier to apply color to background
const int BRIGHT        =  8; // add to obtain brighter version of color
const int NOT_BRIGHT    =  0; 
const int BLACK         =  0; // default background color
const int BLUE          =  1;
const int GREEN         =  2;
const int CYAN          =  3;
const int RED           =  4;
const int PINK          =  5;
const int YELLOW        =  6;
const int WHITE         =  7; // default text color

// easier transmission of text and background color
struct print_style {
    int text_color  = WHITE;
    int bg_color    = BLACK;
    int text_bright = NOT_BRIGHT;
    int bg_bright   = NOT_BRIGHT;
};

// this is a singleton for the absolute coordinates of the computer screen
// and the screen coordinates (coordinates of the cursor within the computer screen)
class cursor_coords {
    // coordinates for cursor
    // abs_coords                 -> the absolute coordinates of the first screen character (top left)
    // screen_coords              -> current coordinates on the screen, relative to the abs_coords
    // abs_coords + screen_coords -> absolute coordinates of current screen character
    COORD screen_coords, abs_coords;

    // static pointer to instance
    static cursor_coords* instance;

    // private constructor
    cursor_coords() {}

public:
    // Deleting the copy constructor to prevent copies
    cursor_coords(const cursor_coords& obj) = delete;

    // static method to get instance
    static cursor_coords* get_instance();

    void init_coords();

    void set_screen_coords(COORD new_coords);

    COORD get_screen_coords();

    COORD get_abs_coords();

    // sets cursor to new coordinates from screen_coords
    void set_cursor();
};

// terminal styles
const print_style DEFAULT_TERM_STYLE =  {WHITE, BLACK, NOT_BRIGHT, NOT_BRIGHT};
const print_style ERROR_TERM_STYLE   =    {RED, BLACK, NOT_BRIGHT, NOT_BRIGHT};
const print_style INFO_TERM_STYLE    = {YELLOW, BLACK, NOT_BRIGHT, NOT_BRIGHT};

// controller styles
const print_style CONTROLLER_TYPE_STYLE    =  {WHITE, BLUE, NOT_BRIGHT, NOT_BRIGHT};
const print_style CONTROLLER_INFO_STYLE    = {YELLOW, BLUE, NOT_BRIGHT, NOT_BRIGHT};
const print_style CONTROLLER_SUCCES_STYLE  =      {GREEN, BLUE, BRIGHT, NOT_BRIGHT};
const print_style CONTROLLER_ERROR_STYLE   =    {RED, BLUE, NOT_BRIGHT, NOT_BRIGHT};

// returns whether two styles are identical
// if style2 is not provided, style1 is checked against CONTROLLER_TYPE_STYLE
bool same_style(print_style style1, print_style style2 = CONTROLLER_TYPE_STYLE);

// sets the terminal color to given style 
// (if style is not given, returns to normal terminal colors)
void set_term_color(print_style style = DEFAULT_TERM_STYLE);

// prints the edges and the screen
void print_screen();

// function for printing list of commands
void print_help(std::string command_type);

// signifies times of waiting for ... in a loading info message
const int THREE_DOTS = 3;
const char ENTER_TO_CONTINUE[] = "\\";

// function to print text char by char (so as to not go out of bounds)
// keep_original_coords -> saves the coords before printing, and sets cursor and global screen_coords to them
// (for text editing like backspace or typing in the middle of the command, not at the end)
// style to change style of text, loading to wait before printing last no_of_loading elements in string
// print_by_char(); will print "Enter to continue." at end of page and wait for key press
// cleanup is specifically for the clean_lines command (so that text repositioning doesn't fuck with it)
void print_by_char(std::string text = ENTER_TO_CONTINUE, bool keep_original_coords = false, 
    print_style style = CONTROLLER_TYPE_STYLE, int no_of_loading = -1, bool cleanup = false);

const int CLEAN_SCREEN = -1;

// cleans lines between start_line and start_line + no_of_lines (including)
// sets cursor at start of start_line (or at its original place, if keep_original_coords is true)
// if only one parameter, cleans only that line
// clean_lines(); will clear entire screen
void clean_lines(SHORT start_line = CLEAN_SCREEN, SHORT no_of_lines = 1, bool keep_original_coords = false);

#endif