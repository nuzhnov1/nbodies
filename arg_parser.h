#ifndef ARG_PARSER_H
#define ARG_PARSER_H


#include "nb_types.h"
#include "menu.h"


typedef struct arguments 
{
    char input[PATH_MAX];       // input system
    char output[PATH_MAX];      // output system
    nb_float time;              // end time of modeling
    nb_float delta;             // delta of time
    char filename[PATH_MAX];    // filename for argument -f
    unsigned char s: 1;         // flag -s
    unsigned char m: 1;         // flag -m
    unsigned char q: 1;         // flag -q
    unsigned char h: 1;         // flag -h
} arguments;


extern arguments default_settings;


bool arg_parser(size_t argc, char** argv, arguments *const args);


#endif
