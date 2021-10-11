#ifndef NB_SYSTEM_H
#define NB_SYSTEM_H


#define NB_SYSTEM_DEBUG


#include <stdio.h>

#include "nb_body.h"


typedef struct nb_system {
    nb_body* bodies;
    void* _calc_buf;  // buffer for new values of speed in calculation
    size_t count;
    size_t capacity;
    nb_float time;
} nb_system;


void nb_system_init_default(nb_system *const system);
void nb_system_copy(nb_system *const system, const nb_system *const copy);
const nb_system* nb_system_assign(nb_system *const system,
    const nb_system *const copy);
void nb_system_destroy(nb_system *const system);
void nb_system_add_body(nb_system *const system, const nb_body *const body);
void nb_system_remove_body(nb_system *const system, size_t index);
void nb_system_clear(nb_system *const system);
void nb_system_run(nb_system *const system, nb_float dt, bool parallel);
bool nb_system_read(nb_system *const system, FILE* stream);
bool nb_system_write(const nb_system *const system, FILE* stream);
bool nb_system_print(const nb_system *const system, FILE* stream);


#endif
