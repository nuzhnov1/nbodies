#ifndef NB_VECTOR2_H
#define NB_VECTOR2_H


#include "nb_types.h"
#include <stdio.h>


typedef struct nb_vector2 {
    nb_float x, y;
} nb_vector2;


void nb_vector2_init_default(nb_vector2 *const vec);
void nb_vector2_init(nb_vector2 *const vec, const nb_float x, const nb_float y);
void nb_vector2_copy(nb_vector2 *const vec, const nb_vector2 *const copy);
const nb_vector2* nb_vector2_assign(nb_vector2 *const vec, const nb_vector2 *const copy);
nb_vector2 nb_vector2_add(const nb_vector2 *const vec1, const nb_vector2 *const vec2);
nb_vector2 nb_vector2_sub(const nb_vector2 *const vec1, const nb_vector2 *const vec2);
nb_vector2 nb_vector2_mul(const nb_vector2 *const vec, const nb_float scal);
nb_vector2 nb_vector2_div(const nb_vector2 *const vec, const nb_float scal);
nb_float nb_vector2_scalar_mul(const nb_vector2 *const vec1, const nb_vector2 *const vec2);
nb_float nb_vector2_norm(const nb_vector2 *const vec);
nb_vector2 nb_vector2_normalize(const nb_vector2 *const vec);
nb_float nb_vector2_distance(const nb_vector2 *const vec1, const nb_vector2 *const vec2);
bool nb_vector2_read(nb_vector2 *const vec, FILE* stream);
bool nb_vector2_write(const nb_vector2 *const vec, FILE* stream);
bool nb_vector2_print(const nb_vector2 *const vec, FILE* stream);


#endif
