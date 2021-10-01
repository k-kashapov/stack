#include "Stack.h"

const int BUFFER_INIT_SIZE = 1;

FILE* log_file = fopen ("log.html", "wt");
const char *_type_name = "int";

const int POISON = 0x42;
uint64_t Stack_Err = 0;

const int CANARY_VAL = 0xC0DEDEAD;

uint64_t StackInit_ (stack_t *stk, const char *file_name, const char *func_name, const int line)
{   
    stk->buffer = NULL;
    
    stk->capacity = 0;
    stk->size = 0;

    #ifdef DEBUG_INFO
        if (file_name && func_name && line > 0)
        {
            stk->file = file_name;
            stk->func = func_name;
            stk->line = line;
        }
    #endif 

    #ifdef CANARY_PROTECTION
        stk->canary_l = CANARY_VAL ^ (uint64_t) stk;
        stk->canary_r = CANARY_VAL ^ (uint64_t) stk;
    #endif 
    
    return OK;
}

uint64_t StackError (stack_t *stk)
{
    uint64_t StkErrors = 0;
    uint64_t canary_check = CANARY_VAL ^ (uint64_t) stk;

    if (stk->buffer == (type_t *)POISON)
    {
        StkErrors |= BAD_PTR;
    }
    if (stk->capacity < 0)
    {        
        StkErrors |= CAPACITY_UNDER_ZERO;
    }
    if (stk->size < 0)
    {        
        StkErrors |= SIZE_UNDER_ZERO;
    }
    if (stk->size > stk->capacity)
    {        
        StkErrors |= SIZE_OVER_CAPACITY;
    }

    #ifdef CANARY_PROTECTION
        if (stk->canary_l != canary_check)
        {        
            StkErrors |= LEFT_CANARY_DEAD;
        }                                   
        if (stk->canary_r != canary_check)
        {        
            StkErrors |= RIGHT_CANARY_DEAD;
        }
    #endif

    #ifdef EXPENSIVE_PROTECTION
        for (int iter = stk->size; iter > 0 && iter < stk->capacity; iter++)
        {
            if (stk->buffer [iter] != POISON)
            {
                StkErrors |= NOT_POISONED_BEYOND_SIZE;
                break;
            }
        }
    #endif 


    return StkErrors;
}

int StackDump (stack_t *stk, uint64_t err, const char *called_from, const int line_called_from)
{    
    const char *err_string = err ? "<em style = \"color : red\">ERROR</em>" : "<em style = \"color : #00FA9A\">ok</em>";
    
    fprintf (log_file, "<pre>[%s] [%s] Stack &#60%s&#62 [&%p] \"%s\" %s at %s at %s (%d); called from %s (%d)</pre>",\
             __DATE__, __TIME__, _type_name, stk, stk->name, err_string, stk->func, stk->file, stk->line, called_from, line_called_from);

    if (err)
    {
        PRINT_ERR (err, BAD_PTR);
        PRINT_ERR (err, ZERO_ELEM_POP);
        PRINT_ERR (err, CAPACITY_UNDER_ZERO);
        PRINT_ERR (err, SIZE_UNDER_ZERO);
        PRINT_ERR (err, SIZE_OVER_CAPACITY);
        PRINT_ERR (err, LEFT_CANARY_DEAD);
        PRINT_ERR (err, RIGHT_CANARY_DEAD);
        PRINT_ERR (err, NOT_POISONED_BEYOND_SIZE);

    }
    
    if (err)
    {
        fprintf (log_file, "<pre>{\n\tcapacity = %d;\n\tsize = %d;\n\tbuffer <%s> [&%p]\n\t{\n",\
                 stk->capacity, stk->size, _type_name, stk->buffer);                                                                                                             

        if (err == BAD_PTR)
        {
            fprintf (log_file, "\n\t}\n}\n </pre>");
            return 0;
        }

        for (int buff_iter = 0; buff_iter < stk->capacity; buff_iter++)
        {
            const char *poison = stk->buffer[buff_iter] == POISON ? "POISON" : "";
            if (buff_iter < stk->size)
            {
                fprintf (log_file, "\t\t*[%d] = %d %s\n", buff_iter, stk->buffer[buff_iter], poison);
            }
            else
            {
                fprintf (log_file, "\t\t [%d] = %d %s\n", buff_iter, stk->buffer[buff_iter], poison);
            }
        }

        fprintf (log_file, "\t}\n}</pre>");
    }
    return 0;
}
         
uint64_t StackResize (stack_t *stk, ResizeDir direction)
{
    STACK_OK (stk);

    if (stk->capacity == 0)
    {
        type_t *buffer = (type_t *) calloc (BUFFER_INIT_SIZE, sizeof(type_t));
        assert (buffer);

        stk->buffer = buffer;
        stk->capacity = BUFFER_INIT_SIZE;

        return StackError (stk);
    }
    
    type_t *buff = NULL;

    if (direction > 0)
    {
        buff = (type_t *) realloc (stk->buffer, (stk->capacity * 2) * sizeof (type_t)); 
    }
    else
    {
        buff = (type_t *) realloc (stk->buffer, (stk->capacity / 2) * sizeof (type_t));
    }

    if (!buff)
    {
        printf ("ERROR: can't allocate memory\n");
        return BAD_PTR;
    }

    stk->buffer = buff;
    if (direction > 0)
    {
        stk->capacity *= 2;

        for (int poison_iter = stk->size; poison_iter < stk->capacity; poison_iter++)
        {
            stk->buffer [poison_iter] = POISON;
        }
    }
    else
    {
        stk->capacity /= 2;
    }
    
    STACK_OK (stk);
    return OK;
}

uint64_t StackPush (stack_t* stk, type_t value)
{   
    STACK_OK (stk);

    if (stk->size >= stk->capacity)
    {
        StackResize (stk, RESIZE_UP);  
    }

    assert (stk->buffer);
    stk->buffer[stk->size++] = value;

    STACK_OK (stk);
    return OK;
}


type_t StackPop (stack_t* stk, int *err_ptr)
{                  
    STACK_OK (stk);

    if (Stack_Err)
    {
        if (stk->size < 1)
        {
            printf ("ERROR: 0 elements in stack\n");
            *err_ptr = ZERO_ELEM_POP;
            return ZERO_ELEM_POP;
        }
    }

    type_t copy = *(stk->buffer + stk->size - 1);

    stk->buffer[--stk->size] = POISON;

    if (stk->size <= stk->capacity / 4)
    {
        StackResize (stk, RESIZE_DOWN);
    }

    STACK_OK (stk); 
    return copy;
}

uint64_t StackDtor (stack_t *stk)
{   
    Stack_Err = StackError (stk);                
    StackDump (stk, Stack_Err, __FUNCSIG__, __LINE__);

    if (Stack_Err & BAD_PTR)
        return Stack_Err;
    
    free (stk->buffer);
    
    stk->buffer = (type_t *)POISON;
    
    stk->capacity = -1;
    stk->size = -1;

    fprintf (log_file, "<em style = \"color : #00FA9A\">Stack Destructed</em>");

    return OK;
}

int close_log ()
{
    fclose (log_file);
    
    return 0;
}

