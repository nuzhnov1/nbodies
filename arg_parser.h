#ifndef ARG_PARSER_H
#define ARG_PARSER_H


#include "nb_types.h"


typedef struct arguments_t 
{
    char input[PATH_MAX];       // input system
    char output[PATH_MAX];      // output system
    nb_float time;              // end time of modeling
    nb_float delta;             // delta of time
    char filename[PATH_MAX];    // filename for argument -f
    bool s, m, q, h;            // flags: -s, -m, -q and -h
} arguments_t;


bool arg_parser(size_t argc, char** argv, arguments_t *const args);


#endif
