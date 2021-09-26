#include "Stack.h"

const int BUFFER_INIT_SIZE = 1;

const int MEMORY_ALLOC_ERR = -1;
const int ZERO_ELEMENT_POP = -2;

const int RESIZE_UP = 1;
const int RESIZE_DOWN = -1;

const int ERR_PTR = 0x42;

int StackInit (stack_t *stk, int init_capacity)
{   
    stk->buffer = NULL;
    
    if (init_capacity > 0)
    {
        type_t *buffer = (type_t *) calloc (init_capacity, sizeof(type_t));
        assert (buffer);

        stk->buffer = buffer;
        stk->size = init_capacity;
    }
    
    stk->capacity = 0;
    stk->size = 0;
    
    return 0;
}

int StackDtor (stack_t *stk)
{   
    free (stk->buffer);
    
    stk->buffer = (type_t *)0xEE;
    
    stk->capacity = 0;
    stk->size = 0;
    
    return 0;
}

int StackResize (stack_t *stk, int direction)
{
    if (stk->size == 0)
    {
        type_t *buffer = (type_t *) calloc (BUFFER_INIT_SIZE, sizeof(type_t));
        assert (buffer);

        stk->buffer = buffer;
        stk->size = BUFFER_INIT_SIZE;

        return 0;
    }
    
    type_t *buff = NULL;

    if (direction > 0)
    {
        buff = (type_t *) realloc (stk->buffer, (stk->size * 2) * sizeof (type_t)); 
    }
    else
    {
        buff = (type_t *) realloc (stk->buffer, (stk->size / 2) * sizeof (type_t));
    }

    if (!buff)
    {
        printf ("\nERROR: can't allocate memory\n\n");
        return MEMORY_ALLOC_ERR;
    }

    stk->buffer = buff;
    if (direction > 0)
    {
        stk->size *= 2;
    }
    else
    {
        stk->size /= 2;
    }

    return 0;
}

int StackPush (stack_t* stk, type_t value)
{   
    if (stk->capacity >= stk->size)
    {
        StackResize (stk, RESIZE_UP);  
    }

    assert (stk->buffer);
    *(stk->buffer + stk->capacity) = value;
    (stk->capacity)++;

    return 0;
}

int StackPop (stack_t* stk)
{
    if (stk->capacity < 1)
    {
        printf ("\nERROR: 0 elements in stack\n\n");
        return ZERO_ELEMENT_POP;
    }
    
    *(stk->buffer + stk->capacity - 1) = 0;
    stk->capacity--;
    
    if (stk->capacity <= stk->size / 4)
    {
        StackResize (stk, RESIZE_DOWN);
    }

    return 0;
}

type_t StackTop (stack_t* stk, int *err)
{                  
    if (err)
    {
        if (stk->capacity < 1)
        {
            printf ("\nERROR: 0 elements in stack\n\n");
            *err = ZERO_ELEMENT_POP;
            return ZERO_ELEMENT_POP;
        }
    }

    type_t copy = *(stk->buffer + stk->capacity - 1);

    *(stk->buffer + stk->capacity - 1) = 0;
    stk->capacity--;

    if (stk->capacity <= stk->size / 4)
    {
        StackResize (stk, RESIZE_DOWN);
    }

    return copy;
}
