#ifndef NB_TYPES_H
#define NB_TYPES_H


#include <stdbool.h>


// setting default int numbers size if it's not defined earlier
#ifndef NB_INT_SIZE
#define NB_INT_SIZE 4
#endif

// setting default float numbers precision if it's not defined earlier
#ifndef NB_FLOAT_PRECISION
#define NB_FLOAT_PRECISION 2
#endif


// choosing alias for int numbers depending on specified size of int numbers
#if NB_INT_SIZE == 1
typedef char nb_int;
typedef unsigned char nb_uint;
#elif NB_INT_SIZE == 2
typedef short nb_int;
typedef unsigned short nb_uint;
#elif NB_INT_SIZE == 4
typedef int nb_int;
typedef unsigned int nb_uint;
#elif NB_INT_SIZE == 8
typedef long int nb_int;
typedef unsigned long int nb_uint;
#elif NB_INT_SIZE == 16
typedef long long nb_int;
typedef unsigned long long nb_uint;
#endif

// choosing precision for float numbers
#if NB_FLOAT_PRECISION == 1
typedef float nb_float;
#elif NB_FLOAT_PRECISION == 2
typedef double nb_float;
#elif NB_FLOAT_PRECISION == 4
typedef long double nb_float;
#endif


#endif
