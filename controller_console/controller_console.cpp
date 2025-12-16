#include "controller_console.hpp"

// cursor_coords singleton functions START
cursor_coords* cursor_coords::instance = nullptr;

cursor_coords* cursor_coords::get_instance() {
    if (instance == nullptr) {
        instance = new cursor_coords();
    }
    return instance;
}

void cursor_coords::init_coords() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // get coordinates of cursor
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

    // setting abs_coords to the absolute coordinates of (0, 0) on the screen
    abs_coords    = csbiInfo.dwCursorPosition;
    abs_coords.X  = 1;
    abs_coords.Y -= SCREEN_HEIGHT;
    screen_coords = COORD{0, 0};
}

void cursor_coords::set_screen_coords(COORD new_coords) {
    screen_coords = new_coords;
}

COORD cursor_coords::get_abs_coords() {
    return abs_coords;
}

COORD cursor_coords::get_screen_coords() {
    return screen_coords;
}

void cursor_coords::set_cursor(COORD new_coords) {
    // check if ne screen coords were given
    if (new_coords.X != -1) {
        screen_coords = new_coords;
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    GetConsoleScreenBufferInfo(hConsole, &csbiInfo);
    csbiInfo.dwCursorPosition.X = abs_coords.X + screen_coords.X;
    csbiInfo.dwCursorPosition.Y = abs_coords.Y + screen_coords.Y;
    SetConsoleCursorPosition(hConsole, csbiInfo.dwCursorPosition);
}

void cursor_coords::print_debug(std::string text) {
    // openning and closing debug to see changes real time
    debug.open("debug.txt", std::ios_base::app);
    debug << text;
    debug.close();
}

void cursor_coords::toggle_cursor() {
    show_cursor = !show_cursor;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ciInfo;
    ciInfo.dwSize = 100;
    ciInfo.bVisible = show_cursor;
    SetConsoleCursorInfo(hConsole, &ciInfo);
}
// cursor_coords singleton functions END

bool same_style(print_style style1, print_style style2) {
    return style1.text_color  == style2.text_color
        && style1.bg_color    == style2.bg_color
        && style1.text_bright == style2.text_bright
        && style1.bg_bright   == style2.bg_bright;
}

void set_term_color(print_style style) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // change to desired terminal color
    SetConsoleTextAttribute(hConsole, style.text_color + style.text_bright + BG_COLOR_MULT * (style.bg_color + style.bg_bright));
}

void print_screen() {
    std::string top_edge    = TOP_RIGHT    + std::string(SCREEN_WIDTH, HORIZONTAL) +    TOP_LEFT + '\n';
    std::string bottom_edge = BOTTOM_RIGHT + std::string(SCREEN_WIDTH, HORIZONTAL) + BOTTOM_LEFT;

    std::cout << top_edge;

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        std::string screen_line = std::string(SCREEN_WIDTH, ' ');
        std::cout << VERTICAL;
        set_term_color(CONTROLLER_TYPE_STYLE);
        std::cout << screen_line;
        set_term_color();
        std::cout << VERTICAL << '\n';
    }

    std::cout << bottom_edge;
}

void print_help(std::string command_type) {
    if (command_type == "help") {
        print_by_char("-> h[elp] = this command (lists all available commands)\n", false, CONTROLLER_INFO_STYLE);
    }
    if (command_type == "start") {
        print_by_char("-> s[tart] = begin simulator training\n", false, CONTROLLER_INFO_STYLE);
    }
    if (command_type == "exit") {
        print_by_char("-> e[xit]  = sever connection to current remote controller\n", false, CONTROLLER_INFO_STYLE);
    }
    if (command_type == "continue") {
        print_by_char("-> c[ontinue] = move onto the next training phase\n", false, CONTROLLER_INFO_STYLE);
    }
    if (command_type == "energy") {
        print_by_char("-> e[nergy] [add_charge in (1..3)]: generate (1 + 2 * add_charge) energy, 0e, (2 + 2^add_charge)s\n", false, CONTROLLER_INFO_STYLE);
    }
    if (command_type == "wasd") {
        print_by_char("-> (w|a|s|d): movement, 1e, 1s\n", false, CONTROLLER_INFO_STYLE);
    }
    if (command_type == "scan") {
        print_by_char("-> sc[an] [add_range in (1..3)]: scan (2 + add_range) units away, (2 * add_range)e, (3 + add_range)s\n", false, CONTROLLER_INFO_STYLE);
    }
}

void print_by_char(std::string text, bool keep_original_coords, print_style style, int loading_cycles, bool cleanup) {
    cursor_coords::get_instance()->toggle_cursor();

    // "Enter to continue."
    if (text == ENTER_TO_CONTINUE) {
        cursor_coords::get_instance()->set_cursor({0, SCREEN_HEIGHT - 1});
        set_term_color(CONTROLLER_INFO_STYLE);
        std::cout << "Enter to continue.";
        while(getch() != ENTER);
        clean_lines();
        return;
    }

    if (!same_style(style)) {
        set_term_color(style);
    }

    COORD original_coords = cursor_coords::get_instance()->get_screen_coords();

    int length = text.length();
    for (int i = 0; i < length; i++) {
        if (cursor_coords::get_instance()->get_screen_coords().X == SCREEN_WIDTH) {
            COORD new_coords;
            new_coords.X = 0;
            new_coords.Y = cursor_coords::get_instance()->get_screen_coords().Y + 1;
            cursor_coords::get_instance()->set_cursor(new_coords);

            // if screen is full (for text-filled introductions)
            if (!cleanup && cursor_coords::get_instance()->get_screen_coords().Y == SCREEN_HEIGHT - 1) {
                set_term_color(CONTROLLER_INFO_STYLE);
                std::cout << "Enter to continue.";
                set_term_color(style);
                while(getch() != ENTER);
                clean_lines(0, SCREEN_HEIGHT);
            }
        }

        if (text[i] == '\n') {
            COORD new_coords;
            new_coords.X = 0;
            new_coords.Y = cursor_coords::get_instance()->get_screen_coords().Y + 1;
            cursor_coords::get_instance()->set_cursor(new_coords);
        } else if(text[i] == '\\' && i < length - 1 && text[i + 1] == 'l') { 
            if (loading_cycles > 0) {
                std::cout << "  ";
            }

            for (int cycle = 0; cycle < loading_cycles; cycle++) {
                std::cout << "\b\\";
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::cout << "\b|";
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::cout << "\b/";
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            std::cout << "\b \b\b";
            i++;
        } else {
            // ignore spaces at beginning of line (only if not from clean_lines)
            if (!(!cleanup && text[i] == ' ' && cursor_coords::get_instance()->get_screen_coords().X == 0)) {
                COORD new_coords;
                new_coords.X = cursor_coords::get_instance()->get_screen_coords().X + 1;
                new_coords.Y = cursor_coords::get_instance()->get_screen_coords().Y;
                cursor_coords::get_instance()->set_screen_coords(new_coords);
                std::cout << text[i];
            }
        }
        
        // skipping to next line, if next word is too long
        if (!cleanup && text[i] == ' ') {
            int next_space = text.find(' ', i + 1);
            int next_enter =    text.find('\n', i);

            // where the next word ends (including punctuation marks)
            int next_end = next_space - 1;
            // if there are no more spaces, or the distance to next '\n' is smaller
            if (next_space == std::string::npos || (next_enter != std::string::npos && next_enter < next_space)) {
                next_end = next_enter - 1;
            }

            if (next_end < 0) {
                next_end = length - 1;
            }

            // next_end - i = size of current word + punctuation mark
            // SCREEN_WIDTH - screen_coords.X = space left on line
            if (SCREEN_WIDTH - cursor_coords::get_instance()->get_screen_coords().X < next_end - i) {
                COORD new_coords;
                new_coords.X = 0;
                new_coords.Y = cursor_coords::get_instance()->get_screen_coords().Y + 1;
                cursor_coords::get_instance()->set_cursor(new_coords);
            }
        }
    }

    if (keep_original_coords) {
        cursor_coords::get_instance()->set_screen_coords(original_coords);
    }
    cursor_coords::get_instance()->set_cursor();
    set_term_color(CONTROLLER_TYPE_STYLE);

    cursor_coords::get_instance()->toggle_cursor();
}

void clean_lines(SHORT start_line, SHORT no_of_lines, bool keep_original_coords) {
    // clean screen for clean_lines();
    if (start_line == CLEAN_SCREEN) {
        start_line  = 0;
        no_of_lines = SCREEN_HEIGHT; 
    }

    COORD original_coords = cursor_coords::get_instance()->get_screen_coords();

    cursor_coords::get_instance()->set_cursor({0, start_line});
    print_by_char(std::string(no_of_lines * SCREEN_WIDTH, ' '), true, CONTROLLER_INFO_STYLE, 0, true);
    if (keep_original_coords) {
        cursor_coords::get_instance()->set_screen_coords(original_coords);
    } else {
        cursor_coords::get_instance()->set_screen_coords({0, start_line});
    }
    cursor_coords::get_instance()->set_cursor();
}