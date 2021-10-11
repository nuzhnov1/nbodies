#define NB_FLOAT_PRECISION 4
#define NB_INT_SIZE 16


#include <stdio.h>
#include <errno.h>

#include "menu.h"
#include "arg_parser.h"


#define MANUAL_PATH "nbodies_man.txt"


static bool _print_manual();
static bool _print_system(const nb_system *const system,
    arguments_t *const args);


int main(int argc, char** argv) 
{
    nb_system system;
    static arguments_t args;

    // Undefined behaviour in arg_parser
    // Setting some structure values to an invalid state after calling
    // this function
    if (!arg_parser((size_t)argc, argv, &args))
        return -1;
    
    if (args.h)
    {
        if (!_print_manual())
            return -1;
    }
    else if (args.input[0] != '\0' && args.output[0] == '\0')
    {
        if (!menu_load_system(&system, args.input) || 
            !_print_system(&system, &args))
        {
            return -1;
        }
    }
    else if (args.input[0] != '\0' && args.output[0] != '\0')
    {
        bool quiet = args.q;
        menu_run_t run;

        run.seq = args.s;
        run.openmp = args.m;
        
        if (!menu_load_system(&system, args.input))
            return -1;

        if (!quiet)
            _print_system(&system, &args);
        
        menu_run_system(&system, args.time, args.delta, run);
        
        if (!quiet)
            _print_system(&system, &args);
        
        if (!menu_save_system(&system, args.output))
            return -1;
    }
    else
        menu_loop(&system);
    
    nb_system_destroy(&system);

    return 0;
}

bool _print_manual()
{
    FILE* man_file = fopen(MANUAL_PATH, "rt");
    char buffer[1024];

    if (man_file == NULL)
    {
        printf("Error: failed to open a manual file.\n");
        return false;
    }

    while (!feof(man_file) && !ferror(man_file))
    {
        fgets(buffer, 1024, man_file);
        puts(buffer);
    }

    if (ferror(man_file))
        printf("Error: failed to read data from manual file.\n");

    fclose(man_file);

    return true;
}

bool _print_system(const nb_system *const system, arguments_t *const args)
{
    if (args->filename[0] == '\0')
    {
        if (!menu_print_system(system, stdout))
            return false;
    }
    else
    {
        FILE* file = fopen(args->filename, "wt");

        if (file == NULL)
        {
            printf("Error: failed to create file \"%s\".\n",
                args->filename);
            return false;
        }

        if (!menu_print_system(system, file))
        {
            fclose(file);
            return false;
        }
        else
            fclose(file);
    }

    return true;
}
