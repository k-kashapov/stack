#pragma once

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#define DEBUG_INFO
#define CANARY_PROTECTION
#define EXPENSIVE_PROTECTION

typedef int type_t;

#define ToString(str) #str, str;

#ifdef DEBUG_INFO
    #define StackInit(stk)                                   \
        StackInit_ (&stk, __FILE__, __FUNCSIG__, __LINE__); \
        stk.name = ToString(stk);
#endif

#ifndef DEBUG_INFO
    #define StackInit(stk)                                  \
        StackInit_ (&stk);   
#endif // !DEBUG

#ifdef DEBUG_INFO
    #define STACK_OK(stk)                      \
        Stack_Err = StackError (stk);                \
        StackDump (stk, Stack_Err, __FUNCSIG__, __LINE__);     \
        if (Stack_Err) return Stack_Err;        
#endif // DEBUG

#ifndef DEBUG_INFO
    #define STACK_OK(stk)                \
        Stack_Err = StackError (stk);    \
        if (Stack_Err) return Stack_Err;
#endif // !DEBUG

#define PRINT_ERR(err, flag) \
        if (err & flag)     \
            fprintf (log_file, "<p style = \"color : red\">ERROR: %s</p>", #flag);

struct stack_t
{
    #ifdef CANARY_PROTECTION
        uint64_t canary_l;
    #endif 

    type_t *buffer;
    int    capacity;
    int    size;

    #ifdef DEBUG_INFO
        const char* name;
        const char* func;
        const char* file;
        int line;
    #endif

    #ifdef CANARY_PROTECTION
        uint64_t canary_r;
    #endif 
};

enum ErrorCodes
{
    OK = 0x0,
    BAD_PTR = 0x1,
    MEM_ALLOC_ERR = 0x2,
    ZERO_ELEM_POP = 0x4,
    CAPACITY_UNDER_ZERO = 0x8,
    SIZE_UNDER_ZERO = 0x10,
    SIZE_OVER_CAPACITY = 0x20,
    LEFT_CANARY_DEAD = 0x40,
    RIGHT_CANARY_DEAD = 0x80,
    NOT_POISONED_BEYOND_SIZE = 0x100,

};

enum ResizeDir
{
    RESIZE_UP = 1,
    RESIZE_DOWN = -1,
};

//  Убирает предупреждение о функциях библиотеки stdlib.h в Visual Studio
#pragma warning(disable:4996)

int StackDump (stack_t *stk, uint64_t err, const char *called_from, const int line_called_from);

int close_log ();

uint64_t StackInit_ (stack_t *stk);

uint64_t StackInit_ (stack_t *stk, const char *file_name = NULL, const char *func_name = NULL, const int line = -1);

uint64_t StackDtor (stack_t *stk);

uint64_t StackError (stack_t *stk);

uint64_t StackPush (stack_t* stk, type_t value);   

type_t StackPop (stack_t* stk, int *err = NULL);
