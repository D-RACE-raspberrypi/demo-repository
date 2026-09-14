#ifndef UTILS_H
#define UTILS_H

#define DEBUG env_or("DEBUG_MOD", 0);

#define RED_form "\033[31m"
#define GREEN_form "\033[32m"
#define BLUE_form "\033[34m"
#define YELLOW_form "\033[33m"
#define MAGENTA_form "\033[35m"
#define CYAN_form "\033[36m"
#define RESET_form "\033[0m"
#define BOLD_form "\033[1m"
#define ITAL_form "\033[3m"

void debug_printf(const char *format, ...);

#define UTILS

#endif // UTILS_H