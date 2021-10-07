#pragma once

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#define DEBUG_INFO
#define CANARY_PROTECTION
#define CHECK_POISON
#define HASH_PROTECTION

#ifdef DEBUG_INFO
    #define MAX_INFO
#endif

typedef int type_t;

#define TO_STRING(str) #str

#ifdef DEBUG_INFO
    #define StackInit(stk)                                        \
        StackInit_ (&stk, __FILE__, __FUNCSIG__, __LINE__, #stk); 

    #define STACK_OK(stk)                                  \
        Stack_Err = StackError (stk);                      \
        StackDump (stk, Stack_Err, __FUNCSIG__, __LINE__); \
        if (Stack_Err) return Stack_Err;
#else
    #define StackInit(stk) \
        StackInit_ (&stk)

    #define STACK_OK(stk) 
#endif 

#ifdef HASH_PROTECTION
    #define COUNT_STACK_HASH(stk, res)                                                 \
        int len = sizeof (*stk) - sizeof (stk->struct_hash) - sizeof (stk->data_hash); \
        res = MurmurHash (stk, len);
                         
    #define COUNT_DATA_HASH(stk, res)                                \
        res = MurmurHash (stk->buffer, stk->size * sizeof (type_t));
#endif

#define PRINT_ERR(err, flag)                                                   \
    if (err & flag)                                                            \
        fprintf (Log_file, "<p style = \"color : red\">ERROR: %s</p>", #flag);

#define REALLOC(ptr, init_len, new_len, ptr_type)         \
    ptr_type *buff = (ptr_type *) realloc (ptr, new_len); \
    assert (buff);                                        \
    ptr = buff;            

struct stack_t
{
    #ifdef CANARY_PROTECTION
        uint64_t canary_l;
    #endif 

    type_t *buffer;
    int    capacity;
    int    size;

    #ifdef DEBUG_INFO
        const char* func;
        const char* file;
        int         line;
        const char* name;
    #endif

    #ifdef CANARY_PROTECTION
        uint64_t canary_r;
    #endif
    
    #ifdef HASH_PROTECTION
        unsigned int struct_hash;
        unsigned int data_hash;
    #endif
};

enum ErrorCodes
{                          
    OK =                       0x0000,
    BAD_PTR =                  0x0001,
    MEM_ALLOC_ERR =            0x0002,
    ZERO_ELEM_POP =            0x0004,
    CAPACITY_UNDER_ZERO =      0x0008,
    SIZE_UNDER_ZERO =          0x0010,
    SIZE_OVER_CAPACITY =       0x0020,
    LEFT_CANARY_DEAD =         0x0040,
    RIGHT_CANARY_DEAD =        0x0080,
    LEFT_DATA_CANARY_DEAD =    0x0100,
    RIGHT_DATA_CANARY_DEAD =   0x0200,
    NOT_POISONED_BEYOND_SIZE = 0x0400,
    STACK_HASH_INVALID =       0x0800,
    DATA_HASH_INVALID =        0x1000
};

//  Убирает предупреждение о функциях библиотеки stdlib.h в Visual Studio
#pragma warning(disable:4996)

uint64_t StackDump (stack_t *stk, uint64_t err, const char *called_from, const int line_called_from);

unsigned int MurmurHash (void *stk, int len);

uint64_t StackInit_ (stack_t *stk, const char *file_name = NULL, const char *func_name = NULL, const int line = -1, const char *name = NULL);

uint64_t StackDtor (stack_t *stk);

uint64_t StackError (stack_t *stk);

uint64_t StackPush (stack_t* stk, type_t value);   

type_t StackPop (stack_t* stk, uint64_t *err = NULL);

type_t StackTop (stack_t *stk, uint64_t *err_ptr);
