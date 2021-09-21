#define NB_FLOAT_PRECISION 4
#define NB_INT_SIZE 16


#include "nb_menu.h"


int main() {
    nb_system system;

    nb_system_init_default(&system);

    nb_menu_loop(&system);
}
