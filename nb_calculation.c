#include "nb_calculation.h"
#include <math.h>
#include <omp.h>


#define move_ptr(ptr, size) ((void*)(ptr) + (size))


const nb_float gravity_const = 6.6743015e-11;


void nb_euler_singlethread(nb_system *const system, const nb_float dt) {
    const size_t size = sizeof(nb_body);   // offset to pointers
    nb_body* bodies = system->bodies;      // vector of bodies
    size_t count = system->count;          // count of bodies
    nb_float time = system->time;          // current time

    // if there are no bodies in the system, then we do nothing    
    if (count == 0)
        return;

    // const pointers to array of different components of bodies in system
    nb_float *const cx = &bodies[0].coords.x;  // "x" coordinates
    nb_float *const cy = &bodies[0].coords.y;  // "y" coordinates
    nb_float *const sx = &bodies[0].speed.x;   // "x" component of speed
    nb_float *const sy = &bodies[0].speed.y;   // "y" component of speed
    nb_float *const fx = &bodies[0].force.x;   // "x" component of force
    nb_float *const fy = &bodies[0].force.y;   // "y" component of force
    nb_float *const mass = &bodies[0].mass;    // mass

    // components of "i" body
    nb_float* cx_i = cx;
    nb_float* cy_i = cy;
    nb_float* sx_i = sx;
    nb_float* sy_i = sy;
    nb_float* fx_i = fx;
    nb_float* fy_i = fy;
    nb_float* mass_i = mass;

    // some components of body "j"
    nb_float* cx_j; 
    nb_float* cy_j;
    nb_float* fx_j;
    nb_float* fy_j;
    nb_float* mass_j;

    // Initializing all total forces for all bodies to 0
    for (size_t i = 0; i < count; i++) {
        *fx_i = 0;
        *fy_i = 0; 

        fx_i = move_ptr(fx_i, size);
        fy_i = move_ptr(fy_i, size);
    }
    fx_i = fx;
    fy_i = fy;

    // Calculate forces for all bodies in system
    for (size_t i = 0; i < count; i++) {
        cx_j = move_ptr(cx_i, size); 
        cy_j = move_ptr(cy_i, size);
        fx_j = move_ptr(fx_i, size);
        fy_j = move_ptr(fy_i, size);
        mass_j = move_ptr(mass_i, size);

        // Calculate forces between body "i" and all bodies "j", when j > i
        for (size_t j = i + 1; j < count; j++) {
            nb_float dx = *cx_j - *cx_i;
            nb_float dy = *cy_j - *cy_i;

            // (dx ^ 2 + dy ^ 2) ^ (3 / 2) =>
            nb_float temp = dx * dx + dy * dy;
            temp = temp * (nb_float)sqrtl(temp);

            // scalar part of force
            nb_float scalar = gravity_const * (*mass_i) * (*mass_j) / temp;

            // components of force
            nb_float fx_c = dx * scalar;
            nb_float fy_c = dy * scalar;

            // The force acting on the body "i" relative to the body "j"
            *fx_i += fx_c;
            *fy_i += fy_c;

            // The force acting on the body "j" relative to the body "i"
            *fx_j -= fx_c;
            *fy_j -= fy_c;

            cx_j = move_ptr(cx_j, size);
            cy_j = move_ptr(cy_j, size);
            fx_j = move_ptr(fx_j, size);
            fy_j = move_ptr(fy_j, size);
            mass_j = move_ptr(mass_j, size);
        }

        cx_i = move_ptr(cx_i, size);
        cy_i = move_ptr(cy_i, size);
        fx_i = move_ptr(fx_i, size);
        fy_i = move_ptr(fy_i, size);
        mass_i = move_ptr(mass_i, size);
    }
    cx_i = cx;
    cy_i = cy;
    fx_i = fx;
    fy_i = fy;
    mass_i = mass;

    // Calculate speed for all bodies in system
    for (size_t i = 0; i < count; i++) {
        *sx_i += dt * (*fx_i) / (*mass_i);
        *sy_i += dt * (*fy_i) / (*mass_i);

        sx_i = move_ptr(sx_i, size);
        sy_i = move_ptr(sy_i, size);
        fx_i = move_ptr(fx_i, size);
        fy_i = move_ptr(fy_i, size);
        mass_i = move_ptr(mass_i, size);
    }
    sx_i = sx;
    sy_i = sy;
    fx_i = fx;
    fy_i = fy;
    mass_i = mass;

    // Calculate new coordinates of body "j" 
    for (size_t i = 0; i < count; i++) {
        *cx_i += dt * (*sx_i);
        *cy_i += dt * (*sy_i);

        cx_i = move_ptr(cx_i, size);
        cy_i = move_ptr(cy_i, size);
        sx_i = move_ptr(sx_i, size);
        sy_i = move_ptr(sy_i, size);
    }

    time += dt;
}

void nb_euler_multithreading(nb_system *const system, const nb_float dt) {
    const size_t threads_count = (size_t)omp_get_max_threads();
    
    const size_t size = sizeof(nb_body);   // offset to pointers
    nb_body* bodies = system->bodies;      // vector of bodies
    size_t count = system->count;          // count of bodies
    nb_float time = system->time;          // current time

    // if there are no bodies in the system, then we do nothing    
    if (count == 0)
        return;
    
    omp_set_num_threads((int)threads_count);

    // const pointers to array of different components of bodies in system
    nb_float *const cx = &bodies[0].coords.x;  // "x" coordinates
    nb_float *const cy = &bodies[0].coords.y;  // "y" coordinates
    nb_float *const sx = &bodies[0].speed.x;   // "x" component of speed
    nb_float *const sy = &bodies[0].speed.y;   // "y" component of speed
    nb_float *const fx = &bodies[0].force.x;   // "x" component of force
    nb_float *const fy = &bodies[0].force.y;   // "y" component of force
    nb_float *const mass = &bodies[0].mass;    // mass

    // components of "i" body
    nb_float* cx_i;
    nb_float* cy_i;
    nb_float* sx_i;
    nb_float* sy_i;
    nb_float* fx_i;
    nb_float* fy_i;
    nb_float* mass_i;

    // some components of body "j"
    nb_float* cx_j; 
    nb_float* cy_j;
    nb_float* fx_j;
    nb_float* fy_j;
    nb_float* mass_j;
    
    // Initializing all total forces for all bodies to 0
    // TODO: specify the number in clause if
    #pragma omp parallel private(cx_i, cy_i, sx_i, sy_i, fx_i, fy_i, mass_i, \
                                 cx_j, cy_j, fx_j, fy_j, mass_j) \
                         shared(count, size, gravity_const, dt, cx, cy, \
                                sx, sy, fx, fy, mass) \
                         num_threads(threads_count) \
                         if (count > threads_count * 10)
    {
        #pragma omp for schedule(static, count / threads_count)
        for (size_t i = 0; i < count; i++)
        {
            fx_i = move_ptr(fx, size * i);
            fy_i = move_ptr(fy, size * i);
            
            *fx_i = 0;
            *fy_i = 0;

            // TODO: remove this aftet debugging
            #if NB_CALCULATION_DEBUG
            if (i == 0)
                printf("Was the force initialization block parallelized: %s.\n", \
                        omp_in_parallel() ? "true" : "false");
            #endif
        }
        
        // Calculate forces for all bodies in system
        // Non-efficient algorithm
        #if NB_CALCULATION_PARALLEL_ALGO == 1
        #pragma omp for schedule(static, count / threads_count)
        for (size_t i = 0; i < count; i++)
        {
            cx_i = move_ptr(cx, size * i);
            cy_i = move_ptr(cy, size * i);
            fx_i = move_ptr(fx, size * i);
            fy_i = move_ptr(fy, size * i);
            mass_i = move_ptr(mass, size * i);

            cx_j = move_ptr(cx_i, size); 
            cy_j = move_ptr(cy_i, size);
            fx_j = move_ptr(fx_i, size);
            fy_j = move_ptr(fy_i, size);
            mass_j = move_ptr(mass_i, size);

            for (size_t j = i + 1; j < count; j++)
            {
                // calculate force between body "i" and body "j"
                nb_float dx = *cx_j - *cx_i;
                nb_float dy = *cy_j - *cy_i;

                // (dx ^ 2 + dy ^ 2) ^ (3 / 2) =>
                nb_float temp = dx * dx + dy * dy;
                temp = temp * (nb_float)sqrtl(temp);

                // scalar part of force
                nb_float scalar = gravity_const * (*mass_i) * (*mass_j) / temp;

                // components of force
                nb_float fx_c = dx * scalar;
                nb_float fy_c = dy * scalar;

                // The force acting on the body "i" relative to the body "j"
                // WARNING: this code may leads to a race condition if not used sync methods!
                #pragma omp atomic
                *fx_i += fx_c;
                #pragma omp atomic
                *fy_i += fy_c;

                // The force acting on the body "j" relative to the body "i"
                // WARNING: this code may leads to a race condition if not used sync methods!
                #pragma omp atomic
                *fx_j -= fx_c;
                #pragma omp atomic
                *fy_j -= fy_c;

                cx_j = move_ptr(cx_j, size); 
                cy_j = move_ptr(cy_j, size);
                fx_j = move_ptr(fx_j, size);
                fy_j = move_ptr(fy_j, size);
                mass_j = move_ptr(mass_j, size);
            }

            // TODO: remove this aftet debugging
            #if NB_CALCULATION_DEBUG
            if (i == 0)
            {
                printf("Was the force calculation block parallelized: %s.\n", \
                        omp_in_parallel() ? "true" : "false");
            }
            #endif
        }
        #elif NB_CALCULATION_PARALLEL_ALGO == 2
        #pragma omp for schedule(static, count / threads_count)
        for (size_t i = 0; i < count; i++)
        {
            cx_i = move_ptr(cx, size * i);
            cy_i = move_ptr(cy, size * i);
            fx_i = move_ptr(fx, size * i);
            fy_i = move_ptr(fy, size * i);
            mass_i = move_ptr(mass, size * i);

            cx_j = cx; 
            cy_j = cy;
            fx_j = fx;
            fy_j = fy;
            mass_j = mass;

            for (size_t j = 0; j < count; j++)
            {
                if (i == j)
                {
                    cx_j = move_ptr(cx_j, size); 
                    cy_j = move_ptr(cy_j, size);
                    fx_j = move_ptr(fx_j, size);
                    fy_j = move_ptr(fy_j, size);
                    mass_j = move_ptr(mass_j, size);

                    continue;
                }
                
                // calculate force between body "i" and body "j"
                nb_float dx = *cx_j - *cx_i;
                nb_float dy = *cy_j - *cy_i;

                // (dx ^ 2 + dy ^ 2) ^ (3 / 2) =>
                nb_float temp = dx * dx + dy * dy;
                temp = temp * (nb_float)sqrtl(temp);

                // scalar part of force
                nb_float scalar = gravity_const * (*mass_i) * (*mass_j) / temp;

                // components of force
                nb_float fx_c = dx * scalar;
                nb_float fy_c = dy * scalar;

                // The force acting on the body "i" relative to the body "j"
                *fx_i += fx_c;
                *fy_i += fy_c;

                cx_j = move_ptr(cx_j, size); 
                cy_j = move_ptr(cy_j, size);
                fx_j = move_ptr(fx_j, size);
                fy_j = move_ptr(fy_j, size);
                mass_j = move_ptr(mass_j, size);
            }

            // TODO: remove this aftet debugging
            #if NB_CALCULATION_DEBUG
            if (i == 0)
            {
                printf("Was the force calculation block parallelized: %s.\n", \
                        omp_in_parallel() ? "true" : "false");
            }
            #endif
        }
        #endif

        // Calculate speed for all bodies in system
        #pragma omp for schedule(static, count / threads_count)
        for (size_t i = 0; i < count; i++)
        {
            sx_i = move_ptr(sx, size * i);
            sy_i = move_ptr(sy, size * i);
            fx_i = move_ptr(fx, size * i);
            fy_i = move_ptr(fy, size * i);
            mass_i = move_ptr(mass, size * i);

            *sx_i += dt * (*fx_i) / (*mass_i);
            *sy_i += dt * (*fy_i) / (*mass_i);

            // TODO: remove this aftet debugging
            #if NB_CALCULATION_DEBUG
            if (i == 0)
            {
                printf("Was the speed calculation block parallelized: %s.\n", \
                        omp_in_parallel() ? "true" : "false");
            }
            #endif
        }

        // Calculate new coordinates of body "j"
        #pragma omp for schedule(static, count / threads_count)
        for (size_t i = 0; i < count; i++)
        {
            cx_i = move_ptr(cx, size * i);
            cy_i = move_ptr(cy, size * i);
            sx_i = move_ptr(sx, size * i);
            sy_i = move_ptr(sy, size * i);
            
            *cx_i += dt * (*sx_i);
            *cy_i += dt * (*sy_i);

            // TODO: remove this aftet debugging
            #if NB_CALCULATION_DEBUG
            if (i == 0)
            {
                printf("Was the coordinates calculation block parallelized: %s.\n", \
                        omp_in_parallel() ? "true" : "false");
            }
            #endif
        }
    }

    time += dt;
}
