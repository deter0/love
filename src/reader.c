#include "reader.h"
#include "log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

int get_split_data(char character) {
	switch (character) {
		case ' ':
		case '\t':
		// case '\n':
		case '\r':
		return 1;
		
		case '-':
		case '+':
		case '*':
		case '/':
		case '[':
		case ']':
		case '{':
		case '}':
		case ';':
		case '=':
		case '!':
		case '@':
		case ',':
		case '$':
		case '~':
		case '<':
		case '>':
		case '`':
		case '%':
		case '^':
		case '&':
		case '(':
		case ')':
		case '#':
		case '.':
		case '?':
		case '\n':
		return 2;
		
		default:
			return -1;
	}
}

void add_read_result(read_result_pool *pool, read_result *to_add) {
	if (pool->length + 1 >= pool->allocated) {
		pool->allocated = pool->allocated * 1.5 + 10;
		pool->results = (read_result **)realloc((void*)pool->results, sizeof(read_result*) * pool->allocated);
		if (pool->results == NULL) {
			fprintf(stderr, "Error reallocating pool memory.\n");
			exit(1);
		}
	}
	pool->results[pool->length] = to_add;
	pool->length++;
}

void free_read_result_pool(read_result_pool *pool) {
	for (size_t i = 0; i < pool->length; i++) {
		free(pool->results[i]);
	}
	free(pool);
}

size_t get_line(char *string, size_t index) {
	size_t count = 1;
	for (size_t j = 0; j < index; j++) {
		if (string[j] == '\n') {
			count++;
		}
	}
	return count;
}

char *get_value(read_result *read) {
	char *buffer = (char *)malloc(sizeof(char) * (read->length + 1));
	assert(buffer != NULL && "error allocating string.");
	if (buffer == NULL) {
		fprintf(stderr, "Error allocating buffer of size %s %ld\n", read->ptr_start, read->length + 1);
		exit(1);
	}
	size_t ptr_size = strlen(read->ptr_start);
	for (size_t k = 0; k < read->length && k < ptr_size; k++) {
		buffer[k] = read->ptr_start[k];
	}
	buffer[read->length] = '\0';
	return buffer;
}

size_t get_index(read_result_pool *pool, read_result *result) {
	size_t srclen = strlen(pool->src);
	size_t diff_to_end = strlen(result->ptr_start);
	return srclen - diff_to_end;
}

read_result_pool *read_string(char *string) {
	read_result_pool *pool = (read_result_pool *)chp(calloc(1, sizeof(read_result_pool)));
	pool->allocated = 50;
	pool->results = (read_result **)calloc(pool->allocated, sizeof(read_result*));
	// for (size_t n = 0; n < strlen(string); n++) {
	// 	if (string[n] == '\n') {
	// 		string[n] = ' ';
	// 	}
	// }
	
	size_t i = 0, t = 0;
	uint8_t locked_string = 0;
	
	while (1) {
		if (string[i] == '\0')
			break;
		if (string[i] == '"' || string[i] == '\'') {
			if (locked_string && (string[i] == '"' ? 1 : 2) == locked_string) {
				size_t escape_counter = 0;
				for (int j = i - 1; j > 0; j--) {
					if (string[j] == '\\') {
						escape_counter++;
					} else {
						break;
					}
				}
				if (escape_counter % 2 == 0 || escape_counter == 0) {
					locked_string = 0;
				} else {
					fprintf(stderr, "Invalid string line: %ld.\n", get_line(string, i));
					exit(1);
				}
			} else if (!locked_string) {
				t = i;
				locked_string = string[i] == '"' ? 1 : 2;
			}
		}
		if (locked_string == 0) {
			int split_data = get_split_data(string[i]);
			switch (split_data) {
			case 1:
				//read_result *thing = (read_result *)malloc(sizeof(read_result));
				{
					read_result *before_i = (read_result *)malloc(sizeof(read_result));
					assert(before_i != NULL && "error allocating memory (1).\n");
					before_i->ptr_start = string + t;
					before_i->length = i - t;
					before_i->is_end = false;
					add_read_result(pool, before_i);
					t = i + 1;
				}
				break;
			
			case 2:
				{
					read_result *before_i = (read_result *)malloc(sizeof(read_result));
					assert(before_i != NULL && "error allocating memory (2).\n");
					before_i->ptr_start = string + t;
					before_i->length = i - t;
					before_i->is_end = false;
					add_read_result(pool, before_i);
					
					read_result *character = (read_result *)malloc(sizeof(read_result));
					assert(character != NULL && "error allocating memory (3).\n");
					character->ptr_start = string + i;
					character->length = 1;
					character->is_end = false;
					add_read_result(pool, character);
					t = i + 1;
				}
				break;
			
			default:
				break;
			}
		}
		i++;
	}
	
	read_result_pool *pool_fixed = (read_result_pool *)chp(calloc(1, sizeof(read_result_pool)));
	pool_fixed->allocated = pool->allocated;
	pool_fixed->results = (read_result **)calloc(pool_fixed->allocated, sizeof(read_result*));
	pool_fixed->src = string;

	for (size_t i = 0; i < pool->length; i++) {
		if (pool->results[i]->length > 0) {
			read_result *result = (read_result*)chp(calloc(1, sizeof(read_result)));
			memcpy(result, pool->results[i], sizeof(read_result));
			add_read_result(pool_fixed, result);
		}
	}
	free_read_result_pool(pool);
#ifdef DEBUG
	for (size_t k = 0; k < pool_fixed->length; k++) {
		char *val = get_value(pool_fixed->results[k]);
		if (!strcmp(val, "\n")) {
			printf("TOKEN: `<NEW LINE>`\n");
		} else {
			printf("TOKEN: `%s`\n", val);
		}
	}
	printf("%ld\n", pool_fixed->length);
#endif
	
	return pool_fixed;
}
