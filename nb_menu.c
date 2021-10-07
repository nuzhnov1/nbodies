#include "nb_menu.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <omp.h>


static void _nb_menu_print();
static void _nb_menu_settings_loop(nb_rand_settings *const settings);
static void _nb_menu_print_settings(const nb_rand_settings *const settings);
static void _nb_menu_compare_systems(const nb_system *const system1,
    const nb_system *const system2);
static void _nb_menu_add_body(nb_system *const system);
static void _nb_menu_remove_body(nb_system *const system);
static nb_int _nb_menu_input_int();
static nb_uint _nb_menu_input_uint();
static nb_float _nb_menu_input_float();
static char* _nb_menu_input_str(char* const buffer, size_t buffer_size);
static void _nb_menu_input_vector(nb_vector2 *const vector);
static void _nb_menu_input_body(nb_body *const body);


const nb_rand_settings default_settings = 
{
    -100.0, 100.0,
    -10.0, 10.0,
    1.e5, 1.e6
};


void nb_menu_loop(nb_system *const system)
{
    nb_rand_settings settings = default_settings;
    bool is_exit = false;
    nb_uint choose;

    while (!is_exit)
    {
        _nb_menu_print();

        choose = _nb_menu_input_uint();

        switch (choose)
        {
        case 1: 
        {
            nb_uint count;

            printf("Enter the count of bodies in system:\n");
            while (true)
            {
                count = _nb_menu_input_uint();

                if (count > NB_MAX_BODIES)
                {
                    printf("Error: the limit on the number of bodies in the"
                        " system has been exceeded.\n");
                }
                else
                    break;
            }

            nb_menu_rand(system, count, &settings);

            break;
        }
        case 2:
        {
            _nb_menu_settings_loop(&settings);
            break;
        }
        case 3:
        {
            if (system->count + 1 > NB_MAX_BODIES)
            {
                printf("Error: the limit on the number of bodies in the system"
                    " has been exceeded.\n");
            }
            else
                _nb_menu_add_body(system);
            
            break;
        }
        case 4:
        {
            _nb_menu_remove_body(system);
            break;
        }
        case 5:
        {
            nb_float end_time, dt;
            nb_uint choose;
            nb_menu_run_t run = {false, false};

            printf("Enter the end time of modeling:\n");
            while (true)
            {
                end_time = _nb_menu_input_float();

                if (end_time <= 0.0)
                    printf("Error: end time must be greater than zero.\n");
                else
                    break;
            }

            printf("Enter a delta of time:\n");
            while (true)
            {
                dt = _nb_menu_input_float();

                if (dt <= 0.0)
                    printf("Error: delta of time must be greater than zero.\n");
                else
                    break;
            }

            printf("Would you like to use calculations using OpenMP "
                "technology, sequential calculations, or both types "
                "at once?\n");
            printf("\t1: Only sequential calculation.\n");
            printf("\t2: Only OpenMP technology.\n");
            printf("\t3: Both types of calculation.\n");

            choose = _nb_menu_input_uint();

            switch (choose)
            {
            case 1:
            {
                run.seq = true;
                break;
            }
            case 2:
            {
                run.openmp = true;
                break;
            }
            case 3:
            {
                run.seq = true;
                run.openmp = true;
                break;
            }
            default:
            {
                printf("Error: this menu item does not exist.\n");
                break;
            }
            }

            nb_menu_run_system(system, end_time, dt, run);

            break;
        }
        case 6:
        {
            char filename[NB_PATH_MAX];

            printf("Enter the path to file with extension\".nb\":\n");
            _nb_menu_input_str(filename, NB_PATH_MAX);

            nb_menu_load_system(system, filename);

            break;
        }
        case 7:
        {
            char filename[NB_PATH_MAX];

            printf("Enter the path to file with extension\".nb\":\n");
            _nb_menu_input_str(filename, NB_PATH_MAX);

            nb_menu_save_system(system, filename);

            break;
        }
        case 8:
        {
            nb_menu_print_system(system, stdout);
            break;
        }
        case 9:
        {
            FILE* file;
            char filename[NB_PATH_MAX];

            printf("Enter the path to file:\n");
            _nb_menu_input_str(filename, NB_PATH_MAX);

            file = fopen(filename, "wt");

            if (file == NULL)
                printf("Error: failed to create this file.\n");
            else
                nb_menu_print_system(system, file);

            fclose(file);

            break;
        }
        case 10:
        {
            printf("Exiting the program...\n");
            is_exit = true;

            break;
        }
        default:
        {
            printf("Error: this menu item does not exist.\n");
            break;
        }
        }

        printf("\n");
    }

    printf("The program has completed its work.\n");
}

void nb_menu_rand(nb_system *const system, nb_uint count,
    const nb_rand_settings *const settings) 
{
    char name[NB_NAME_MAX];
    nb_body body;
    bool success = true;

    name[NB_NAME_MAX - 1] = '\0';

    if (!nb_system_clear(system))
    {
        printf("Critical error: failed to allocate memory. "
            "System was destroyed!\n");
        exit(EXIT_FAILURE);
    }

    printf("Initializing system of random bodies...\n");
    for (size_t i = 0; i < count; i++)
    {
        snprintf(name, NB_NAME_MAX - 1, "Body %lu", i + 1);
        nb_rand_body(&body, name, settings);
        if (!nb_system_add_body(system, &body)) {
            printf("Error: failed to add body in system.");
            if (errno == ENOMEM)
                printf(" There is not enough memory to create a body.\n");
            else
                printf("\n");
            
            success = false;
            break;
        }
    }

    if (!success)
        printf("Failed to initializing system.\n");
    else
        printf("System was successfully initialized.\n"); 
}

void nb_menu_run_system(nb_system *const system, nb_float end_time,
    nb_float dt, nb_menu_run_t run)
{
    double timework;

    if (run.seq && !run.openmp)
    {
        clock_t start, finish;

        printf("The system is being modeled in sequential mode...\n");

        start = clock();
        for (nb_float cur = 0.0; cur < end_time; cur += dt)
            nb_system_run(system, dt, false);
        finish = clock();
        timework = (finish - start) / (double)CLOCKS_PER_SEC;
        printf("The simulation of the system is completed.\n");
        printf("Simulation time: %.3f sec.\n", timework);
    }
    else if (!run.seq && run.openmp)
    {
        double start, finish;

        printf("The system is being modeled in parallel mode...\n");

        start = omp_get_wtime();
        for (nb_float cur = 0.0; cur < end_time; cur += dt)
            nb_system_run(system, dt, true);
        finish = omp_get_wtime();
        timework = finish - start;
        printf("The simulation of the system is completed.\n");
        printf("Simulation time: %.3f sec.\n", timework);
    }
    else if (run.seq && run.openmp)
    {
        nb_system copy;
        clock_t seq_start, seq_finish;
        double par_start, par_finish;

        if (!nb_system_copy(&copy, system))
        {
            printf("Error: failed to create copy of system.\n");
            return;
        }

        printf("The system is being modeled in sequential mode...\n");
        seq_start = clock();
        for (nb_float cur = 0.0; cur < end_time; cur += dt)
            nb_system_run(system, dt, false);
        seq_finish = clock();
        timework = (seq_finish - seq_start) / (double)CLOCKS_PER_SEC;
        printf("The simulation of the system is completed.\n");
        printf("Simulation time: %.3f sec.\n", timework);
        
        printf("The system is being modeled in parallel mode...\n");
        par_start = omp_get_wtime();
        for (nb_float cur = 0.0; cur < end_time; cur += dt)
            nb_system_run(&copy, dt, true);
        par_finish = omp_get_wtime();
        timework = par_finish - par_start;
        printf("The simulation of the system is completed.\n");
        printf("Simulation time: %.3f sec.\n", timework);

        _nb_menu_compare_systems(system, &copy);
    }
}

void _nb_menu_compare_systems(const nb_system *const system1,
    const nb_system *const system2)
{
    // Average values of:
    nb_vector2 ae_coords;  // absolute error of coordinates
    nb_vector2 re_coords;  // relative error of coordinates
    nb_vector2 ae_speed;   // absolute error of speed
    nb_vector2 re_speed;   // relative error of speed
    nb_vector2 ae_force;   // absolute error of force
    nb_vector2 re_force;   // relative error of force
    
    nb_vector2_init_default(&ae_coords);
    nb_vector2_init_default(&re_coords);
    nb_vector2_init_default(&ae_speed);
    nb_vector2_init_default(&re_speed);
    nb_vector2_init_default(&ae_force);
    nb_vector2_init_default(&re_force);

    nb_body* body1, *body2;
    nb_vector2 vec;

    printf("Comparison system 2 relative to system1:\n");
    printf("Number of bodies in system: %lu\n", system1->count);
    printf("System time: %lf sec.\n", system1->time);

    if (system1->count == 0)
        return;

    for (size_t i = 0; i < system1->count; i++)
    {
        body1 = &system1->bodies[i];
        body2 = &system2->bodies[i];

        vec = nb_vector2_sub(&body1->coords, &body2->coords);
        vec.x = fabsl(vec.x), vec.y = fabsl(vec.y);
        ae_coords = nb_vector2_add(&ae_coords, &vec);
        vec.x = vec.x / fabsl(body1->coords.x) * 100;
        vec.y = vec.y / fabsl(body1->coords.y) * 100;
        re_coords = nb_vector2_add(&re_coords, &vec);

        vec = nb_vector2_sub(&body1->speed, &body2->speed);
        vec.x = fabsl(vec.x), vec.y = fabsl(vec.y);
        ae_speed = nb_vector2_add(&ae_speed, &vec);
        vec.x = vec.x / fabsl(body1->speed.x) * 100;
        vec.y = vec.y / fabsl(body1->speed.y) * 100;
        re_speed = nb_vector2_add(&re_speed, &vec);

        vec = nb_vector2_sub(&body1->force, &body2->force);
        vec.x = fabsl(vec.x), vec.y = fabsl(vec.y);
        ae_force = nb_vector2_add(&ae_force, &vec);
        vec.x = vec.x / fabsl(body1->force.x) * 100;
        vec.y = vec.y / fabsl(body1->force.y) * 100;
        re_force = nb_vector2_add(&re_force, &vec);
    }

    nb_vector2_mul(&ae_coords, 1 / system1->count);
    nb_vector2_mul(&re_coords, 1 / system1->count);
    nb_vector2_mul(&ae_speed, 1 / system1->count);
    nb_vector2_mul(&re_speed, 1 / system1->count);
    nb_vector2_mul(&ae_force, 1 / system1->count);
    nb_vector2_mul(&re_force, 1 / system1->count);

    printf("Average absolute error of coordinates: ");
    nb_vector2_print(&ae_coords, stdout);
    printf("\n");
    printf("Average relative error of coordinates(%%): ");
    nb_vector2_print(&re_coords, stdout);
    printf("\n");
    printf("Average absolute error of speed: ");
    nb_vector2_print(&ae_speed, stdout);
    printf("\n");
    printf("Average relative error of speed(%%): ");
    nb_vector2_print(&re_speed, stdout);
    printf("\n");
    printf("Average absolute error of force: ");
    nb_vector2_print(&ae_force, stdout);
    printf("\n");
    printf("Average relative error of force(%%): ");
    nb_vector2_print(&re_force, stdout);
    printf("\n");
}

void nb_menu_load_system(nb_system *const system, const char *const filename)
{
    FILE* file;

    file = fopen(filename, "rb");

    if (file == NULL) {
        printf("Error: failed to open this file.\n");
        return;
    }

    printf("Reading the data from this file...\n");
    if (!nb_system_read(system, file))
    {
        printf("Error: failed to read data from this file.\n");
        nb_system_init_default(system);
    }
    else
        printf("The data was successfully read from this file.\n");
    
    fclose(file);
}

void nb_menu_save_system(const nb_system *const system,
    const char *const filename)
{
    FILE* file;

    file = fopen(filename, "wb");

    if (file == NULL)
    {
        printf("Error: failed to create this file.\n");
        return;
    }

    printf("Writing the data to this file...\n");
    if (!nb_system_write(system, file))
        printf("Error: failed to write data to this file.\n");
    else
        printf("The data was successfully written to this file.\n");
    
    fclose(file);
}

void nb_menu_print_system(const nb_system *const system, FILE* stream)
{
    if (!nb_system_print(system, stream))
        printf("Error: failed to print system.\n");
    else if (stream != stdout)
        printf("The system was successfully printed to this file.\n");
}

void _nb_menu_print()
{
    printf("Menu:\n");
    printf("\t1: Generate a random system of \"n\" bodies.\n");
    printf("\t2: Set parameters for random generation of the system.\n");
    printf("\t3: Add new body in system.\n");
    printf("\t4: Remove body from system by index.\n");
    printf("\t5: Run the system before time \"T\" with step \"dt\".\n");
    printf("\t6: Load system from file with \".nb\" file extension.\n");
    printf("\t7: Save system to file with \".nb\" file extension.\n");
    printf("\t8: Print system to screen.\n");
    printf("\t9: Print system to file.\n");
    printf("\t10: Exit.\n");
}

void _nb_menu_settings_loop(nb_rand_settings *const settings)
{
    bool is_exit = false;
    nb_uint choose;

    while (!is_exit)
    {
        nb_float min, max;

        _nb_menu_print_settings(settings);

        choose = _nb_menu_input_uint();

        if (choose >= 1 && choose <= 3)
        {
            while (true)
            {
                printf("Enter the minimum generator value:\n");
                min = _nb_menu_input_float();

                printf("Enter the maximum generator value:\n");
                max = _nb_menu_input_float();

                if (min > max)
                {
                    printf("Error: minimum value greater then maximum "
                        "value.\n");
                }
                else
                    break;
            }
        }

        switch (choose)
        {
        case 1:
        {
            settings->min_coord = min;
            settings->max_coord = max;

            break;
        }
        case 2:
        {
            settings->min_speed = min;
            settings->max_speed = max;

            break;
        }
        case 3:
        {
            if (min <= 0.0)
            {
                printf("Error: minimum value of mass must be greater "
                    "than zero.\n");
            }
            else
            {
                settings->min_mass = min;
                settings->max_mass = max;
            }

            break;
        }
        case 4:
        {
            printf("Coordinates range: [%lf, %lf).\n",
                settings->min_coord, settings->max_coord);
            printf("Speed range: [%lf, %lf).\n",
                settings->min_speed, settings->max_speed);
            printf("Mass range: [%lf, %lf).\n",
                settings->min_mass, settings->max_mass);

            break;
        }
        case 5:
        {
            is_exit = true;
            break;
        }
        default:
        {
            printf("Error: this menu item does not exist.\n");
            break;
        }
        }

        printf("\n");
    }
}

void _nb_menu_print_settings(const nb_rand_settings *const settings)
{
    printf("Settings of random generation:\n");
    printf("\t1: Set min and max generator values for coodinates.\n");
    printf("\t2: Set min and max generator values for speed.\n");
    printf("\t3: Set min and max generator values for mass.\n");
    printf("\t4: Print settings.\n");
    printf("\t5: Exit.\n");
}

void _nb_menu_add_body(nb_system *const system)
{
    nb_body body;

    printf("Enter a body information:\n");
    _nb_menu_input_body(&body);

    if (!nb_system_add_body(system, &body))
    {
        printf("Error: failed to add body in system.");
        if (errno == ENOMEM)
            printf(" There is not enough memory to create a body.\n");
        else
            printf("\n");
    }
    else
        printf("Body was successfully added to the system.\n");
}

void _nb_menu_remove_body(nb_system *const system)
{
    size_t index;

    printf("Enter the index of removing body:\n");
    index = _nb_menu_input_uint();

    if (!nb_system_remove_body(system, index))
        printf("Error: failed to remove body from system at that index.\n");
    else
    {
        printf("The body at that index was successfully removed from the "
            "system.\n");
    }
}

nb_int _nb_menu_input_int()
{
    nb_int val;
    char buffer[NB_NAME_MAX];
    char* endptr;
    
    buffer[NB_NAME_MAX - 1] = '\0';

    while (true) {
        _nb_menu_input_str(buffer, NB_NAME_MAX);
        errno = 0;
        
        val = (nb_int)strtol(buffer, &endptr, 0);

        if (errno == ERANGE)
            printf("Error: the number is out of the allowed range.\n");
        else if ((errno == EINVAL && val == 0) || (*endptr != '\0'))
        {
            printf("Error: failed to convertion input string to integer "
                "number.\n");
        }
        else
            break;
    }

    return val;
}

nb_uint _nb_menu_input_uint()
{
    nb_uint val;
    char buffer[NB_NAME_MAX];
    char* endptr = buffer;
    
    buffer[NB_NAME_MAX - 1] = '\0';

    while (true)
    {
        _nb_menu_input_str(buffer, NB_NAME_MAX);
        errno = 0;

        val = (nb_uint)strtoul(buffer, &endptr, 0);

        if (errno == ERANGE)
            printf("Error: the number is out of the allowed range.\n");
        else if ((errno == EINVAL && val == 0) || (*endptr != '\0'))
        {
            printf("Error: failed to convertion input string to unsigned "
                "integer number.\n");
        }
        else
            break;
    }

    return val;
}

nb_float _nb_menu_input_float() {
    nb_float val;
    char buffer[NB_NAME_MAX];
    char* endptr = buffer;
    
    buffer[NB_NAME_MAX - 1] = '\0';

    while (true)
    {
        _nb_menu_input_str(buffer, NB_NAME_MAX);
        errno = 0;

        #if NB_FLOAT_PRECISION == 1
        val = strtof(buffer, &endptr);
        #elif NB_FLOAT_PRECISION == 2
        val = strtod(buffer, &endptr);
        #elif NB_FLOAT_PRECISION == 4
        val = strtold(buffer, &endptr);
        #endif

        if (errno == ERANGE)
            printf("Error: the number is out of the allowed range.\n");
        else if (*endptr != '\0')
            printf("Error: failed to conversion input string to float "
                "number.\n");
        else
            break;
    }

    return val;
}

char* _nb_menu_input_str(char* const buffer, size_t buffer_size)
{
    char* result;
    
    buffer[buffer_size - 1] = '\0';
    printf(">>> ");
    
    result = fgets(buffer, buffer_size - 1, stdin);
    buffer[strlen(buffer) - 1] = '\0'; // replace '\n' to '\0' at end of string

    return result;
}

void _nb_menu_input_vector(nb_vector2 *const vector)
{
    nb_float x, y;

    printf("Input component \"x\":\n");
    x = _nb_menu_input_float();
    printf("Input component \"y\":\n");
    y = _nb_menu_input_float();

    nb_vector2_init(vector, x, y);
}

void _nb_menu_input_body(nb_body *const body)
{
    char name[NB_NAME_MAX];
    nb_vector2 coords, speed, force;
    nb_float mass;

    printf("Input name:\n");
    _nb_menu_input_str(name, NB_NAME_MAX);

    printf("Input coordinates:\n");
    _nb_menu_input_vector(&coords);
    
    printf("Input speed:\n");
    _nb_menu_input_vector(&speed);

    nb_vector2_init(&force, 0.0, 0.0);

    while (true)
    {
        printf("Input mass:\n");
        mass = _nb_menu_input_float();

        if (mass <= 0.0)
            printf("Error: mass must be greater than zero.\n");
        else
            break;
    }

    nb_body_init(body, name, &coords, &speed, &force, mass);
}
