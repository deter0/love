#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TERM_RED "\033[0;31m"
#define TERM_GREEN "\033[0;32m"
#define TERM_BLUE "\033[0;34m"
#define TERM_CYAN "\033[0;36m"
#define TERM_DEFAULT "\033[0m"

void apply_color(FILE *fs, char *color);
void set_error_cat(char *cat);
void panic(bool has_context);
void *chp(void *ptr);
void l_assert(int condition, char *context);
