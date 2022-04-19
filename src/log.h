#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

const char *TERM_RED();
const char *TERM_GREEN(); 
const char *TERM_BLUE();
const char *TERM_CYAN();
const char *TERM_DEFAULT();

void apply_color(FILE *fs, char *color);
void set_error_cat(char *cat);
void panic(bool has_context);
void *chp(void *ptr);
void l_assert(int condition, char *context);
