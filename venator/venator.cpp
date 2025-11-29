#include "venator.hpp"

venator_class::venator_class() {
    SHORT map_size = combat_zone::get_instance()->get_map_size();

    // spawining venator in one of the four corners of the map
    switch (rand() % 4) {
        case 0: map_coords = {1, 1};
            break;
        
        case 1: map_coords = {1, map_size};
            break;

        case 2: map_coords = {map_size, 1};
            break;

        case 3: map_coords = {map_size, map_size};
            break;
    }

    combat_zone::get_instance()->set_map_object(map_coords, map_object::OBSTACLE);

    last_action_timestamp = current_time_ms();
}

void venator_class::action(destructor_class *destructor) {
    // one action per second
    int no_of_actions = time_ms_since(last_action_timestamp) / 1000;

    COORD target = destructor->get_map_coords();

    for (int i = 0; i < no_of_actions; i++) {

        // if it still has stamina, it takes an action
        // if it's out of stamina, it spends next actions recharging stamina
        // until stamina is back at max 
        if (0 < stamina && !recharge_stamina) {
            // checking if venator is in striking range
            // (if it is at a distance of at most 1 on both axis)
            if (!(abs(target.X - map_coords.X) <= 1 && abs(target.Y - map_coords.Y) <= 1)) {
                combat_zone::get_instance()->set_map_object(map_coords, map_object::EMPTY);
                int X_dif = target.X - map_coords.X;
                int Y_dif = target.Y - map_coords.Y;

                // the new coordinates of venator if it moves on either axis towards target
                COORD move_X = {map_coords.X + X_dif / abs(X_dif), map_coords.Y};
                COORD move_Y = {map_coords.X, map_coords.Y + Y_dif / abs(Y_dif)};

                // moving on the axis that is farther away (if that axis is not blocked by obstacle)
                if ((abs(X_dif) > abs(Y_dif) && combat_zone::get_instance()->get_map_object(move_X) != map_object::OBSTACLE) || combat_zone::get_instance()->get_map_object(move_Y) == map_object::OBSTACLE) {
                    // moving one unit in the correct direction
                    map_coords = move_X;
                } else {
                    // moving one unit in the correct direction
                    map_coords = move_Y;
                }

                combat_zone::get_instance()->set_map_object(map_coords, map_object::OBSTACLE);
            } else {
                // strike
            }
            stamina--;
        } else {
            stamina++;
            if (stamina < MAX_STAMINA) {
                recharge_stamina = true;
            } else {
                recharge_stamina = false;
            }
        }
    }
    last_action_timestamp = current_time_ms();
}