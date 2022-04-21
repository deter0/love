#include "log.h"
#include "reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DEBUG

#include "tokenizer.h"
#include "ast.h"

#include "tokenizer.c"
#include "reader.c"
#include "log.c"
#include "ast.c"
#include "interpreter.c"

char *slurp_file(char *file_path) {
    FILE *fp = fopen(file_path, "rb");
    if (!fp) {
        set_error_cat("[PRE-COMPILER ERROR]");
        fprintf(stderr, "Error opening file `%s`: %s\n", file_path, strerror(errno));
        return NULL;
    }
    size_t len;
    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    rewind(fp);

    char *buffer = chp(malloc(len + 1));

    if (fread(buffer, len, 1, fp) != 1) {
        set_error_cat("[PRE-COMPILE ERROR]");
        fprintf(stderr, "Error reading file `%s`: %s\n", file_path, strerror(errno));
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    buffer[len] = '\0';

    return buffer;
}

void print_help() {
    fprintf(stderr, "Usage: love [options] file...\nOptions:\n");
    fprintf(stderr, "\t-c Compile all files to x86_64 native assembly and statically link them to an executable.\n\t   If not passed it will be interpreted.\n");
}

int main(int argc, char **argv) {
    assert(argc >= 1);
    argv++;
    argc--;
    char **input_files = malloc(sizeof(char*) * (argc + 1));
    int input_files_count = 0;
    bool compile = false;
    for (int i = 0; i < argc; i++) {
        assert(strlen(argv[i]) >= 1);
        if (argv[i][0] == '-') {
            if (!strcmp(argv[i], "-c")) {
                compile = true;
            } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
                print_help();
                exit(1);
            } else {
                goto addAsFile;
            }
        } else {
            goto addAsFile;
        }
        continue;
        addAsFile:
            input_files[input_files_count] = argv[i];
            input_files_count++;
    }
    if (input_files_count <= 0) {
        set_error_cat("[INPUT]");
        fprintf(stderr, "No input files provided. (--help for help)\n");
        panic(true);
    }
    if (compile) {
        fprintf(stderr, "Compiling is not implemeneted.\n");
        return 1;
    } else {
        for (int i = 0; i < input_files_count; i++) {
            char *file = slurp_file(input_files[i]);
            if (file == NULL) {
                set_error_cat("[ERROR READING]");
                fprintf(stderr, "Error reading file `%s`: %s", input_files[i], strerror(errno));
                panic(true);
            }
            read_result_pool *read_resultd = read_string(file);
            TokenPool *tokens = Tokenize(read_resultd);
            ASTMethod *root = constructAST(tokens);
            interpretProgram(root);
        }
    }

    return 0;
}
