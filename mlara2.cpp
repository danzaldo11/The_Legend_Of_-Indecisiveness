// Author: Mariana Lara
// Project Software Eng.
//
// 

//Websites used:
//Character: https://www.pinterest.ie/pin/257338566191273090/
//Character: https://www.pngegg.com/en/png-eticx

//Map: https://www.deviantart.com/erichkisaragi/art/Beach-House-604911358

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
//#include "image.h"
//#include "credits.h"


//checks armour level
 void mlara2_armour(int a){
   
      if(a <= 0){
          printf("Armour level low, pause to level up.\n");
          a++;
 
      } else {
          printf("Armour: %i\n", a);
          a--;
      }
 }
