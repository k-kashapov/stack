#include "Stack.h"

const int BUFFER_INIT_SIZE = 10;

const int MEMORY_ALLOC_ERR = -1;

stack createStack ()
{
    type_t *buffer = (type_t *) calloc (BUFFER_INIT_SIZE, sizeof(type_t));
    assert (buffer);

    stack new_stack; // = (stack) calloc (1, sizeof(stack));
    new_stack.buffer = buffer;
    new_stack.elements_num = 0;
    new_stack.buff_len = BUFFER_INIT_SIZE;
    
    return new_stack;
}

int push (stack* dest, type_t value)
{   
    if (dest->elements_num >= dest->buff_len)
    {
        type_t *tmp_buff = (type_t *) realloc (dest->buffer, (dest->buff_len + BUFFER_INIT_SIZE) * sizeof (type_t));
        if (!tmp_buff)
        {
            printf ("ERROR: can't allocate memory");
            return MEMORY_ALLOC_ERR;
        }
        dest->buffer = tmp_buff;
        dest->buff_len += BUFFER_INIT_SIZE;
    }

    assert (dest->buffer);
    *(dest->buffer + dest->elements_num) = value;
    (dest->elements_num)++;
    

    return 0;
}

int pop (stack* dest)
{
    if (dest->elements_num < 1)
    {
        printf ("ERROR: 0 elements in stack");
        return MEMORY_ALLOC_ERR;
    }
    
    *(dest->buffer + dest->elements_num - 1) = 0;
    dest->elements_num--;
    
    return 0;
}

type_t top (stack* dest)
{                  
    if (dest->elements_num < 1)
    {
        printf ("ERROR: 0 elements in stack");
        return *dest->buffer;
    }

    type_t copy = *(dest->buffer + dest->elements_num - 1);

    *(dest->buffer + dest->elements_num - 1) = 0;
    dest->elements_num--;

    return copy;
}
