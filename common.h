#ifndef COMMON_H
#define COMMON_H
#include<string.h>

/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"

/* Status will be used in fn. return type */
typedef enum
{
    failure,
    success
} Status;

/* To know operation user wants to perform */
typedef enum
{
    unsupported,
    encode,
    decode
} Opr_type;

#endif