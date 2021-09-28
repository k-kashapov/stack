#include "Stack.h"

extern const char *_type_name;

int main (int argc, char** argv)
{  
    stack_t st;
    StackInit(st);

    StackPush (&st, 6);
    StackPush (&st, 7);
    StackPush (&st, 8);
    StackPush (&st, 8);
    StackPush (&st, 8);
                        
    StackPop (&st);
    
    st.size = -2;  // Wanted error

    StackPop (&st);

    int err = 0;
    printf ("%d\n", StackTop (&st, &err));
    
    StackPop (&st);

    printf ("%d\n", StackTop (&st, &err));

    StackDtor (&st);

    StackPush (&st, 7); // TODO this situation handling

    return 0;
}
