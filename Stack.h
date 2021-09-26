#pragma once

#include <malloc.h>
#include "Log.h"

typedef int type_t;

struct stack_t
{
    type_t *buffer;
    int  capacity;
    int  size;
};

int StackInit (stack_t *stk, int init_capacity = 0);

int StackDtor (stack_t *stk);

int StackPush (stack_t* stk, type_t value);   

int StackPop (stack_t* stk);

type_t StackTop (stack_t* stk, int *err = NULL);
