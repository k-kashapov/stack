#include "Stack.h"

const int BUFFER_INIT_SIZE = 1;

FILE* log_file = fopen ("log.html", "wt");

const char *_type_name = "int";

const int ERR_PTR = 0x42;

ErrorCodes StackInit_ (stack_t *stk)
{   
    stk->buffer = NULL;
    
    stk->capacity = 0;
    stk->size = 0;

    return OK;
}

ErrorCodes StackInit_d (stack_t *stk, const char *file_name, const char *func_name, const int line)
{   
    stk->buffer = NULL;
    
    stk->capacity = 0;
    stk->size = 0;
    
    if (file_name && func_name && line > 0)
    {
        stk->file = (char *)file_name;
        stk->func = (char *)func_name;
        stk->line = line;
    }

    return OK;
}

ErrorCodes StackDtor (stack_t *stk)
{   
    STACK_OK (stk);
    
    free (stk->buffer);
    
    stk->buffer = (type_t *)ERR_PTR;
    
    stk->capacity = 0;
    stk->size = 0;
    
    STACK_OK (stk);

    return OK;
}

ErrorCodes StackError (stack_t *stk)
{
    if (stk->buffer == (type_t *)ERR_PTR)
    {
        return BAD_PTR;
    }
    else if (stk->capacity < 0)
    {        
        return CAPACITY_UNDER_ZERO;
    }
    else if (stk->size < 0)
    {        
        return SIZE_UNDER_ZERO;
    }
    else if (stk->size > stk->capacity)
    {        
        return SIZE_OVER_CAPACITY;
    }

    return OK;
}
         
ErrorCodes StackResize (stack_t *stk, ResizeDir direction)
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
    }
    else
    {
        stk->capacity /= 2;
    }

    return OK;
}

ErrorCodes StackPush (stack_t* stk, type_t value)
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

ErrorCodes StackPop (stack_t* stk)
{
    STACK_OK (stk);

    if (stk->size < 1)
    {
        printf ("ERROR: 0 elements in stack\n");
        return ZERO_ELEM_POP;
    }
    
    stk->buffer[--stk->size] = 0;
    
    if (stk->size <= stk->capacity / 4)
    {
        StackResize (stk, RESIZE_DOWN);
    }

    STACK_OK (stk);
    return OK;
}

type_t StackTop (stack_t* stk, int *err)
{                  
    STACK_OK (stk);
    if (err)
    {
        if (stk->size < 1)
        {
            printf ("ERROR: 0 elements in stack\n");
            *err = ZERO_ELEM_POP;
            return ZERO_ELEM_POP;
        }
    }

    type_t copy = *(stk->buffer + stk->size - 1);

    stk->buffer[--stk->size] = 0;

    if (stk->size <= stk->capacity / 4)
    {
        StackResize (stk, RESIZE_DOWN);
    }

    STACK_OK (stk); 
    return copy;
}

int StackDump (stack_t *stk, ErrorCodes err, const char *called_from)
{    
    char err_msg[100] = {};
    switch(err)
    {
    case OK:
        strcpy (err_msg, "<em style = \"color : #00FA9A\">ok</em>");
        break;
    case BAD_PTR:
        strcpy (err_msg, "<em style = \"color : red\">ERROR: BAD BUFFER POINTER</em>");
        break;
    case ZERO_ELEM_POP:
        strcpy (err_msg, "<em style = \"color : red\">ERROR: zero element pop</em>");
        break;
    case CAPACITY_UNDER_ZERO:
        strcpy (err_msg, "<em style = \"color : red\">ERROR: CAPACITY IS LESS THAN 0</em>");
        break;
    case SIZE_UNDER_ZERO:
        strcpy (err_msg, "<em style = \"color : red\">ERROR: SIZE IS LESS THAN 0</em>");
        break;
    case SIZE_OVER_CAPACITY:
        strcpy (err_msg, "<em style = \"color : red\">ERROR: CAPACITY IS LESS THAN SIZE</em>");
        break;
    default:
        strcpy (err_msg, "<em style = \"color : red\">ERROR: UNKNOWN ERROR</em>");
        break;
    }

    fprintf (log_file, "<pre>[%s] [%s] Stack <%s> [&%p] %s \"%s\" at %s at %s (%d); called from %s\n</pre>",\
             __DATE__, __TIME__, _type_name, stk, err_msg, stk->name, stk->func, stk->file, stk->line, called_from);
    
    if (err != OK)
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
            if (buff_iter < stk->size)
            {
                fprintf (log_file, "\t\t*[%d] = %d\n", buff_iter, stk->buffer[buff_iter]);
            }
            else
            {
                fprintf (log_file, "\t\t[%d] = %d\n", buff_iter, stk->buffer[buff_iter]);
            }
        }

        fprintf (log_file, "\t}\n}</pre>");
    }
    return 0;
}

int close_log ()
{
    fclose (log_file);
    
    return 0;
}

