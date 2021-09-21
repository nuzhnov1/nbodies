#include "nb_calculation.h"
#include <math.h>


#define move_ptr(ptr, delta) ((void*)(ptr) + delta)


const nb_float gravity_const = 6.6743015e-11;


void nb_euler_singlethread(nb_system *const system, const nb_float dt) {
    const size_t delta = sizeof(nb_body);  // offset to pointers
    nb_body* bodies = system->bodies;      // vector of bodies
    size_t count = system->count;          // count of bodies
    nb_float time = system->time;          // current time

    // if there are no bodies in the system, then we do nothing    
    if (count == 0)
        return;

    nb_float* cx_i = &bodies[0].coords.x;  // pointer to vector of component "x" of coordinates
    nb_float* cy_i = &bodies[0].coords.y;  // pointer to vector of component "y" of coordinates
    nb_float* sx_i = &bodies[0].speed.x;   // pointer to vector of component "x" of speed
    nb_float* sy_i = &bodies[0].speed.y;   // pointer to vector of component "y" of speed
    nb_float* fx_i = &bodies[0].force.x;   // pointer to vector of component "x" of force
    nb_float* fy_i = &bodies[0].force.y;   // pointer to vector of component "y" of force
    nb_float* mass_i = &bodies[0].mass;      // pointer to vector of mass

    // Initializing all total forces for all bodies to 0
    for (size_t i = 0; i < count; i++) {
        *((nb_float*)move_ptr(fx_i, delta * i)) = 0;
        *((nb_float*)move_ptr(fy_i, delta * i)) = 0;
    }

    // Calculate forces, speeds and coordinates for all bodies in system
    for (size_t i = 0; i < count; i++) {
        nb_float* cx_k = cx_i; 
        nb_float* cy_k = cy_i;
        nb_float* fx_k = fx_i;
        nb_float* fy_k = fy_i;
        nb_float* mass_k = mass_i;

        // Calculate forces between body "i" and next bodies
        for (size_t j = i + 1; j < count; j++) {
            cx_k = move_ptr(cx_k, delta), cy_k = move_ptr(cy_k, delta);
            fx_k = move_ptr(fx_k, delta), fy_k = move_ptr(fy_k, delta);
            mass_k = move_ptr(mass_k, delta);

            nb_float dx = *cx_k - *cx_i;
            nb_float dy = *cy_k - *cy_i;

            // (dx ^ 2 + dy ^ 2) ^ (3 / 2) =>
            nb_float temp = dx * dx + dy * dy;
            temp = temp * (nb_float)sqrtl(temp);

            // scalar part of force
            nb_float scalar = gravity_const * (*mass_i) * (*mass_k) / temp;

            // components of force
            nb_float fx = dx * scalar;
            nb_float fy = dy * scalar;

            // The force acting on the body "i" relative to the body "j"
            *fx_i = *fx_i + fx;
            *fy_i = *fy_i + fy;

            // The force acting on the body "i" relative to the body "j"
            *fx_k = *fx_k - fx;
            *fy_k = *fy_k - fy;
        }

        // Calculate new speed of body "i"
        *sx_i = *sx_i + dt * (*fx_i) / (*mass_i);
        *sy_i = *sy_i + dt * (*fy_i) / (*mass_i);

        // Calculate new coordinates of body "j" 
        *cx_i = *cx_i + dt * (*sx_i);
        *cy_i = *cy_i + dt * (*sy_i);

        // moving to the next body
        cx_i = move_ptr(cx_i, delta);
        cy_i = move_ptr(cy_i, delta);
        sx_i = move_ptr(sx_i, delta);
        sy_i = move_ptr(sy_i, delta);
        fx_i = move_ptr(fx_i, delta);
        fy_i = move_ptr(fy_i, delta);
        mass_i = move_ptr(mass_i, delta);;
    }

    time += dt;
}

void nb_euler_multithreading(nb_system *const system, const nb_float dt) {

}
