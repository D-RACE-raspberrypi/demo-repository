#include <stdio.h>
#include <stdarg.h>
#include "utils.h"

void debug_print(const char* format, ...) {
    if (!(DEBUG)) {
        return;
    }
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    return;
}