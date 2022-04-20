#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DEBUG

#include "tokenizer.h"
#include "ast.h"

#include "tokenizer.c"
#include "parser.c"
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
}

int main(int argc, char **argv) {
    if (argc >= 1)
        argv++;
    int fh = 0;
    while (*argv != NULL) {
        char *file = slurp_file(*argv);
        if (!file)
            panic(true);
        TokenPool *tokens = Tokenize(parse_string(file));
        interpretProgram(constructAST(tokens));
        
        fh++;
        argv++;
    }
    if (fh <= 0) {
        fprintf(stderr, "No files provided.\n");
        print_help();
    }

    return 0;
}
