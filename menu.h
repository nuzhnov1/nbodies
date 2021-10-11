#ifndef MENU_H
#define MENU_H


#define MENU_DEBUG


#include <stdio.h>
#include <limits.h>

#include "nb_system.h"
#include "nb_rand.h"


#define NB_MAX_BODIES 65536


// Type of simulation run
typedef struct menu_run_t
{
    bool seq: 1;
    bool openmp: 1;
} menu_run_t;


extern nb_rand_settings default_rand_settings;


void menu_loop(nb_system *const system);
void menu_rand(nb_system *const system, nb_uint count,
    const nb_rand_settings *const settings);
void menu_run_system(nb_system *const system, nb_float end_time,
    nb_float dt, menu_run_t run);
bool menu_load_system(nb_system *const system, const char *const filename);
bool menu_save_system(const nb_system *const system,
    const char *const filename);
bool menu_print_system(const nb_system *const system, FILE* stream);


#endif
