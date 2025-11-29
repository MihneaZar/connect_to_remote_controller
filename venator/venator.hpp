#ifndef VENATOR_HPP

#define VENATOR_HPP

#include "../combat_zone/combat_zone.hpp"
#include "../destructor/destructor.hpp"
#include "../timer/timer.hpp"
#include <stdlib.h> // abs (not muscles)

// this is the venator class
class venator_class {
    // max stamina of venator
    // (how many actions it can take 
    // before needing to recharge)
    const int MAX_STAMINA = 5;

    // coordinates on the game map
    COORD map_coords;

    // when the venator last took an action
    // based on the time difference, it will take more actions
    // (for now) it takes one action every second
    uint64_t last_action_timestamp;

    // venator's stamina
    // when it reaches 0, it spends all actions
    // to recharge it back to MAX
    int stamina = MAX_STAMINA;
    bool recharge_stamina = false;

public:
    venator_class();

    // this is the primary action of the venator
    // (for now) either it moves closer to the destructor
    // or it attempts to strike
    void action(destructor_class *destructor);
};

#endif