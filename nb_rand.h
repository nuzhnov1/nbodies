#ifndef NB_RAND_H
#define NB_RAND_H


#include "nb_body.h"


typedef struct nb_rand_settings
{
    nb_float min_coord, max_coord;
    nb_float min_speed, max_speed;
    nb_float min_mass, max_mass;
    nb_float min_radius, max_radius;
} nb_rand_settings;


nb_int nb_rand_int(nb_int min, nb_int max);
nb_int nb_rand_uint(nb_uint min, nb_uint max);
nb_float nb_rand_float(nb_float min, nb_float max);
void nb_rand_vector2(nb_vector2 *const vector, nb_float min,
    nb_float max);
void nb_rand_body(nb_body *const body, const char *const name,
    const nb_rand_settings *const settings);


#endif
