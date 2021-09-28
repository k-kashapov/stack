#pragma once

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define DEBUG

typedef int type_t;

#ifdef DEBUG
    #define stack_t _stack_td
#endif
#ifndef DEBUG
    #define stack_t _stack_t
#endif // !DEBUG

#define ToString(str) #str, str;

#ifdef DEBUG
    #define StackInit(stk)                                  \
        StackInit_d (&stk, __FILE__, __FUNCSIG__, __LINE__);   \
        stk.name = (char *)ToString(stk);
#endif

#ifndef DEBUG
    #define StackInit (stk)                                  \
        StackInit_ (stk);   
#endif // !DEBUG

#define STACK_OK(stk)                           \
            StackDump (stk, StackError (stk), __FUNCSIG__);


struct _stack_t
{
    type_t *buffer;
    int    capacity;
    int    size;

};

struct _stack_td
{
    type_t *buffer;
    int    capacity;
    int    size;

    char* name;
    char* func;
    char* file;
    int line;

};

enum ErrorCodes
{
    OK = 0,
    BAD_PTR = -1,
    MEM_ALLOC_ERR = -2,
    ZERO_ELEM_POP = -3,
    CAPACITY_UNDER_ZERO = -4,
    SIZE_UNDER_ZERO = -5,
    SIZE_OVER_CAPACITY = -6,

};

enum ResizeDir
{
    RESIZE_UP = 1,
    RESIZE_DOWN = -1,
};

//  Убирает предупреждение о функциях библиотеки stdlib.h в Visual Studio
#pragma warning(disable:4996)

int StackDump (stack_t *stk, ErrorCodes err, const char *called_from);

int close_log ();

ErrorCodes StackInit_ (stack_t *stk);

ErrorCodes StackInit_d (stack_t *stk, const char *file_name = NULL, const char *func_name = NULL, const int line = -1);

ErrorCodes StackDtor (stack_t *stk);

ErrorCodes StackError (stack_t *stk);

ErrorCodes StackPush (stack_t* stk, type_t value);   

ErrorCodes StackPop (stack_t* stk);

type_t StackTop (stack_t* stk, int *err = NULL);
