#include "Log.h"

// TODO log files
//FILE *open_log ()
//{
//    
//    assert (log_file);
//    
//    return log_file;
//}

int print_to_log (FILE *log_file, const char *msg)
{
    assert (log_file);

    fputs (msg, log_file);
    fputs ("\n", log_file);
    
    return 0;
}
