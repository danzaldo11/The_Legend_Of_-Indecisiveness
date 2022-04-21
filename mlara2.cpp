// Author: Mariana Lara
// Project Software Eng.
//
// 

//Websites used:
// Girl Character: https://sanderfrenken.github.io/Universal-LPC-Spritesheet-Character-Generator/#?body=Humanlike_white&sex=female&shadow=none&nose=Straight_nose_tanned&eyes=Eyes_brown&hair=Curly_long_light_brown&hat=none&visor=none&earring=Earring_gold&necklace=Necklace_gold&shoulders=none&arms=none&dress=none&clothes=Tanktop_yellow&legs=Pants_teal&shoes=Sandals_brown&quiver=none&weapon=Thrust_shovel
// Beach Map:      https://www.deviantart.com/erichkisaragi/art/Beach-House-604911358
// Crab:           https://www.pngegg.com/en/png-klfdd


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
