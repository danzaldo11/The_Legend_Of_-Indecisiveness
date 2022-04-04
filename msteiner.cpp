// Madyson Steiner
// CMPS 3350
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

void msteiner_Check_Stamina(int stamina)
{
    //stamina = 100;

    if (stamina <= 0)
    {
        printf("You are out of breath, take a walk to heal.\n");
        stamina++;
    }
    else
    {
        printf("Stamina: %i\n", stamina);
        printf("Keep Running!\n");
        stamina --;
    }
}

int main()
{
    msteiner_Check_Stamina(100);

    return 0;
}
