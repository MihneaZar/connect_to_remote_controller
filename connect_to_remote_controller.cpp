#include "connect_to_remote_controller.hpp"

bool connect_to_destructor = false;

std::ofstream debug("debug.txt", std::ios::app);

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
    return command;
}

std::string parse_command_type(const std::string command, const std::vector<std::string> accepted_commands) {
    std::string type = command.substr(0, command.find(' '));
    
    // converting string to lowercase
    int length = type.length();
    for (int i = 0; i < length; i++) {
        if ('A' <= type[i] && type[i] <= 'Z') {
            type[i] += 'a' - 'A'; 
        }
    }

    bool type_found = false;
    for (auto poss_type: accepted_commands) {
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

int parse_command_parameter(std::string command) {
    // check if command can have parameter; if not, don't even worry about it
    if (count(PARAMETER_ACTIONS.begin(), PARAMETER_ACTIONS.end(), command) == 0) {
        return 0;
    }

    // first position of non-space character, after the command
    int first_pos = command.find_first_not_of(command.find(' '));

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
    int length = rand() % (MAX_PASS_LENGTH + 1);
    std::string password;
    for (int i = 0; i < length; i++) {
        password.push_back(rand_alfanum());
    }
    return password;
}

void mission_loop(const std::vector<std::string> accepted_commands) {
    // keeping the original coords, so that this function can be generalized
    // rather than starting on line 1, or any other specific line, it starts
    // on line original_coords.Y
    COORD original_coords = cursor_coords::get_instance()->get_screen_coords();

    // initialise map
    combat_zone::get_instance()->init_map();

    destructor_class *destructor = new destructor_class();
    while (true) {
        std::string command = get_engineer_command();

        // clean response to command (such as command list) -> needs to be before parsing,
        // so as to not delete the errors printed by it
        clean_lines(original_coords.Y + 1, 3);

        std::string type  = parse_command_type(command, accepted_commands);
        int add_parameter = parse_command_parameter(command);

        // the two commands for these checks have already printed the error, all that remains is to continue;
        if (add_parameter == PARAMETER_ERROR || !destructor->check_energy_cooldown(command, add_parameter)) {
            // clean last command
            clean_lines(original_coords.Y);
            continue;
        }   
        
        // !!! update help
        if (type == "help") {
            for (auto command_type: accepted_commands) {
                print_help(command_type);
            }
        }

        if (type == "energy") {
            destructor->generate_energy(add_parameter);
        }

        // checking just first character for direction (w|a|s|d)
        if (command[1] == ' ' && destructor->move(command[0])) {
            continue;
        }

        if (type == "scan") {
            destructor->scan(add_parameter);
        }

        // this is for the training simulators
        // this has no reason to exist as an actual combat action
        if (type == "continue") {
            clean_lines();
            return;
        }

        // all that's left is to print the map, everything else should be good (hopefully, fingers crossed)

        // clean last command
        clean_lines(original_coords.Y);
    }
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
    print_by_char("Remember this: although the RC can only send you snapshots of the system, it's all happening in real time - and time is of the essence.\n", false, CONTROLLER_INFO_STYLE);
    print_by_char("On top of that, each subsystem has its limitations: its abilities require energy, which you need to manually recharge, and they also have a cooldown.\n", false, CONTROLLER_INFO_STYLE);
    print_by_char("That said, get used to the controls in this first simulation, then hit 'c[ontinue]'.\n", false, CONTROLLER_INFO_STYLE);
    mission_loop(SIMULATOR_MOVEMENT);

    print_by_char("Now, the first ", false, CONTROLLER_INFO_STYLE);
    print_by_char("essential ", false, CONTROLLER_INFO_STYLE);
    print_by_char("lesson: ", false, CONTROLLER_INFO_STYLE);
    print_by_char("Keep. It. Away.\n", false, CONTROLLER_ERROR_STYLE);
    print_by_char("Before you worry yourself with how to fight it off, you need to remember one crucial detail - ", false, CONTROLLER_INFO_STYLE);
    print_by_char("a single slice ", false, CONTROLLER_ERROR_STYLE);
    print_by_char("of its blade-like appendages, and the ", false, CONTROLLER_INFO_STYLE);
    print_by_char("destructor is done.\n", false, CONTROLLER_ERROR_STYLE);
    print_by_char("On the very small off chance it misses the main structure of the generator, you may still end up with a broken subsystem: a generator with a lower capacity and energy output, slower tracks, scanner errors, and so on.", false, CONTROLLER_INFO_STYLE);
    print_by_char();
}

void init_controller() {
    set_term_color(CONTROLLER_TYPE_STYLE);

    cursor_coords::get_instance()->init_coords();
    cursor_coords::get_instance()->set_cursor();
    
    // rand() % 94 + 33 gives a number between 33 and 126, which is (approximately) where "normal" chars (usable for the name) are
    std::string random_name = {rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum()};
    print_by_char("Success! You are now connected to remote controller RC" + random_name + ".\n", false, CONTROLLER_SUCCES_STYLE);

    // init RC loop
    while (true) {
        std::string command = get_engineer_command();

        // clean response to command (such as command list) -> needs to be before parsing,
        // so as to not delete the errors printed by it
        clean_lines(2, 3);

        std::string type = parse_command_type(command, INIT_COMMANDS);

        if (type == "help") {
            for (auto command_type: INIT_COMMANDS) {
                print_help(command_type);
            }
        }

        if (type == "start") {
            // clear all printed lines
            clean_lines(0, 5);
            print_by_char("Press enter to receieve instructions in simulator training.\nIf you have used an RC to control a destructor before, type 's[kip]' to ignore this section.\n(Activating a destructor unit without simulator training is", false, CONTROLLER_INFO_STYLE);
            print_by_char(" ill", false, CONTROLLER_ERROR_STYLE);
            print_by_char("-", false, CONTROLLER_INFO_STYLE);
            print_by_char("advised", false, CONTROLLER_ERROR_STYLE);
            print_by_char(")\n", false, CONTROLLER_INFO_STYLE);
            std::string command = get_engineer_command();
            std::string type    = parse_command_type(command, {"", "skip", "exit"});
            clean_lines(0, 4);
            if (type != "skip") {
                training_simulator();
            }
            if (type != "exit") {
                connect_to_destructor = true;
            }
            return;
        }

        // controller ends
        if (type == "exit") {
            return;
        }

        // clean last command
        clean_lines(1);
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
    std::string        output;

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
    
    if (argc < 3) {
        CreateDirectory(".pass", NULL);
        SetFileAttributes(".pass", FILE_ATTRIBUTE_HIDDEN);
        std::string new_pass = random_password();
        std::ofstream write_pass(".pass/pass");
        write_pass << new_pass;
        write_pass.close();
        set_term_color(ERROR_TERM_STYLE);
        std::cout << "\nMust provide user and temporary password hash to initiate connection. New password generated.\n\n";
        set_term_color();
        return 0;
    }

    std::string user = (char*) argv[1];
    if (user != "sys_admin") {
        CreateDirectory(".pass", NULL);
        SetFileAttributes(".pass", FILE_ATTRIBUTE_HIDDEN);
        std::string new_pass = random_password();
        std::ofstream write_pass(".pass/pass");
        write_pass << new_pass;
        write_pass.close();
        set_term_color(ERROR_TERM_STYLE);
        std::cout << "\nOnly a system admin may connect to a remote controller. New password generated.\n\n";
        set_term_color();
        return 0;
    }

    std::ifstream read_pass(".pass/pass");
    std::string pass;
    read_pass >> pass;
    read_pass.close();

    std::string pass_hash = md5(pass);

    std::string temp_pass_hash = (char*) argv[2];

    // uncomment this for final version, as an easter egg
    // perhaps make the black-on-black text be decodable with some information from controller (perhaps if you win?)
    // reference administrator-X entity or whatever it is, and the hunter entities 
    // if (temp_pass_hash == ".debug") {
    //     set_term_color(INFO_TERM_STYLE);
    //     std::cout << "Entering debugging mode";
    //     for (int i = 0; i < THREE_DOTS; i++) {
    //         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //         std::cout << '.';
    //     }
    //     set_term_color(ERROR_TERM_STYLE);
    //     std::cout << "\n\nError: You are not ";
    //     set_term_color({BLACK, BLACK, NOT_BRIGHT, NOT_BRIGHT});
    //     std::cout << "<E00000X>";
    //     set_term_color(ERROR_TERM_STYLE);
    //     std::cout << '.';
    //     set_term_color({BLACK, BLACK, NOT_BRIGHT, NOT_BRIGHT});
    //     std::cout << "\n<V" + std::string({rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum(), rand_alfanum()}) + '>';
    //     set_term_color(ERROR_TERM_STYLE);
    //     std::cout << " has been dispatched.\n\n";
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
        std::cout << "\nTemporary password hash is incorrect. New password generated.\n\n";
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