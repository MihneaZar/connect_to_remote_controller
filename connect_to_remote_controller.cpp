#include "connect_to_remote_controller.hpp"

// this will be a variable which signifies current progress
// at least for now, it will be hidden within the password
// written in hex, starting at password_length / 2
// to hide it even better, a..f will have random capitalization
// ITS A..F FOR HEX GOD FUCKING DAMNIT-
int progress_state = 0;

std::string get_engineer_command() {
    print_by_char(">> ");
    
    std::string command;
    int pos_in_string = 0;
    char input = ' ';
    while (true) {
        input = getch();
        if (input == ENTER) {
            // forcing a newline
            print_by_char("\n");
            break;
        } else if (input == BACKSPACE) {
            if (pos_in_string == 0) {
                continue;
            }

            if (cursor_coords::get_instance()->get_screen_coords().X == 0) {
                COORD new_coords;
                new_coords.X = SCREEN_WIDTH;
                new_coords.Y = cursor_coords::get_instance()->get_screen_coords().Y - 1;
                cursor_coords::get_instance()->set_screen_coords(new_coords);
            } else {
                COORD new_coords;
                new_coords.X = cursor_coords::get_instance()->get_screen_coords().X - 1;
                new_coords.Y = cursor_coords::get_instance()->get_screen_coords().Y;
                cursor_coords::get_instance()->set_screen_coords(new_coords);
            }
            pos_in_string--;
            if (pos_in_string == command.length()) {
                command.pop_back();
                std::cout << input;
                std::cout << ' ';
                std::cout << input; 
            } else {
                command.erase(pos_in_string, 1);
                cursor_coords::get_instance()->set_cursor();
                print_by_char(command.substr(pos_in_string) + ' ', true);
            }
        } else if ((int) input == ARROW_KEY) {
            input = getch();

            if (input == ARROW_LEFT) {
                if (pos_in_string == 0) {
                    continue;
                }

                if (cursor_coords::get_instance()->get_screen_coords().X == 0) {
                    COORD new_coords;
                    new_coords.X = SCREEN_WIDTH;
                    new_coords.Y = cursor_coords::get_instance()->get_screen_coords().Y - 1;
                    cursor_coords::get_instance()->set_screen_coords(new_coords);
                } else {
                    COORD new_coords;
                    new_coords.X = cursor_coords::get_instance()->get_screen_coords().X - 1;
                    new_coords.Y = cursor_coords::get_instance()->get_screen_coords().Y;
                    cursor_coords::get_instance()->set_screen_coords(new_coords);
                }
                pos_in_string--;
            }

            if (input == ARROW_RIGHT) {
                if (pos_in_string < command.length()) {
                    COORD new_coords;
                    new_coords.X = cursor_coords::get_instance()->get_screen_coords().X + 1;
                    new_coords.Y = cursor_coords::get_instance()->get_screen_coords().Y;
                    cursor_coords::get_instance()->set_screen_coords(new_coords);
                    pos_in_string++;
                }
            }
            cursor_coords::get_instance()->set_cursor();
        } else {
            if (cursor_coords::get_instance()->get_screen_coords().X == SCREEN_WIDTH) {
                COORD new_coords;
                new_coords.X = 0;
                new_coords.Y = cursor_coords::get_instance()->get_screen_coords().Y + 1;
                cursor_coords::get_instance()->set_screen_coords(new_coords);
                cursor_coords::get_instance()->set_cursor();
            }
            pos_in_string++;
            COORD new_coords;
            new_coords.X = cursor_coords::get_instance()->get_screen_coords().X + 1;
            new_coords.Y = cursor_coords::get_instance()->get_screen_coords().Y;
            cursor_coords::get_instance()->set_screen_coords(new_coords);
            std::cout << input;
            if (pos_in_string == command.length() + 1) {
                command += input;
            } else {
                command = command.substr(0, pos_in_string - 1) + input + command.substr(pos_in_string - 1);
                print_by_char(command.substr(pos_in_string), true);
            }
        }
    }

    // converting command to lowercase
    int length = command.length();
    for (int i = 0; i < length; i++) {
        if ('A' <= command[i] && command[i] <= 'Z') {
            command[i] += 'a' - 'A'; 
        }
    }
    return command;
}

std::string parse_command_type(const std::string command, const std::vector<std::string> accepted_commands) {
    if (command == "") {
        return "";
    }

    std::string type = command.substr(0, command.find(' '));

    int length = type.length();

    bool type_found = false;
    for (auto poss_type: accepted_commands) {

        // separate check for movement (w|a|s|d) command
        // if (poss_type == DIRECTIONS && DIRECTIONS.find(type) != std::string::npos) {
        //     type = poss_type;
        //     type_found = true;
        //     break;
        // }

        type_found = true;
        for (int i = 0; i < length; i++) {
            if (type[i] != poss_type[i]) {
                type_found = false;
                break;
            }
        }

        if (type_found) {
            type = poss_type;
            break;
        }
    }

    // command not found in available commands
    if (!type_found) {
        type = "error";
        print_by_char("Error: Unknown command. Type 'h[elp]' to list commands.\n", false, CONTROLLER_ERROR_STYLE);
    }

    return type;
}

int parse_command_parameter(std::string command, std::string type) {
    // check if command can have parameter; if not, don't even worry about it
    // I need to check the TYPE here, duh...
    if (count(PARAMETER_ACTIONS.begin(), PARAMETER_ACTIONS.end(), type) == 0) {
        return 0;
    }

    // first position of non-space character, after the command
    // i forgor to add the thing im searching for =(((
    int first_pos = command.find_first_not_of(' ', command.find(' ') + 1);

    // no parameter was given
    if (command.find(' ') == std::string::npos || first_pos == std::string::npos) {
        return 0;
    }

    // if command[first_pos] is not a number, print an error and return -1
    if (!('0' <= command[first_pos] && command[first_pos] <= '9')) {
        print_by_char("Error: given parameter is not numerical.\n", false, CONTROLLER_ERROR_STYLE);
        return PARAMETER_ERROR;
    }
    return stoi(command.substr(first_pos, 1));
}

char rand_alfanum() {
    int type = rand() % 3;
    if (type == 0) {
        // random lowercase letter
        return (char) ('a' + rand() % 26);
    }

    if (type == 1) {
        // random uppercase letter
        return (char) ('A' + rand() % 26);
    }

    // random number
    return (char) ('0' + rand() % 9); 
}

std::string random_password() {
    int pow16 = 1;
    
    // finding biggest power of 16 
    // smaller or equal to progress state
    while (pow16 * 16 <= progress_state) {
        pow16 *= 16;    
    }

    std::string hidden_progress;
    // avoiding from changing the global variable
    int cp_progress_state = progress_state;
    while (pow16 > 0) {
        // digit in hex
        int digit = 0;
        while (pow16 * (digit + 1) <= cp_progress_state) {
            digit++;
        }
        // a..f
        if (digit >= 10) {
            if (rand() % 2 == 0) {
                // not capitalized
                hidden_progress += (char) 'a' + digit - 10;
            } else {
                // capitalized 
                hidden_progress += (char) 'A' + digit - 10;
            }
        } else {
            hidden_progress += (char) '0' + digit;
        }
        cp_progress_state -= pow16 * digit;
        pow16 /= 16;
    }

    int hidden_length = hidden_progress.length();

    // the +1 is for a random, non-a..f letter (randomly capitalized)
    // so that the program knows where the sequence for current progress ends
    int length = rand() % (MAX_PASS_LENGTH + 1 - (MIN_PASS_LENGTH + hidden_length + 1)) + MIN_PASS_LENGTH;
    std::string password;
    for (int i = 0; i < length; i++) {
        password.push_back(rand_alfanum());
    }

    int total_length = length + hidden_length;

    // padding hidden_progress on the right if the total length is even
    // for whatever reason, when it is even, the program can't read the progress properly 
    // if (total_length % 2 == 0) {
    //     hidden_progress = rand_alfanum() + hidden_progress;
    // }
    
    // inserting a random non-a..f letter
    // THERES 6 U BUFFOON
    char random_char = (char) 'a' + rand() % 21 + 6; 
    if (rand() % 2 == 1) {
        random_char = (char) 'A' + rand() % 21 + 6; 
    }
    return password.substr(0, (total_length - 1) / 2) + hidden_progress + random_char + password.substr(total_length / 2);
}

int get_progress_state(std::string pass) {
    int state = 0;
    int position = (pass.length() - 1) / 2;
    while(true) {
        bool still_valid = false;

        // 0..9 digits
        if ('0' <= pass[position] && pass[position] <= '9') {
            state = state * 16 + pass[position] - '0';
            still_valid = true;
        }

        // a..F
        if ('a' <= pass[position] && pass[position] <= 'f') {
            state = state * 16 + pass[position] - 'a' + 10;
            still_valid = true;
        }

        // A..F
        if ('A' <= pass[position] && pass[position] <= 'F') {
            state = state * 16 + pass[position] - 'A' + 10;
            still_valid = true;
        }

        if (!still_valid) {
            break;
        }
        position++;
    }
    return state;
}

void training_simulator() {
    print_by_char("Welcome, engineer! This is a short introduction (or more hopefully, a reminder) of the abilities of our venator foes, and the counters of a destructor unit, designed by the great Pashiv Itcha and his team of brilliant engineers.\n", false, CONTROLLER_SUCCES_STYLE);
    print_by_char("There are very few accounts of what a venator is, or what it looks like. We do know it is a heavy, two-meter tall humanoid creature, and likes to wear some sort of short cape on its back. Therefore, it must have some low level of self-awareness and intelligence, and to. That. Point.\n", false, CONTROLLER_SUCCES_STYLE);
    print_by_char("DO NOT underestimate them.\n\n", false, CONTROLLER_ERROR_STYLE);
    print_by_char("The reason why we're in this shit in the first place, is because they've been able to trick our AI systems.\n", false, CONTROLLER_INFO_STYLE);
    print_by_char("It will toy with you. It won't attack when you expect, and it will when you don't.\n", false, CONTROLLER_INFO_STYLE);
    print_by_char("They have incredible cloaking abilities. ", false, CONTROLLER_INFO_STYLE);
    print_by_char("Anything ", false, CONTROLLER_ERROR_STYLE);
    print_by_char("can be a venator. The only possitive is that Itcha's team have been able to develop a scanner that can track them.\n", false, CONTROLLER_INFO_STYLE);
    print_by_char("The negative is that the scanner can't tell a rock from a tree.\n", false, CONTROLLER_INFO_STYLE);
    print_by_char("Watch everything.\n", false, CONTROLLER_ERROR_STYLE);
    print_by_char();

    print_by_char("Alright, before we pit you against a simulated venator, let's get you used to the controls.\n", false, CONTROLLER_INFO_STYLE);
    print_by_char("Remember this: although the RC can only send you snapshots of the system's status, it's all happening in real time - and time is of the essence.\n", false, CONTROLLER_INFO_STYLE);
    print_by_char("On top of that, each subsystem has its limitations: its abilities require energy, which you need to manually recharge, and they also have a cooldown.\n", false, CONTROLLER_INFO_STYLE);
    print_by_char("That said, get used to the controls in this first simulation, then hit 'c[ontinue]'.\n", false, CONTROLLER_INFO_STYLE);

    // initialise map
    // combat_zone::get_instance()->init_map();
    // mission_loop(SIMULATOR_MOVEMENT, new destructor_class);

    print_by_char("Now, the first essential lesson: ", false, CONTROLLER_INFO_STYLE);
    print_by_char("Stay. Away. From the venator.\n", false, CONTROLLER_ERROR_STYLE);
    print_by_char("Before you worry yourself with how to fight it off, you need to remember one crucial detail - ", false, CONTROLLER_INFO_STYLE);
    print_by_char("a single slice ", false, CONTROLLER_ERROR_STYLE);
    print_by_char("of its blade-like appendages, and the ", false, CONTROLLER_INFO_STYLE);
    print_by_char("destructor is done.\n", false, CONTROLLER_ERROR_STYLE);
    print_by_char("On the very small off chance it misses the main structure of the generator, you may still end up with a broken subsystem: a generator with a lower capacity and energy output, slower tracks, scanner errors, and so on. Before proceding, press any button to activate the simulated venator.\n", false, CONTROLLER_INFO_STYLE);
    // print_by_char();
    char input = getch();

    // combat_zone::get_instance()->init_map(MEDIUM_MAP_SIZE);
    // mission_loop(SIMULATOR_MOVEMENT, new destructor_class, new venator_class);
}

void init_controller() {
    cursor_coords::get_instance()->init_coords();
    cursor_coords::get_instance()->set_cursor();

    print_by_char("Searching for active controller\\l.\n", false, CONTROLLER_INFO_STYLE, rand() % 3 + 1);
    std::string RC_name = {rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum()};
    print_by_char("Remote controller found: <RC" + RC_name + ">.\n", false, CONTROLLER_INFO_STYLE);
    print_by_char("Establishing connection\\l.\n", false, CONTROLLER_INFO_STYLE, rand() % 2 + 1);
    print_by_char("Success! You are now connected to remote controller <RC" + RC_name + ">.\n", false, CONTROLLER_SUCCES_STYLE);

    // init RC loop
    while (true) {
        std::string command = get_engineer_command();

        // clean response to command (such as command list) -> needs to be before parsing,
        // so as to not delete the errors printed by it
        clean_lines(5, 3);

        std::string type = parse_command_type(command, INIT_COMMANDS);

        if (type == "help") {
            for (auto command_type: INIT_COMMANDS) {
                print_help(command_type, progress_state);
            }
        }

        if (type == "start") {
            // entering simulator
            if (progress_state == 0) {
                training_simulator();
                progress_state = 1;
            }
            return;
        }

        // controller ends
        if (type == "exit") {
            return;
        }

        if (type == "") {  
            print_by_char("Error: Unknown command. Type 'h[elp]' to list commands.\n", false, CONTROLLER_ERROR_STYLE);
        }

        // clean last command
        clean_lines(4);
    }
}

void end_controller() {
    // when the program is finished,
    // set cursor to the second next line after the screen 
    cursor_coords::get_instance()->set_screen_coords({0, SCREEN_HEIGHT + 1});
    cursor_coords::get_instance()->set_cursor();

    // set new password, for security
    CreateDirectory(".pass", NULL);
    SetFileAttributes(".pass", FILE_ATTRIBUTE_HIDDEN);
    std::string new_pass = random_password();
    std::ofstream write_pass(".pass/pass");
    write_pass << new_pass;

    // reset terminal to its normal colors
    set_term_color();
}

std::string md5(const std::string content) {
    EVP_MD_CTX*   context = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_md5();
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int  md_len;
    std::string output;

    EVP_DigestInit_ex2(context, md, NULL);
    EVP_DigestUpdate(context, content.c_str(), content.length());
    EVP_DigestFinal_ex(context, md_value, &md_len);
    EVP_MD_CTX_free(context);

    output.resize(md_len * 2);
    for (unsigned int i = 0 ; i < md_len ; i++) {
        std::sprintf(&output[i * 2], "%02x", md_value[i]);
    }
    return output;
}

int main(int argc, char *argv[]) {
    srand(time(0)); // set seed for rand()

    // --help or --binary
    if (argc == 2) {
        std::string option = (char*) argv[1];
        if (option == "--help" || option == "-h") {
            std::cout << "\nConnect to remote controller.\n\n";
            std::cout << "Usage:\n\t./connect_to_remote_controller -h | --help\n\t./connect_to_remote_controller -b | --binary\n\t./connect_to_remote_controller --md5 <string_without_spaces>\n\t./connect_to_remote_controller <user_name> <password_hash>\n\n";
            std::cout << "Options:\n\t-h --help Print all executable usages (this page).\n\t-b --binary Run executable locally as an 8-bit binary-decimal converter.\n\t--md5 Convert given string to MD5 hash.\n\n";
            return 0;
        }

        if (option == "--binary" || option == "-b") {
            // run_binary_converter 
        }
    }

    std::ifstream read_pass(".pass/pass");
    std::string pass;
    read_pass >> pass;
    read_pass.close();

    progress_state = get_progress_state(pass);
    
    if (argc < 3) {
        CreateDirectory(".pass", NULL);
        SetFileAttributes(".pass", FILE_ATTRIBUTE_HIDDEN);
        progress_state++;
        std::string new_pass = random_password();
        std::ofstream write_pass(".pass/pass");
        write_pass << new_pass;
        write_pass.close();
        print_by_char("\nMust provide user and temporary password hash to initiate connection. New password generated.\n\n", false, ERROR_TERM_STYLE);
        set_term_color();
        return 0;
    }

    std::string first_arg = (char*) argv[1];

    // --md5
    if (first_arg == "--md5") {
        std::string unhashed_string = (char*) argv[2];
        std::cout << '\n' << md5(unhashed_string) << "\n\n";
        return 0;
    }

    std::string user = first_arg;
    if (user != "sys_admin") {
        CreateDirectory(".pass", NULL);
        SetFileAttributes(".pass", FILE_ATTRIBUTE_HIDDEN);
        std::string new_pass = random_password();
        std::ofstream write_pass(".pass/pass");
        write_pass << new_pass;
        write_pass.close();
        print_by_char("\nOnly a system admin may connect to a remote controller. New password generated.\n\n", false, ERROR_TERM_STYLE);
        set_term_color();
        return 0;
    }

    std::string pass_hash = md5(pass);

    std::string temp_pass_hash = (char*) argv[2];

    // uncomment this for final version, as an easter egg
    // perhaps make the black-on-black text be decodable with some information from controller (perhaps if you win?)
    // reference administrator-X entity or whatever it is, and the hunter entities 
    // if (temp_pass_hash == ".debug") {
    //     print_by_char("\nEntering debugging mode\\l.\n", false, INFO_TERM_STYLE, rand() % 2 + 1);
    //     print_by_char("Error: You are not ", false, ERROR_TERM_STYLE);
    //     print_by_char("<E00000X>", false, HIDDEN_TERM_STYLE);
    //     print_by_char(".\n", false, ERROR_TERM_STYLE);
    //     print_by_char("<V" + std::string({rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum()}) + '>', false, HIDDEN_TERM_STYLE);
    //     print_by_char(" has been dispatched.\n\n", false, ERROR_TERM_STYLE);
    //     set_term_color();
    //     return 0;
    // }

    if (temp_pass_hash != pass_hash && temp_pass_hash != ".debug") {
        CreateDirectory(".pass", NULL);
        SetFileAttributes(".pass", FILE_ATTRIBUTE_HIDDEN);
        std::string new_pass = random_password();
        std::ofstream write_pass(".pass/pass");
        write_pass << new_pass;
        write_pass.close();
        set_term_color(ERROR_TERM_STYLE);
        print_by_char("\nTemporary password hash is incorrect. New password generated.\n\n", false, ERROR_TERM_STYLE);
        set_term_color();
        return 0;
    }
    
    std::cout << "\033[2J\033[1;1H";
    print_screen();
    init_controller();
    
    // loop_controller();

    end_controller();
    return 0;
}