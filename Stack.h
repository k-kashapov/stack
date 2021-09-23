#pragma once

#include <malloc.h>
#include "Log.h"

typedef int type_t;

struct stack
{
    type_t *buffer;
    int  elements_num;
    int  buff_len;
};

stack createStack ();
int push (stack* dest, type_t value);   
int pop (stack* dest);
type_t top (stack* dest);
