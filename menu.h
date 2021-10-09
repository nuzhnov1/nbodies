#ifndef MENU_H
#define MENU_H


#define MENU_DEBUG


#include "nb_system.h"
#include "nb_rand.h"


#define NB_MAX_BODIES 65536
#define NB_PATH_MAX 4096


typedef struct menu_run_t
{
    unsigned int seq: 1;
    unsigned int openmp: 1;
} menu_run_t;


void menu_loop(nb_system *const system);
void menu_rand(nb_system *const system, nb_uint count,
    const nb_rand_settings *const settings);
void menu_run_system(nb_system *const system, nb_float end_time,
    nb_float dt, menu_run_t run);
void menu_load_system(nb_system *const system, const char *const filename);
void menu_save_system(const nb_system *const system,
    const char *const filename);
void menu_print_system(const nb_system *const system, FILE* stream);


#endif
