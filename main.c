#define NB_FLOAT_PRECISION 4
#define NB_INT_SIZE 16


#include <errno.h>

#include "menu.h"


int main() 
{
    nb_system system;

    nb_system_init_default(&system);
    if (errno == ENOMEM)
    {
        printf("Critical error: failed to initializing system.\n");
        return -1;
    }
    menu_loop(&system);

    return 0;
}
