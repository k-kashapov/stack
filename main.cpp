#include "Stack.h"

extern const char *_type_name;

int main (int argc, char** argv)
{  
    stack_t st = {};
    StackInit(st);
    
    StackDump (&st, 16, "aaaa", -121212);

    StackPush (&st, 6);

    st.size = 0;

    StackDump (&st, 16, "aaaa", -121212);

    StackPush (&st, 5);
    StackPush (&st, 3);
    StackPush (&st, 1);    

    StackPush (&st, -1);
    StackPush (&st, -18);

    StackDump (&st, 16, "aaaa", -121212);

    StackPush (&st, -90);

    StackPop (&st);
    StackPop (&st);
    StackPop (&st);
    
    //*(st.buffer + st.capacity) = 0;  // Wanted error

    StackPush (&st, 7);

    int err = 0;
    printf ("%d\n", StackPop (&st, &err));

    StackDtor (&st);

    StackDump (&st, StackError (&st), "jojo", -32131);

    close_log ();

    return 0;
}
