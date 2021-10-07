#include "nb_rand.h"

#include <stdlib.h>


nb_int nb_rand_int(nb_int min, nb_int max)
{
    nb_int temp = rand();

    return temp % (max - min) - min;
}

nb_int nb_rand_uint(nb_uint min, nb_uint max)
{
    nb_uint temp = rand();

    return temp % (max - min) - min;
}

nb_float nb_rand_float(nb_float min, nb_float max)
{
    nb_float scale = rand() / (nb_float)RAND_MAX;

    return min + (max - min) * scale;
}

void nb_rand_vector2(nb_vector2 *const vector, nb_float min,
    nb_float max)
{
    nb_float x, y;

    x = nb_rand_float(min, max);
    y = nb_rand_float(min, max);

    nb_vector2_init(vector, x, y);
}

void nb_rand_body(nb_body *const body, const char *const name,
                  const nb_rand_settings *const settings)
{    
    nb_vector2 coords, speed, force;
    nb_float mass, radius;

    nb_rand_vector2(&coords, settings->min_coord, settings->max_coord);
    nb_rand_vector2(&speed, settings->min_speed, settings->max_speed);
    nb_vector2_init(&force, 0.0, 0.0);
    mass = nb_rand_float(settings->min_mass, settings->max_mass);
    radius = nb_rand_float(settings->min_radius, settings->max_radius);

    nb_body_init(body, name, &coords, &speed, &force, mass, radius);
}
