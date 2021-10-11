#ifndef ARG_PARSER_H
#define ARG_PARSER_H


#include "nb_types.h"


typedef struct arguments_t 
{
    bool s, m, q, h;    // flags -s, -m, -q, h
    nb_float time;      // end time of modeling
    nb_float delta;     // delta of time
    char* input;        // input system
    char* output;       // output system
    char* filename;     // filename for argument -f
} arguments_t;


bool arg_parser(size_t argc, char** argv, arguments_t *const args);


#endif
