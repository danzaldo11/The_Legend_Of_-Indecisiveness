//3350
//program: danzaldo.cpp
//author:  Dylan Anzaldo
//date:    spring 2022
//
//Medieval map and sprite movement
//images courtesy: https://www.artstation.com/artwork/xJv4WW
//                 https://www.artstation.com/artwork/PmggkZ 
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

void danzaldo_gameover(int health) {
    if (health <= 0) {
        printf("You Died!\n");
    } else {
        printf("Health: %i\n", health);
    }   
}
