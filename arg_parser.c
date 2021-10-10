#include "arg_parser.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>


static bool _parse_single_dash_args(size_t argc, char** argv, 
    arguments *const args, size_t* const num);
static bool _parse_double_dash_args(size_t argc, char** argv, 
    arguments *const args, size_t* const num);


arguments default_settings = 
{
    "\0", "\0",
    10.0, 0.1,
    "\0",
    false, true,
    false, false
};


bool arg_parser(size_t argc, char** argv, arguments *const args)
{
    *args = default_settings;

    // skip first argument - program name
    for (size_t i = 1; i < argc; i++)
    {
        char* arg = argv[i];

        if (arg[0] == '-' && args->input[0] == '\0' && 
            args->output[0] == '\0')
        {
            bool success;

            if (strlen(arg) > 1 && arg[1] == '-')
                success = _parse_single_dash_args(argc, argv, args, &i);
            else
                success = _parse_single_dash_args(argc, argv, args, &i);
            
            if (!success)
                return false;
        }
        else if (args->input[0] == '\0')
        {
            strncpy(args->input, arg, PATH_MAX - 1);
            args->input[PATH_MAX - 1] = '\0';
        }
        else if (args->output[0] == '\0')
        {
            strncpy(args->output, arg, PATH_MAX - 1);
            args->output[PATH_MAX - 1] = '\0';
        }
        else
        {
            printf("Failed parse: unknown argument \"%s\".\n", arg);
            return false;
        }
    }

    return true;
}

bool _parse_single_dash_args(size_t argc, char** argv, arguments *const args, 
    size_t* const num)
{
    size_t index = *num;
    char* arg = argv[index];
    size_t arg_len = strlen(arg);

    for (size_t i = 1; i < arg_len; i++)
    {
        char flag = arg[i];

        switch (flag)
        {
        case 't': case 'd':
        {
            if (arg[i + 1] != '\0' && strlen(arg) == 2)
            {
                printf("Failed parse: argument \"-%c\" must not be included "
                    "in the union of arguments.\n", flag);
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
                    "to float value for parameter \"-%c\".\n", add_arg, flag);
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

            break;
        }
        case 'f':
        {
            if (arg[i + 1] != '\0' && strlen(arg) == 2)
            {
                printf("Failed parse: argument \"-f\" must not be included "
                    "in the union of arguments.\n");
                return false;
            }
            
            index++;
            if (index >= argc)
            {
                printf("Failed parse: value is not set for parameter "
                    "\"-f\".\n");
                *num = index;

                return false;
            }

            char* arg_add = argv[index];
            strncpy(args->filename, arg_add, PATH_MAX - 1);
            args->filename[PATH_MAX - 1] = '\0';

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

bool _parse_double_dash_args(size_t argc, char** argv, arguments *const args,
    size_t* const num)
{
    size_t index = *num;
    char* arg = argv[index] + 2;
    char* sep = strchr(arg, '=');
    char* add_arg;
    char* temp;

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

    if (temp = strstr(arg, "time"))
    {
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
        if (sep != NULL && (sep - temp == strlen("file")) ||
            sep == NULL && (strcmp(arg, "file") == 0))
        {
            if (add_arg == NULL)
            {
                printf("Failed parse: value is not set for parameter "
                    "\"--file\".\n");
                return false;
            }

            strncpy(args->filename, add_arg, PATH_MAX - 1);
            args->filename[PATH_MAX - 1] = '\0';
            
            return true;
        }
    }
    else if (temp = strstr(arg, "help"))
    {
        if (sep != NULL && (sep - temp == strlen("help")) ||
            sep == NULL && (strcmp(arg, "help") == 0))
        {
            args->h = true;
        }
    }
    
    printf("Failed parse: unknown parameter \"%s\".\n", arg);
    
    return false;
}
