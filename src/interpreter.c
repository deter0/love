#include "ast.h"
#include "tokenizer.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void interpretProgram(ASTMethod *executionStart) {
    l_assert(CheckASTMethodDataType(executionStart, "MethodExecutionStartData"), "Need execution start node to interpret program.\n");
    struct ASTNodeExecutionStartData *data = (struct ASTNodeExecutionStartData *)executionStart->NodeData;
    printf("Interpreting program.\n------------------------\n");
    for (size_t i = 0; i < data->MethodsLength; i++) {
        ASTMethod *method = data->Methods[i];
        if (CheckASTMethodDataType(method, FUNCTION_CALL)) {
            struct ASTNodeCallFunctionData *fncCallData
                = (struct ASTNodeCallFunctionData *)method->NodeData;
            if (!strcmp(fncCallData->FunctionToCall, "print")) {
                for (size_t pn = 0; pn < fncCallData->ParameterCount; pn++) {
                    if (fncCallData->Parameters[pn]->isConstant
                    && !strcmp(fncCallData->Parameters[pn]->constantType, "string")) {
                        // Chop of quotes
                        char *argument = (char*)fncCallData->Parameters[pn]->constantData;
                        char *quoteless = (char*)chp(malloc(strlen(argument)));
                        memcpy(quoteless, argument, strlen(argument));
                        quoteless += 1;
                        quoteless[strlen(argument) - 2] = '\0';
                        printf("%s\n", quoteless);
                    } else {
                        assert(false && "Not implemented");
                    }
                }
            } else {
                printf("Unknown function: %s\n", fncCallData->FunctionToCall);
            }
        }
    }
}