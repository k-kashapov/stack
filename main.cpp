#include "Stack.h"

int main (int argc, char** argv)
{ 
    stack st = createStack();

    push (&st, 6);
    push (&st, 3);
    push (&st, 3);
    push (&st, 3);
    push (&st, 3);
    push (&st, 3);
    push (&st, 3);
    push (&st, 3);
    push (&st, 3);
    push (&st, 3);
    push (&st, 3);
    push (&st, 3);
    push (&st, 3);

    printf ("%d\n", top (&st));
    printf ("%d\n", top (&st));
    printf ("%d\n", top (&st));
    printf ("%d\n", top (&st));
    printf ("%d\n", top (&st));
    printf ("%d\n", top (&st));
    printf ("%d\n", top (&st));
    
    
}
