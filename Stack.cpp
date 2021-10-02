#include "Stack.h"

const int BUFFER_INIT_SIZE = 1;

FILE* log_file = fopen ("log.html", "wt");
const char *_type_name = "int";

const int POISON = 0x42;
uint64_t Stack_Err = 0;

const uint64_t CANARY_VAL = 0xC0DEDEADC0DEDEAD;

uint64_t StackInit_ (stack_t *stk, const char *file_name, const char *func_name, const int line, const char *name)
{   
    stk->buffer = NULL;
    
    stk->capacity = 0;
    stk->size = 0;

    #ifdef DEBUG_INFO
        if (file_name && func_name && line > 0 && name)
        {
            stk->file = file_name;
            stk->func = func_name;
            stk->line = line;
            stk->name = name;
        }
    #endif 

    #ifdef CANARY_PROTECTION
        stk->canary_l = CANARY_VAL ^ (uint64_t) stk;
        stk->canary_r = CANARY_VAL ^ (uint64_t) stk;
    #endif 
    
    #ifdef HASH_PROTECTION
        COUNT_STACK_HASH (stk, stk->struct_hash);
        COUNT_DATA_HASH  (stk, stk->data_hash);
    #endif

    return OK;
}

uint64_t StackError (stack_t *stk)
{
    uint64_t StkErrors = 0;
    uint64_t canary_check = CANARY_VAL ^ (uint64_t) stk;

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

    if (stk->buffer == (type_t *)POISON)
    {
        StkErrors |= BAD_PTR;
        return StkErrors;
    }

    #ifdef HASH_PROTECTION

        unsigned int stk_hash = 0;
        COUNT_STACK_HASH (stk, stk_hash)

        if (stk_hash != stk->struct_hash)
        {
            StkErrors |= STACK_HASH_INVALID;
        }

        unsigned int data_hash = 0;
        COUNT_DATA_HASH (stk, data_hash)

        if (data_hash != stk->data_hash)
        {
            StkErrors |= DATA_HASH_INVALID;
        }

    #endif

    #ifdef CANARY_PROTECTION
        if (stk->canary_l != canary_check)
        {        
            StkErrors |= LEFT_CANARY_DEAD;
        }                                   
        if (stk->canary_r != canary_check)
        {        
            StkErrors |= RIGHT_CANARY_DEAD;
        }
       
        if (stk->buffer)
        {
            if (*((uint64_t *)stk->buffer - 1) != canary_check)
            {
                StkErrors |= LEFT_DATA_CANARY_DEAD;
            }
            if (*((uint64_t *)(stk->buffer + stk->capacity)) != canary_check)
            {
                StkErrors |= RIGHT_DATA_CANARY_DEAD;
            }
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
    
    #ifdef DEBUG_INFO
        fprintf (log_file, "<pre>[%s] [%s] Stack &#60%s&#62 [&%p] \"%s\" %s at %s at %s (%d); called from %s (%d)</pre>",\
                 __DATE__, __TIME__, _type_name, stk, stk->name, err_string, stk->func, stk->file, stk->line, called_from, line_called_from);
    #endif

    if (err)
    {
        PRINT_ERR (err, BAD_PTR);
        PRINT_ERR (err, ZERO_ELEM_POP);
        PRINT_ERR (err, CAPACITY_UNDER_ZERO);
        PRINT_ERR (err, SIZE_UNDER_ZERO);
        PRINT_ERR (err, SIZE_OVER_CAPACITY);
        PRINT_ERR (err, LEFT_CANARY_DEAD);
        PRINT_ERR (err, RIGHT_CANARY_DEAD);
        PRINT_ERR (err, LEFT_DATA_CANARY_DEAD);
        PRINT_ERR (err, RIGHT_DATA_CANARY_DEAD);
        PRINT_ERR (err, NOT_POISONED_BEYOND_SIZE);
        PRINT_ERR (err, STACK_HASH_INVALID);
        PRINT_ERR (err, DATA_HASH_INVALID);
    }
    
    if (err)
    {
        fprintf (log_file, "<pre>{\n\tcapacity = %d;\n\tsize = %d;",\
                 stk->capacity, stk->size);                                                                                                             

        #ifdef CANARY_PROTECTION
            fprintf (log_file, "\n\tcanary_l = %llx;\n\tcanary_r = %llx;", stk->canary_l ^ (uint64_t)stk, stk->canary_r ^ (uint64_t)stk);
        #endif
                           
        #ifdef HASH_PROTECTION
            unsigned int stk_hash = 0;
            COUNT_STACK_HASH (stk, stk_hash);
            
            unsigned int data_hash = 0;
            COUNT_DATA_HASH (stk, data_hash);                
            fprintf (log_file, "\n\tstruct_hash = %u;\n\texpected struct_hash = %u;\n\tdata_hash = %u;\n\texpected data_hash = %u;", stk_hash, stk->struct_hash, data_hash, stk->data_hash);
        #endif

        fprintf (log_file, "\n\tbuffer &#60%s&#62 [&%p]\n\t{\n", _type_name, stk->buffer);

        if (err & BAD_PTR || !stk->buffer)
        {
            fprintf (log_file, "\n\t}\n}\n </pre>");
            return err;
        }

        #ifdef CANARY_PROTECTION
            fprintf (log_file, "\t\t left_canary = %llx\n", *((uint64_t *)stk->buffer - 1) ^ (uint64_t) stk);
        #endif

        for (int buff_iter = 0; buff_iter < stk->capacity; buff_iter++)
        {
            const char *poison = stk->buffer[buff_iter] == POISON ? "POISON" : "";
            if (buff_iter < stk->size)
            {
                fprintf (log_file, "\t\t*[%d] = %d\n", buff_iter, stk->buffer[buff_iter]);
            }
            else
            {
                fprintf (log_file, "\t\t [%d] = %d %s\n", buff_iter, stk->buffer[buff_iter], poison);
            }
        }
        
        fprintf (log_file, "\t\t right_canary = %llx\n", *((uint64_t *)(stk->buffer + stk->capacity)) ^ (uint64_t) stk);

        fprintf (log_file, "\t}\n}</pre>");
    }
    return 0;
}
         
uint64_t StackResize (stack_t *stk, long new_capacity)
{
    STACK_OK (stk);                               

    if (new_capacity <= 0) new_capacity = BUFFER_INIT_SIZE;

    int buff_len = new_capacity * sizeof (type_t) + 2 * sizeof (CANARY_VAL);    
    uint64_t canary = CANARY_VAL ^ (uint64_t) stk;
    
    if (stk->capacity == 0)
    { 
        REALLOC (stk->buffer, stk->capacity, buff_len, type_t);
        *(uint64_t *)stk->buffer = canary;
        stk->buffer = (type_t *)((uint64_t *)stk->buffer + 1);
    }
    else
    {
        uint64_t *tmp_buff = (uint64_t*) ((char *) stk->buffer - sizeof (CANARY_VAL)); 
        REALLOC (tmp_buff, stk->capacity, buff_len, uint64_t);
        stk->buffer = (type_t *)(tmp_buff + 1);
    }

    stk->capacity = new_capacity;

    *(uint64_t *)(stk->buffer + stk->capacity) = canary;

    for (int iter = stk->size; iter < stk->capacity; iter++)
    {
        stk->buffer [iter] = POISON;
    }
    
    #ifdef HASH_PROTECTION
        COUNT_STACK_HASH (stk, stk->struct_hash);    
        COUNT_DATA_HASH  (stk, stk->data_hash);    
    #endif 

    STACK_OK (stk);
    return OK;
}

uint64_t StackPush (stack_t* stk, type_t value)
{   
    STACK_OK (stk);

    if (stk->size >= stk->capacity)
    {
        StackResize (stk, stk->size * 2);  
    }

    assert (stk->buffer);
    stk->buffer[stk->size++] = value;

    #ifdef HASH_PROTECTION
        COUNT_STACK_HASH (stk, stk->struct_hash);    
        COUNT_DATA_HASH (stk, stk->data_hash);    
    #endif

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
        StackResize (stk, stk->size / 2);
    }

    #ifdef HASH_PROTECTION
        COUNT_STACK_HASH (stk, stk->struct_hash);    
        COUNT_DATA_HASH (stk, stk->data_hash);    
    #endif

    STACK_OK (stk); 
    return copy;
}

unsigned int Hash (void *stk, int len)
{
    if (!stk) return 0;

    const unsigned int m = 0x5bd1e995;
    const unsigned int seed = 0;
    const int r = 24;
    unsigned int h = seed ^ len;

    const unsigned char * data = (const unsigned char *)stk;
    unsigned int k;

    while (len >= 4)
    {
        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len)
    {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h; 
}

uint64_t StackDtor (stack_t *stk)
{   
    Stack_Err = StackError (stk);                
    StackDump (stk, Stack_Err, __FUNCSIG__, __LINE__);

    if (Stack_Err & BAD_PTR || !stk->buffer)
        return Stack_Err;
    
    free ((uint64_t *)stk->buffer - 1);
    
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

