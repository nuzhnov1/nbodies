#ifndef NB_BODY_H
#define NB_BODY_H


#define NB_NAME_MAX 256


#include "nb_vector2.h"


typedef struct nb_body
{
    char name[NB_NAME_MAX];
    nb_vector2 coords;
    nb_vector2 speed;
    nb_vector2 force;
    nb_float mass;
    nb_float radius;
} nb_body;


void nb_body_init_default(nb_body *const body);
void nb_body_init(nb_body *const body, const char* name,
    const nb_vector2 *const coords, const nb_vector2 *const speed, 
    const nb_vector2 *const force, nb_float mass, nb_float radius);
void nb_body_copy(nb_body *const body, const nb_body *const copy);
const nb_body* nb_body_assign(nb_body *const body, const nb_body *const copy);
bool nb_body_read(nb_body *const body, FILE* stream);
bool nb_body_write(const nb_body *const body, FILE* stream);
bool nb_body_print(const nb_body *const body, FILE* stream);


#endif
