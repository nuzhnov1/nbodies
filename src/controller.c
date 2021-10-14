#include <stdio.h>
#include <errno.h>

#include "arg_parser.h"
#include "menu.h"


#define MANUAL_PATH "../manual.txt"


static bool _print_manual();
static bool _print_system(const nb_system *const system,
    arguments_t *const args, bool is_input_system);
static void _print_nums_types_info();


int controller(int argc, char** argv) 
{
    nb_system system;
    arguments_t args;

    if (!arg_parser((size_t)argc, argv, &args))
        return -1;
    
    // if "help" flag is specified
    if (args.h)
    {
        if (!_print_manual())
            return -1;
    }
    // if the input file is specified, but the output file is not specified
    else if (args.input != NULL && args.output == NULL)
    {
        nb_system_init_default(&system);
        if (errno == ENOMEM)
        {
            printf("Critical error: failed to initializing system.\n");
            return -1;
        }

        if (!menu_load_system(&system, args.input) || 
            !_print_system(&system, &args, true))
        {
            nb_system_destroy(&system);
            return -1;
        }

        nb_system_destroy(&system);
    }
    // if both files are specified
    else if (args.input != NULL && args.output != NULL)
    {
        bool quiet = args.q;
        menu_run_t run;

        nb_system_init_default(&system);
        if (errno == ENOMEM)
        {
            printf("Critical error: failed to initializing system.\n");
            return -1;
        }

        run.seq = args.s;
        run.openmp = args.m;
        
        if (!menu_load_system(&system, args.input))
        {
            nb_system_destroy(&system);
            return -1;
        }

        _print_nums_types_info();

        if (!quiet)
            _print_system(&system, &args, true);
        
        menu_run_system(&system, args.time, args.delta, run);
        
        if (!quiet)
            _print_system(&system, &args, false);
        
        if (!menu_save_system(&system, args.output))
        {
            nb_system_destroy(&system);
            return -1;
        }
        
        nb_system_destroy(&system);
    }
    else
    {
        nb_system_init_default(&system);
        if (errno == ENOMEM)
        {
            printf("Critical error: failed to initializing system.\n");
            return -1;
        }

        _print_nums_types_info();
        menu_loop(&system);
        nb_system_destroy(&system);
    }

    return 0;
}

bool _print_manual()
{
    FILE* man_file = fopen(MANUAL_PATH, "rt");
    bool status = true;

    if (man_file == NULL)
    {
        printf("Error: failed to open a manual file.\n");
        return false;
    }

    while (!feof(man_file) && !ferror(man_file))
    {
        int chr = getc(man_file);

        if (chr == EOF)
            break;
        else
            putc(chr, stdout);
    }
    
    if (ferror(man_file))
    {
        printf("Error: failed to read data from manual file.\n");
        status = false;
    }

    fclose(man_file);
    return true;
}

bool _print_system(const nb_system *const system, arguments_t *const args, 
    bool is_input_system)
{
    bool is_print;

    // if filename is not specified
    if (args->filename == NULL)
    {   
        if (is_input_system)
            is_print = printf("Input system:\n") > 0;
        else
            is_print = printf("Output system:\n") > 0;
        
        if (!is_print)
        {
            printf("Error: failed to print system.\n");
            return false;
        }

        if (!menu_print_system(system, stdout))
            return false;
    }
    else
    {
        FILE* file = fopen(args->filename, "wt");
        bool is_print;

        if (file == NULL)
        {
            printf("Error: failed to create file \"%s\".\n",
                args->filename);
            
            return false;
        }

        if (is_input_system)
            is_print = fprintf(file, "Input system:\n") > 0;
        else
            is_print = fprintf(file, "Output system:\n") > 0;
        
        if (!is_print)
        {
            printf("Error: failed to print system.\n");
            fclose(file);

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

void _print_nums_types_info()
{
    printf("To represent data in the system , the following are used:\n");

#if NB_FLOAT_PRECISION == 1
    printf("\tsingle-precision(float) floating-point numbers;\n");
#elif NB_FLOAT_PRECISION == 2
    printf("\tdouble-precision(double) floating-point numbers;\n");
#else
    printf("\textended double-precision(long double) "
        "floating-point numbers;\n");
#endif

    printf("\tintegers in the %u-byte range;\n", NB_INT_SIZE);
}
