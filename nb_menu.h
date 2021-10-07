#ifndef NB_MENU_H
#define NB_MENU_H


#include <stdio.h>

#include "nb_system.h"
#include "nb_rand.h"


#define NB_MAX_BODIES 65536
#define NB_PATH_MAX 4096


typedef struct nb_menu_run_t
{
    unsigned int seq: 1;
    unsigned int openmp: 1;
} nb_menu_run_t;


void nb_menu_loop(nb_system *const system);
void nb_menu_rand(nb_system *const system, nb_uint count,
    const nb_rand_settings *const settings);
void nb_menu_run_system(nb_system *const system, nb_float end_time,
    nb_float dt, nb_menu_run_t run);
void nb_menu_load_system(nb_system *const system, const char *const filename);
void nb_menu_save_system(const nb_system *const system,
    const char *const filename);
void nb_menu_print_system(const nb_system *const system, FILE* stream);


#endif
