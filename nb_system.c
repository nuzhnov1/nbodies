#include "nb_system.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "nb_calculation.h"


void nb_system_init_default(nb_system *const system)
{   
    system->bodies = (nb_body*)malloc(sizeof(nb_body));
    if (system->bodies == NULL || errno != 0)
    {
        system->capacity = 0;
        return;
    }
    else
        system->capacity = 1;
    
    system->_calc_buf = malloc(sizeof(nb_float) * 2);
    system->count = 0;
    system->time = 0.0;
}

void nb_system_copy(nb_system *const system, const nb_system *const copy)
{
    void* mem_p = malloc(sizeof(nb_body) * copy->capacity);
    if (mem_p == NULL || errno != 0)
        return;
    else
        system->bodies = (nb_body*)mem_p;

    mem_p = malloc(sizeof(nb_float) * copy->capacity * 2);
    if (mem_p == NULL || errno != 0)
        return;
    else
        system->_calc_buf = mem_p;

    for (size_t i = 0; i < copy->count; i++)
        nb_body_copy(&system->bodies[i], &copy->bodies[i]);
    
    system->count = copy->count;
    system->capacity = copy->capacity;
    system->time = copy->time;
}

const nb_system* nb_system_assign(nb_system *const system,
    const nb_system *const copy)
{
    if (system == copy)
        return system;
    
    if (copy->capacity > system->capacity ||
        copy->capacity <= system->capacity / 4) 
    {
        nb_system_destroy(system);

        void* mem_p = malloc(sizeof(nb_body) * copy->capacity);
        if (mem_p == NULL || errno != 0)
            return NULL;
        else
            system->bodies = (nb_body*)mem_p;
        
        mem_p = malloc(sizeof(nb_float) * copy->capacity * 2);
        if (mem_p == NULL || errno != 0)
            return NULL;
        else
            system->_calc_buf = mem_p;

        system->capacity = copy->capacity;
    }

    for (size_t i = 0; i < copy->count; i++)
        nb_body_copy(&system->bodies[i], &copy->bodies[i]);
    
    system->count = copy->count;
    system->time = copy->time;

    return system;
}

void nb_system_destroy(nb_system *const system)
{
    if (system->bodies != NULL && system->_calc_buf != NULL)
    {
        free(system->bodies);
        free(system->_calc_buf);
        system->bodies = NULL;
        system->_calc_buf = NULL;
        system->capacity = 0;
        system->count = 0;
        system->time = 0.0;
    }
}

void nb_system_add_body(nb_system *const system, const nb_body *const body)
{
    size_t last = system->count;
    
    if (system->capacity == 0)
        return;

    if (system->count + 1 > system->capacity)
    {
        size_t new_capacity = system->capacity * 2;

        void* mem_p = malloc(sizeof(nb_body) * new_capacity);
        if (mem_p == NULL || errno != 0)
            return;
        else
        {
            memcpy(mem_p, system->bodies, sizeof(nb_body) * system->count);
            free(system->bodies);
            system->bodies = (nb_body*)mem_p;
        }

        mem_p = malloc(sizeof(nb_float) * new_capacity * 2);
        if (mem_p == NULL || errno != 0)
            return;
        else
        {
            free(system->_calc_buf);
            system->_calc_buf = mem_p;
        }
        
        system->capacity = new_capacity;
    }

    nb_body_copy(&system->bodies[last], body);
    system->count++;
}

void nb_system_remove_body(nb_system *const system, size_t index)
{
    if (index >= system->count)
        return;
    
    for (size_t i = index; i < system->count - 1; i++)
        nb_body_copy(&system->bodies[i], &system->bodies[i + 1]);

    if (system->count - 1 < system->capacity / 4)
    {
        size_t new_capacity = system->capacity / 4;
        
        void* mem_p = malloc(sizeof(nb_body) * new_capacity);
        if (mem_p == NULL || errno != 0)
        {
            system->count--;
            return;
        }
        else
        {
            memcpy(mem_p, system->bodies, 
                sizeof(nb_body) * (system->count - 1));
            free(system->bodies);
            system->bodies = (nb_body*)mem_p;
            system->count--;
        }

        mem_p = malloc(sizeof(nb_float) * new_capacity * 2);
        if (mem_p == NULL || errno != 0)
        {
            system->count--;
            return;
        }
        else
        {
            free(system->_calc_buf);
            system->_calc_buf = mem_p;
        }

        system->capacity = new_capacity;
    }

    system->count--;
}

void nb_system_clear(nb_system *const system)
{
    nb_system_destroy(system);
    return nb_system_init_default(system);
}

void nb_system_run(nb_system *const system, const nb_float dt,
    const bool parralel)
{
    if (parralel)
        nb_euler_multithreading(system, dt);
    else
        nb_euler_singlethread(system, dt);
}

bool nb_system_read(nb_system *const system, FILE* stream)
{
    bool is_read = true;
    nb_body body;
    size_t count;
    nb_float time;

    is_read &= fread(&count, sizeof(size_t), 1, stream) == 1;
    is_read &= fread(&time, sizeof(nb_float), 1, stream) == 1;

    if (!is_read)
        return false;

    system->count = 0;  // better rewrite old data then reallocate memory

    for (size_t i = 0; i < count; i++)
    {
        is_read &= nb_body_read(&body, stream);

        if (!is_read)
            break;
        
        nb_system_add_body(system, &body);
        if (errno != 0)
        {
            is_read = false;
            break;
        }
    }

    size_t new_capacity = system->capacity;
    while (system->count < new_capacity / 4)
        new_capacity = new_capacity / 4;
    
    if (new_capacity != system->capacity)
    {
        void* mem_p = malloc(sizeof(nb_body) * new_capacity);
        if (mem_p == NULL || errno != 0)
            return is_read;
        else
        {
            memcpy(mem_p, system->bodies, sizeof(nb_body) * system->count);
            free(system->bodies);
            system->bodies = (nb_body*)mem_p;
        }

        mem_p = malloc(sizeof(nb_float) * new_capacity * 2);
        if (mem_p == NULL || errno != 0)
            return is_read;
        else
        {
            free(system->_calc_buf);
            system->_calc_buf = mem_p;
        }

        system->capacity = new_capacity;
    }

    return is_read;
}

bool nb_system_write(const nb_system *const system, FILE* stream)
{
    bool is_write = true;

    is_write &= fwrite(&system->count, sizeof(size_t), 1, stream) == 1;
    is_write &= fwrite(&system->time, sizeof(nb_float), 1, stream) == 1;

    for (size_t i = 0; i < system->count; i++)
    {
        is_write &= nb_body_write(&system->bodies[i], stream);

        if (!is_write)
            break;
    }

    return is_write;
}

bool nb_system_print(const nb_system *const system, FILE* stream)
{
    bool is_print = true;

    is_print &= fprintf(stream, "System information:\n") > 0;
    is_print &= fprintf(stream, "Count of bodies: %lu\n", system->count) > 0;
    #if NB_SYSTEM_DEBUG
    is_print &= fprintf(stream, "System capacity: %lu\n",
        system->capacity) > 0;
    #endif
    is_print &= fprintf(stream, "Time: %lf\n\n", system->time) > 0;

    for (size_t i = 0; i < system->count; i++)
    {
        is_print &= fprintf(stream, "Body index: %lu\n", i) > 0;
        is_print &= nb_body_print(&system->bodies[i], stream);
        is_print &= fprintf(stream, "\n") > 0;

        if (!is_print)
            break;
    }

    return is_print;
}
