#include "Stack.h"

int main (int argc, char** argv)
{  
    stack_t st;
    StackInit(&st);

    StackPush (&st, 6);
    StackPush (&st, 6);
    StackPush (&st, 6);

    StackPop(&st);
    StackPop(&st);
    StackPop(&st);

    int err = 0;
    printf ("%d\n", StackTop (&st, &err));

    return 0;
}
