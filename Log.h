#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//  Убирает предупреждение о функциях библиотеки stdlib.h в Visual Studio
#pragma warning(disable:4996)

FILE *open_log ();
int print_to_log (FILE *log_file, const char *msg);
