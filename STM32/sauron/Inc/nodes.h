//
// Created by August R. Mason on 12/10/17.
//

#ifndef STM32_NODES_H
#define STM32_NODES_H
//This is the nodes that will be run on the STM32
typedef struct node_descriptor {
    char name[32];
    void (*function)(void* params);
    unsigned long deadline;

} node_decriptor;

//list of nodes running on the robot
node_decriptor nodes[] = {
        {"killSwitch",    killSwitch,    15},
        {"missionSwitch", missionSwitch, 15},
        {"pwmStamped",    pwmStamped,    100}
};
#endif //STM32_NODES_H
