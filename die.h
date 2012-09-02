#ifndef DIE_H
#define DIE_H

#include <stdio.h>
#include <stdlib.h>

#define die(fmt, args...)\
    fprintf(stderr, fmt "\n", ##args);\
    exit(1);

#endif
