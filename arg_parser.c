#include "arg_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


static bool _parse_single_dash_args(size_t argc, char** argv, 
    arguments_t *const args, size_t* const num);
static bool _parse_double_dash_args(size_t argc, char** argv, 
    arguments_t *const args, size_t* const num);


static arguments_t _default_args_settings = 
{
    false, false,
    false, false,
    10.0, 0.1,
    NULL, NULL, NULL
};


bool arg_parser(size_t argc, char** argv, arguments_t *const args)
{
    *args = _default_args_settings;

    // skip first argument - program name
    for (size_t i = 1; i < argc; i++)
    {
        char* arg = argv[i];
        
        // if arguments start with dash and input 
        // and output files while not specified
        if (arg[0] == '-' && args->input == NULL && args->output == NULL)
        {
            bool success;

            if (arg[1] == '-')
                success = _parse_double_dash_args(argc, argv, args, &i);
            else
                success = _parse_single_dash_args(argc, argv, args, &i);
            
            if (!success)
                return false;
        }
        // if arg is filename and both files are not specified
        else if (args->input == NULL)
            args->input = arg;
        // if arg is filename and input file is specified
        else if (args->output == NULL)
            args->output = arg;
        else
        {
            printf("Failed parse: unknown argument \"%s\".\n", arg);
            return false;
        }
    }

    // By default use only -m flag
    if (!args->s && !args->m)
        args->m = true;

    return true;
}

bool _parse_single_dash_args(size_t argc, char** argv, arguments_t *const args, 
    size_t* const num)
{
    size_t index = *num;
    char* arg = argv[index] + 1;  // skip start dash
    size_t arg_len = strlen(arg);

    for (size_t i = 0; i < arg_len; i++)
    {
        char flag = arg[i];

        switch (flag)
        {
        case 't': case 'd': case 'f':
        {
            // if "t", "d" or "f" parameters are including in the union 
            // of arguments
            if (strlen(arg) > 1)
            {
                printf("Failed parse: argument \"-%c\" must not be included "
                    "in the union of arguments_t.\n", flag);
                return false;
            }

            index++;
            if (index >= argc)
            {
                printf("Failed parse: value is not set for parameter "
                    "\"-%c\".\n", flag);
                *num = index;

                return false;
            }

            char* add_arg = argv[index];

            if (flag != 'f')
            {
                char* endptr = add_arg;
                double value;
                #if NB_FLOAT_PRECISION == 1
                value = strtof(add_arg, &endptr);
                #elif NB_FLOAT_PRECISION == 2
                value = strtod(add_arg, &endptr);
                #elif NB_FLOAT_PRECISION == 4
                value = strtold(add_arg, &endptr);
                #endif

                if (errno == ERANGE)
                {
                    printf("Failed parse: the value for parameter \"-%c\" is "
                        "out of the allowed range.\n", flag);
                    *num = index;

                    return false;
                }
                else if (*endptr != '\0')
                {
                    printf("Failed parse: failed to conversion \"%s\" "
                        "to float value for parameter \"-%c\".\n", add_arg, 
                        flag);
                    *num = index;

                    return false;
                }
                else
                {
                    if (flag == 't')
                        args->time = value;
                    else
                        args->delta = value;
                }
            }
            else
                args->filename = add_arg;

            break;
        }
        case 's':
        {
            args->s = true;
            break;
        }
        case 'm':
        {
            args->m = true;
            break;
        }
        case 'q':
        {
            args->q = true;
            break;
        }
        case 'h':
        {
            args->h = true;
            break;
        }
        default:
        {
            printf("Failed parse: unknown parameter \"-%c\".\n", flag);
            return false;
        }
        }
    }

    *num = index;
    
    return true;
}

bool _parse_double_dash_args(size_t argc, char** argv, arguments_t *const args,
    size_t* const num)
{
    size_t index = *num;
    char* arg = argv[index] + 2;
    char* sep = strchr(arg, '=');
    char* add_arg;
    char* temp;

    // if '=' is contained in the argument
    if (sep != NULL)
        add_arg = sep + 1;
    else
    {
        index++;
        if (index >= argc)
            add_arg = NULL;
        else
            add_arg = argv[index];
        
        *num = index;
    }

    // if argument is "time"
    if (temp = strstr(arg, "time"))
    {
        // checking the correctness of the "time" argument entry
        if (sep != NULL && (sep - temp == strlen("time")) ||
            sep == NULL && (strcmp(arg, "time") == 0))
        {
            if (add_arg == NULL)
            {
                printf("Failed parse: value is not set for parameter "
                    "\"--time\".\n");
                return false;
            }

            char* endptr = add_arg;
            double value;
            #if NB_FLOAT_PRECISION == 1
            value = strtof(add_arg, &endptr);
            #elif NB_FLOAT_PRECISION == 2
            value = strtod(add_arg, &endptr);
            #elif NB_FLOAT_PRECISION == 4
            value = strtold(add_arg, &endptr);
            #endif

            if (errno == ERANGE)
            {
                printf("Failed parse: the value for parameter \"--time\" is "
                    "out of the allowed range.\n");
                return false;
            }
            else if (*endptr != '\0')
            {
                printf("Failed parse: failed to conversion \"%s\" "
                    "to float value for parameter \"--time\".\n", add_arg);
                return false;
            }
            else
                args->time = value;
            
            return true;
        }
    }
    else if (temp = strstr(arg, "delta"))
    {
        // checking the correctness of the "delta" argument entry
        if (sep != NULL && (sep - temp == strlen("delta")) ||
            sep == NULL && (strcmp(arg, "delta") == 0))
        {
            if (add_arg == NULL)
            {
                printf("Failed parse: value is not set for parameter "
                    "\"--delta\".\n");
                return false;
            }

            char* endptr = add_arg;
            double value;
            #if NB_FLOAT_PRECISION == 1
            value = strtof(add_arg, &endptr);
            #elif NB_FLOAT_PRECISION == 2
            value = strtod(add_arg, &endptr);
            #elif NB_FLOAT_PRECISION == 4
            value = strtold(add_arg, &endptr);
            #endif

            if (errno == ERANGE)
            {
                printf("Failed parse: the value for parameter \"--delta\" is "
                    "out of the allowed range.\n");
                return false;
            }
            else if (*endptr != '\0')
            {
                printf("Failed parse: failed to conversion \"%s\" "
                    "to float value for parameter \"--delta\".\n", add_arg);
                return false;
            }
            else
                args->delta = value;
            
            return true;
        }
    }
    else if (temp = strstr(arg, "file"))
    {
        // checking the correctness of the "file" argument entry
        if (sep != NULL && (sep - temp == strlen("file")) ||
            sep == NULL && (strcmp(arg, "file") == 0))
        {
            if (add_arg == NULL)
            {
                printf("Failed parse: value is not set for parameter "
                    "\"--file\".\n");
                return false;
            }

            args->filename = add_arg;
            
            return true;
        }
    }
    else if (temp = strstr(arg, "help"))
    {
        // checking the correctness of the "help" argument entry
        if (sep != NULL && (sep - temp == strlen("help")) ||
            sep == NULL && (strcmp(arg, "help") == 0))
        {
            args->h = true;
        }
    }
    
    printf("Failed parse: unknown parameter \"%s\".\n", arg);
    
    return false;
}
