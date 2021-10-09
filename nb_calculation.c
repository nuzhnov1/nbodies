#include "nb_calculation.h"

#include <math.h>

#include <omp.h>


// move "ptr" by "count" bytes to the right
#define move_ptr(ptr, count) ((void*)(ptr) + (count))


const nb_float gravity_const = 6.6743015e-11;


void nb_euler_singlethread(nb_system *const system, nb_float dt)
{
    const size_t offset = sizeof(nb_body); // offset to pointers
    nb_body* bodies = system->bodies;      // vector of bodies
    size_t count = system->count;          // count of bodies

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
    nb_float *const rad = &bodies[0].radius;   // radius

    // values of speed after probably collisions
    nb_float* const sx_new = (nb_float*)system->_calc_buf;
    nb_float* const sy_new = (nb_float*)system->_calc_buf + count;

    // components of "i" body
    nb_float* cx_i;
    nb_float* cy_i;
    nb_float* sx_i;
    nb_float* sy_i;
    nb_float* fx_i;
    nb_float* fy_i;
    nb_float* mass_i;
    nb_float* rad_i;

    // components of "j" body
    nb_float* cx_j; 
    nb_float* cy_j;
    nb_float* sx_j;
    nb_float* sy_j;
    nb_float* fx_j;
    nb_float* fy_j;
    nb_float* mass_j;
    nb_float* rad_j;

    // Initializing all total forces for all bodies to 0
    fx_i = fx;
    fy_i = fy;
    for (size_t i = 0; i < count; i++)
    {
        *fx_i = 0;
        *fy_i = 0; 

        fx_i = move_ptr(fx_i, offset);
        fy_i = move_ptr(fy_i, offset);
    }

    // Calculate forces for all bodies in system and check probably collisions
    cx_i = cx;
    cy_i = cy;
    sx_i = sx;
    sy_i = sy;
    fx_i = fx;
    fy_i = fy;
    mass_i = mass;
    rad_i = rad;
    for (size_t i = 0; i < count; i++)
    {
        // total mass of other bodies, which colided with body "i"
        nb_float t_mass = 0.0;
        // total impulse of other bodies, which colided with body "j"
        nb_float t_impulse_x = 0.0;
        nb_float t_impulse_y = 0.0;
        
        // did the body "i" collide with anyone body
        bool is_collided = false;

        cx_j = cx; 
        cy_j = cy;
        sx_j = sx;
        sy_j = sy;
        fx_j = fx;
        fy_j = fy;
        mass_j = mass;
        rad_j = rad;

        // Calculate forces between body "i" and all bodies "j", when j != i
        for (size_t j = 0; j < count; j++)
        {
            if (j == i)
            {
                cx_j = move_ptr(cx_j, offset);
                cy_j = move_ptr(cy_j, offset);
                sx_j = move_ptr(sx_j, offset);
                sy_j = move_ptr(sy_j, offset);
                fx_j = move_ptr(fx_j, offset);
                fy_j = move_ptr(fy_j, offset);
                mass_j = move_ptr(mass_j, offset);
                rad_j = move_ptr(rad_j, offset);

                continue;
            }

            nb_float dx = (*cx_j) - (*cx_i);
            nb_float dy = (*cy_j) - (*cy_i);

            // distance between bodies "i" and "j"
            nb_float distance = (nb_float)sqrtl(dx * dx + dy * dy);
            
            // if the bodies collided
            if (distance <= (*rad_i) + (*rad_j))
            { 
                is_collided |= true;
                t_mass += (*mass_j);
                t_impulse_x += (*sx_j) * (*mass_j);
                t_impulse_y += (*sy_j) * (*mass_j);
            }

            // (dx * dx + dy * dy) ^ (3 / 2)
            nb_float temp = distance * distance * distance;            

            // scalar part of force
            nb_float scalar = gravity_const * (*mass_i) * (*mass_j) / temp;

            // components of force
            nb_float fx_c = dx * scalar;
            nb_float fy_c = dy * scalar;

            // The force acting on the body "i" relative to the body "j"
            *fx_i += fx_c;
            *fy_i += fy_c;

            cx_j = move_ptr(cx_j, offset);
            cy_j = move_ptr(cy_j, offset);
            sx_j = move_ptr(sx_j, offset);
            sy_j = move_ptr(sy_j, offset);
            fx_j = move_ptr(fx_j, offset);
            fy_j = move_ptr(fy_j, offset);
            mass_j = move_ptr(mass_j, offset);
            rad_j = move_ptr(rad_j, offset);
        }

        // Calculate speed for body "i" after collisions
        if (is_collided)
        {
            nb_float scal1 = ((*mass_i) - t_mass) / ((*mass_i) + t_mass);
            nb_float scal2 = 2.0 / ((*mass_i) + t_mass);

            sx_new[i] = scal1 * (*sx_i) + scal2 * t_impulse_x;
            sy_new[i] = scal1 * (*sy_i) + scal2 * t_impulse_y;
        }
        // Or record current speed values if there no collisions
        else
        {
            sx_new[i] = (*sx_i);
            sy_new[i] = (*sy_i);
        }

        cx_i = move_ptr(cx_i, offset);
        cy_i = move_ptr(cy_i, offset);
        sx_i = move_ptr(sx_i, offset);
        sy_i = move_ptr(sy_i, offset);
        fx_i = move_ptr(fx_i, offset);
        fy_i = move_ptr(fy_i, offset);
        mass_i = move_ptr(mass_i, offset);
        rad_i = move_ptr(rad_i, offset);
    }

    // Calculate new speed and new coordinates for all bodies
    cx_i = cx;
    cy_i = cy;
    sx_i = sx;
    sy_i = sy;
    fx_i = fx;
    fy_i = fy;
    mass_i = mass;
    for (size_t i = 0; i < count; i++)
    {
        // Set new speed after probably collision  
        *sx_i = sx_new[i];
        *sy_i = sy_new[i];

        // Values of speed at current time moment
        nb_float prev_sx_i = *sx_i;
        nb_float prev_sy_i = *sy_i;

        // Calculate new speed for body "i" through time "dt"
        *sx_i += dt * (*fx_i) / (*mass_i);
        *sy_i += dt * (*fy_i) / (*mass_i);

        // Calculate new coordinates for body "i" 
        *cx_i += dt * prev_sx_i + ((*sx_i) - prev_sx_i) * dt / 2;
        *cy_i += dt * prev_sy_i + ((*sy_i) - prev_sy_i) * dt / 2;

        cx_i = move_ptr(cx_i, offset);
        cy_i = move_ptr(cy_i, offset);
        sx_i = move_ptr(sx_i, offset);
        sy_i = move_ptr(sy_i, offset);
        fx_i = move_ptr(fx_i, offset);
        fy_i = move_ptr(fy_i, offset);
        mass_i = move_ptr(mass_i, offset);
    }
    
    system->time += dt;
}

void nb_euler_multithreading(nb_system *const system, nb_float dt)
{
    const size_t max_threads = (size_t)omp_get_max_threads();

    const size_t offset = sizeof(nb_body); // offset to pointers
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
    nb_float *const rad = &bodies[0].radius;   // radius

    // components of "i" body
    nb_float* cx_i;
    nb_float* cy_i;
    nb_float* sx_i;
    nb_float* sy_i;
    nb_float* fx_i;
    nb_float* fy_i;
    nb_float* mass_i;
    nb_float* rad_i;

    // components of "j" body
    nb_float* cx_j; 
    nb_float* cy_j;
    nb_float* sx_j;
    nb_float* sy_j;
    nb_float* fx_j;
    nb_float* fy_j;
    nb_float* mass_j;
    nb_float* rad_j;

    // TODO: specify the number in clause if
    #pragma omp parallel private(cx_i, cy_i, sx_i, sy_i, fx_i, fy_i, mass_i, \
                                 rad_i, cx_j, cy_j, sx_j, sy_j, fx_j, fy_j, \
                                 mass_j, rad_j) \
                         shared(count, offset, gravity_const, dt, cx, cy, \
                                sx, sy, fx, fy, mass, rad) \
                         if (count > max_threads * 10)
    {
        const size_t threads_count = (size_t)omp_get_num_threads();

        // Initializing all total forces for all bodies to 0
        #pragma omp for schedule(static, count / threads_count)
        for (size_t i = 0; i < count; i++)
        {
            fx_i = move_ptr(fx, offset * i);
            fy_i = move_ptr(fy, offset * i);
            
            *fx_i = 0;
            *fy_i = 0;

            // TODO: remove this aftet debugging
            #ifdef NB_CALCULATION_DEBUG
            if (i == 0)
            {
                printf("Was the force initialization block parallelized: "
                    "%s.\n", omp_in_parallel() ? "true" : "false");
            }
            #endif
        }
        
        // Calculate forces for all bodies in system
        #pragma omp for schedule(static, count / threads_count)
        // Calculate parameters for all bodies in system
        for (size_t i = 0; i < count; i++)
        {
            // total mass of other bodies, which colided with body "i"
            nb_float t_mass = 0.0;
            // total impulse of other bodies, which colided with body "j"
            nb_float t_impulse_x = 0.0;
            nb_float t_impulse_y = 0.0;
            
            // did the body "i" collide with anyone body
            bool is_collided = false;

            // Initialize body "i" parameters
            cx_i = move_ptr(cx, offset * i);
            cy_i = move_ptr(cy, offset * i);
            sx_i = move_ptr(sx, offset * i);
            sy_i = move_ptr(sy, offset * i);
            fx_i = move_ptr(fx, offset * i);
            fy_i = move_ptr(fy, offset * i);
            mass_i = move_ptr(mass, offset * i);
            rad_i = move_ptr(rad, offset * i);

            // Initialize body "j" parameters
            cx_j = cx; 
            cy_j = cy;
            sx_j = sx;
            sy_j = sy;
            fx_j = fx;
            fy_j = fy;
            mass_j = mass;
            rad_j = rad;

            // Calculate forces between body "i" and all bodies "j",
            // when j != i. Controlling collisions
            for (size_t j = 0; j < count; j++)
            {
                if (j == i)
                {
                    cx_j = move_ptr(cx_j, offset);
                    cy_j = move_ptr(cy_j, offset);
                    sx_j = move_ptr(sx_j, offset);
                    sy_j = move_ptr(sy_j, offset);
                    fx_j = move_ptr(fx_j, offset);
                    fy_j = move_ptr(fy_j, offset);
                    mass_j = move_ptr(mass_j, offset);
                    rad_j = move_ptr(rad_j, offset);

                    continue;
                }

                nb_float dx = (*cx_j) - (*cx_i);
                nb_float dy = (*cy_j) - (*cy_i);

                // distance between bodies "i" and "j"
                nb_float distance = (nb_float)sqrtl(dx * dx + dy * dy);
                
                // if the bodies collided
                if (distance <= (*rad_i) + (*rad_j))
                { 
                    is_collided |= true;
                    t_mass += (*mass_j);
                    t_impulse_x += (*sx_j) * (*mass_j);
                    t_impulse_y += (*sy_j) * (*mass_j);
                }

                // (dx * dx + dy * dy) ^ (3 / 2)
                nb_float temp = distance * distance * distance;            

                // scalar part of force
                nb_float scalar = gravity_const * (*mass_i) * (*mass_j) / temp;

                // components of force
                nb_float fx_c = dx * scalar;
                nb_float fy_c = dy * scalar;

                // The force acting on the body "i" relative to the body "j"
                *fx_i += fx_c;
                *fy_i += fy_c;

                cx_j = move_ptr(cx_j, offset);
                cy_j = move_ptr(cy_j, offset);
                sx_j = move_ptr(sx_j, offset);
                sy_j = move_ptr(sy_j, offset);
                fx_j = move_ptr(fx_j, offset);
                fy_j = move_ptr(fy_j, offset);
                mass_j = move_ptr(mass_j, offset);
                rad_j = move_ptr(rad_j, offset);
            }

            // Values of speed at current time moment
            nb_float prev_sx_i = *sx_i;
            nb_float prev_sy_i = *sy_i;

            // Calculate speed for body "i" after collisions
            if (is_collided)
            {
                nb_float scal1 = ((*mass_i) - t_mass) / ((*mass_i) + t_mass);
                nb_float scal2 = 2.0 / ((*mass_i) + t_mass);

                *sx_i = scal1 * prev_sx_i + scal2 * t_impulse_x;
                *sy_i = scal1 * prev_sy_i + scal2 * t_impulse_y;
            }

            // Calculate speed for body "i" through time "dt"
            *sx_i += dt * (*fx_i) / (*mass_i);
            *sy_i += dt * (*fy_i) / (*mass_i);

            // Calculate new coordinates for body "i" 
            *cx_i += dt * prev_sx_i + ((*sx_i) - prev_sx_i) * dt / 2;
            *cy_i += dt * prev_sy_i + ((*sy_i) - prev_sy_i) * dt / 2;

            cx_i = move_ptr(cx_i, offset);
            cy_i = move_ptr(cy_i, offset);
            sx_i = move_ptr(sx_i, offset);
            sy_i = move_ptr(sy_i, offset);
            fx_i = move_ptr(fx_i, offset);
            fy_i = move_ptr(fy_i, offset);
            mass_i = move_ptr(mass_i, offset);
            rad_i = move_ptr(rad_i, offset);

            // TODO: remove this aftet debugging
            #ifdef NB_CALCULATION_DEBUG
            if (i == 0)
            {
                printf("Was the force calculation block parallelized: %s.\n", \
                    omp_in_parallel() ? "true" : "false");
            }
            #endif
        }
    }

    time += dt;
}
