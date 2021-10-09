#include "nb_vector2.h"

#include <math.h>


void nb_vector2_init_default(nb_vector2 *const vec)
{ 
    vec->x = 0, vec->y = 0;
}

void nb_vector2_init(nb_vector2 *const vec, nb_float x, nb_float y)
{
    vec->x = x, vec->y = y;
}

void nb_vector2_copy(nb_vector2 *const vec, const nb_vector2 *const copy)
{
    vec->x = copy->x, vec->y = copy->y;
}

const nb_vector2* nb_vector2_assign(nb_vector2 *const vec,
    const nb_vector2 *const copy)
{
    if (vec != copy)
    {
        vec->x = copy->x;
        vec->y = copy->y;
    }
    
    return vec;
}

nb_vector2 nb_vector2_add(const nb_vector2 *const vec1,
    const nb_vector2 *const vec2)
{
    nb_vector2 tmp = {vec1->x + vec2->x, vec1->y + vec2->y};
    return tmp;
}

nb_vector2 nb_vector2_sub(const nb_vector2 *const vec1,
    const nb_vector2 *const vec2)
{
    nb_vector2 tmp = {vec1->x - vec2->x, vec1->y - vec2->y};
    return tmp;
}

nb_vector2 nb_vector2_mul(const nb_vector2 *const vec, nb_float scal)
{
    nb_vector2 tmp = {vec->x * scal, vec->y * scal};
    return tmp;
}

nb_vector2 nb_vector2_div(const nb_vector2 *const vec, nb_float scal)
{
    nb_vector2 tmp = {vec->x / scal, vec->y / scal};
    return tmp;
}

nb_float nb_vector2_scalar_mul(const nb_vector2 *const vec1,
    const nb_vector2 *const vec2)
{
    return vec1->x * vec2->x + vec1->y * vec2->y;
}

nb_float nb_vector2_norm(const nb_vector2 *const vec)
{
    return (nb_float)sqrt(vec->x * vec->x + vec->y * vec->y);
}

nb_vector2 nb_vector2_normalize(const nb_vector2 *const vec)
{
    nb_float norm = nb_vector2_norm(vec);
    nb_vector2 tmp = {vec->x / norm, vec->y / norm};
    
    return tmp;
}

nb_float nb_vector2_distance(const nb_vector2 *const vec1,
    const nb_vector2 *const vec2)
{
    nb_vector2 tmp = nb_vector2_sub(vec1, vec2);
    return nb_vector2_norm(&tmp);
}

bool nb_vector2_read(nb_vector2 *const vec, FILE* stream)
{
    bool is_read = fread((void*)vec, sizeof(nb_vector2), 1, stream) == 1;
    
    if (!is_read)
        vec->x = 0, vec->y = 0;
    
    return is_read;
}

bool nb_vector2_write(const nb_vector2 *const vec, FILE* stream)
{
    bool is_write = fwrite((const void*)vec, sizeof(nb_vector2), 
        1, stream) == 1;

    return is_write;        
}

bool nb_vector2_print(const nb_vector2 *const vec, FILE* stream)
{
    bool is_print = fprintf(stream, "(%lf, %lf)", vec->x, vec->y) > 0;

    return is_print;
}
