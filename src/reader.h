#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct read_result {
	char *ptr_start;
	size_t length;
	bool is_end;
} read_result;

typedef struct read_result_pool {
	read_result **results;
	size_t length;
	size_t allocated;
	char *src;
} read_result_pool;

size_t get_index(read_result_pool *pool, read_result *result);
int get_split_data(char character);
void add_read_result(read_result_pool *pool, read_result *to_add);

size_t get_line(char *string, size_t index);
char *get_value(read_result *read);
read_result_pool *read_string(char *string);

