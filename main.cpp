#include "Stack.h"

extern const char *_type_name;

int main (int argc, char** argv)
{                              
    stack_t st = {};
    StackInit(st);

    StackPush (&st, 6);                          
    StackPush (&st, 5);
    StackPush (&st, 3);
    StackPush (&st, 1);    
    
    //st.size = -1; // Wanted error
    
    StackPush (&st, -1);
    StackPush (&st, -18);
    StackPush (&st, -90);

    StackPop (&st);
    StackPop (&st);
    StackPop (&st);

    StackPush (&st, 1000-3);

    uint64_t err = 0;
    printf ("%d\n", StackTop (&st, &err));
    printf ("%d\n", StackTop (&st, &err));
    printf ("%d\n", StackTop (&st, &err));

    StackDtor (&st);

    close_log ();

    return 0;
}
