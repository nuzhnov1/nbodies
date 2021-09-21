#include "nb_system.h"
#include "nb_calculation.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>


bool nb_system_init_default(nb_system *const system) {
    system->count = 0;
    system->capacity = 1;
    system->time = 0.0;
    
    void* mem_p = malloc(sizeof(nb_body));
    
    if (mem_p == NULL) {
        system->bodies = NULL;
        system->capacity = 0;
        
        return false;
    }
    else {
        system->bodies = (nb_body*)mem_p;
        return true;
    }
}

bool nb_system_copy(nb_system *const system, const nb_system *const copy) {
    void* mem_p = malloc(sizeof(nb_body) * copy->capacity);

    if (mem_p == NULL)
        return false;
    else
        system->bodies = (nb_body*)mem_p;

    for (size_t i = 0; i < copy->count; i++)
        nb_body_copy(&system->bodies[i], &copy->bodies[i]);
    
    system->count = copy->count;
    system->capacity = copy->capacity;
    system->time = copy->time;

    return true;
}

const nb_system* nb_system_assign(nb_system *const system, const nb_system *const copy) {
    if (system == copy)
        return system;
    
    if (copy->capacity > system->capacity || copy->capacity <= system->capacity / 4) {
        nb_system_destroy(system);

        void* mem_p = malloc(sizeof(nb_body) * copy->capacity);

        if (mem_p == NULL)
            return NULL;

        system->bodies = (nb_body*)mem_p;
        system->capacity = copy->capacity;
    }

    for (size_t i = 0; i < copy->count; i++)
        nb_body_copy(&system->bodies[i], &copy->bodies[i]);
    
    system->count = copy->count;
    system->time = copy->time;

    return system;
}

void nb_system_destroy(nb_system *const system) {
    if (system->bodies != NULL) {
        free((void*)system->bodies);
        system->bodies = NULL;
        system->capacity = 0;
        system->count = 0;
        system->time = 0.0;
    }
}

bool nb_system_add_body(nb_system *const system, const nb_body *const body) {
    size_t last = system->count;
    
    if (system->capacity == 0)
        return false;

    if (system->count + 1 > system->capacity) {
        size_t new_capacity = system->capacity * 2;
        void* mem_p = malloc(sizeof(nb_body) * new_capacity);

        if (mem_p == NULL)
            return false;
        
        memcpy(mem_p, (const void*)system->bodies, sizeof(nb_body) * system->count);
        free((void*)system->bodies);
        system->bodies = (nb_body*)mem_p;
        system->capacity = new_capacity;
    }

    nb_body_copy(&system->bodies[last], body);
    system->count++;

    return true;
}

bool nb_system_remove_body(nb_system *const system, const size_t index) {
    if (index >= system->count)
        return false;
    
    for (size_t i = index; i < system->count - 1; i++) {
        nb_body_copy(&system->bodies[i], &system->bodies[i + 1]);
    }

    if (system->count - 1 < system->capacity / 4) {
        size_t new_capacity = system->capacity / 4;
        void* mem_p = malloc(sizeof(nb_body) * new_capacity);

        if (mem_p == NULL) {
            system->count--;
            return true;
        }

        memcpy(mem_p, (const void*)system->bodies, sizeof(nb_body) * (system->count - 1));
        free((void*)system->bodies);
        system->bodies = (nb_body*)mem_p;
        system->capacity = new_capacity;
    }

    system->count--;

    return true;
}

bool nb_system_clear(nb_system *const system) {
    nb_system_destroy(system);
    return nb_system_init_default(system);
}

void nb_system_run(nb_system *const system, const nb_float dt, const bool parralel) {
    if (parralel)
        nb_euler_multithreading(system, dt);
    else
        nb_euler_singlethread(system, dt);
}

bool nb_system_read(nb_system *const system, FILE* stream) {
    bool is_read = true;
    nb_body body;
    size_t count;
    nb_float time;

    is_read &= (bool)(fread((void*)&count, sizeof(size_t), 1, stream) == 1);
    is_read &= (bool)(fread((void*)&time, sizeof(nb_float), 1, stream) == 1);

    if (!is_read)
        return false;

    system->count = 0;  // better rewrite old data then reallocate memory

    for (size_t i = 0; i < count; i++) {
        is_read &= nb_body_read(&body, stream);

        if (!is_read)
            break;
        
        if (!nb_system_add_body(system, &body)) {
            is_read = false;
            break;
        }
    }

    size_t new_capacity = system->capacity;
    while (system->count < new_capacity / 4)
        new_capacity = new_capacity / 4;
    
    if (new_capacity != system->capacity) {
        void* mem_p = malloc(sizeof(nb_body) * new_capacity);

        if (mem_p == NULL) {
            return is_read;
        }

        memcpy(mem_p, (const void*)system->bodies, sizeof(nb_body) * system->count);
        free((void*)system->bodies);
        system->bodies = (nb_body*)mem_p;
        system->capacity = new_capacity;
    }

    return is_read;
}

bool nb_system_write(const nb_system *const system, FILE* stream) {
    bool is_write = true;

    is_write &= (bool)(fwrite((const void*)&system->count, sizeof(size_t), 1, stream) == 1);
    is_write &= (bool)(fwrite((const void*)&system->time, sizeof(nb_float), 1, stream) == 1);

    for (size_t i = 0; i < system->count; i++) {
        is_write &= nb_body_write(&system->bodies[i], stream);

        if (!is_write)
            break;
    }

    return is_write;
}

bool nb_system_print(const nb_system *const system, FILE* stream) {
    bool is_print = true;

    is_print &= (bool)(fprintf(stream, "System information:\n") > 0);
    is_print &= (bool)(fprintf(stream, "Count of bodies: %lu\n", system->count) > 0);
    #ifdef NB_SYSTEM_DEBUG
    is_print &= (bool)(fprintf(stream, "System capacity: %lu\n", system->capacity) > 0);
    #endif
    is_print &= (bool)(fprintf(stream, "Time: %lf\n\n", system->time) > 0);

    for (size_t i = 0; i < system->count; i++) {
        is_print &= (bool)(fprintf(stream, "Body index: %lu\n", i) > 0);
        is_print &= nb_body_print(&system->bodies[i], stream);
        is_print &= (bool)(fprintf(stream, "\n") > 0);

        if (!is_print)
            break;
    }

    return is_print;
}
