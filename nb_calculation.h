#ifndef NB_CALCULATION_H
#define NB_CALCULATION_H


#define NB_CALCULATION_DEBUG


#include "nb_system.h"


void nb_euler_singlethread(nb_system *const system, nb_float dt);
void nb_euler_multithreading(nb_system *const system, nb_float dt);


#endif
