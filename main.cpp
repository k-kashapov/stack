#include "Stack.h"

extern const char *_type_name;

int main (int argc, char** argv)
{  
    stack_t st;
    StackInit(st);

    StackPush (&st, 6);
    StackPush (&st, 6);
    StackPush (&st, 6);
    StackPush (&st, 6);
    StackPush (&st, 6);
    StackPush (&st, 6);
    StackPush (&st, 6);

    StackPop (&st);
    StackPop (&st);
    StackPop (&st);
    
    st.size = st.capacity + 1;  // Wanted error

    StackPush (&st, 7);

    int err = 0;
    printf ("%d\n", StackPop (&st, &err));

    StackDtor (&st);

    close_log ();

    return 0;
}
