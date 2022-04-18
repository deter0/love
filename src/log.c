#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "log.h"

void apply_color(FILE *fs, char *color) {
    fprintf(fs, "%s", color);
}

void set_error_cat(char *cat) {
    fprintf(stderr, "\033[0;35m%s\033[0m ", cat);
}

void panic(bool has_context) {
    set_error_cat(has_context == true ? "\t↳ [FATAL ERROR]" : "[FATAL ERROR]");
    fprintf(stderr, "Fatal error, exiting (1)\n");
    exit(1);
}

void *chp(void *ptr) {
    if (ptr == NULL) {
        set_error_cat("[MEMORY ERROR]");
        fprintf(stderr, "Error allocating memory.\n");
        panic(true);
    }
    return ptr;
}

void l_assert(int condition, char *context) {
    if (condition != 1) {
        set_error_cat("[UNKNOWN]");
        fprintf(stderr, "Assertion failed! Context: \"%s\"\n", context);
        panic(true);
    }
}
