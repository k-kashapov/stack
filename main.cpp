#include "Stack.h"

extern const char *_type_name;

int main (int argc, const char** argv)
{                              
    stack_t st = {};
    StackInit(st);

    StackPush (&st, 6);                          
    StackPush (&st, 5);
    StackPush (&st, -1);
    StackPush (&st, -18);
    StackPush (&st, -90);

    uint64_t err = 0;

    StackPush (&st, 1000-7);

    StackDtor (&st);

    return 0;
}
