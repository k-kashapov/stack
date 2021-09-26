#include "Stack.h"

int main (int argc, char** argv)
{  
    stack_t st;
    StackInit(&st);

    StackPush (&st, 6);
    StackPush (&st, 7);
    StackPush (&st, 8);

    int err = 0;
    printf ("%d\n", StackTop (&st, &err));
    
    StackPop (&st);

    printf ("%d\n", StackTop (&st, &err));

    StackDtor (&st);

    return 0;
}
