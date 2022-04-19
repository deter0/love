#include "log.h"
#include "tokenizer.h"
#include "ast.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

#define NOT_IMPLEMENTED assert(false && "Not implemented")

bool CheckASTMethodDataType(ASTMethod *method, const char *expectedDataType) {
	if (method->NodeData == NULL) {
		return false;
	}
	if (((struct ASTMethodDataHL*)method->NodeData)->Type == expectedDataType) {
		return true;
	} else {
		return false;
	}
}

ASTMethod *executionStartNode() {
	ASTMethod *method = (ASTMethod*)chp(malloc(sizeof(ASTMethod)));
	method->NodeType = METHOD_EXECUTION_START;
	struct ASTNodeExecutionStartData *data = (struct ASTNodeExecutionStartData*)chp(malloc(sizeof(struct ASTNodeExecutionStartData)));
	data->Type = "MethodExecutionStartData";
	data->MethodsAllocated = 40;
	data->MethodsLength = 0;
	data->Methods = (ASTMethod**)chp(malloc(sizeof(ASTMethod*)*data->MethodsAllocated));
	
	method->NodeData = (void*)data;
	return method;
}

void executionStartAddMethod(ASTMethod *ASTNodeStartExecution, ASTMethod *to_add) {
	l_assert(CheckASTMethodDataType(ASTNodeStartExecution, "MethodExecutionStartData"), "Invalid data");
	struct ASTNodeExecutionStartData *Data = (struct ASTNodeExecutionStartData*)ASTNodeStartExecution->NodeData;
	if (Data->MethodsLength + 1 > Data->MethodsAllocated) {
		Data->MethodsAllocated *= 2;
		Data->Methods = chp(realloc(Data->Methods, sizeof(ASTMethod*) * Data->MethodsAllocated));
	}
	Data->Methods[Data->MethodsLength] = to_add;
	Data->MethodsLength++;
}

#define VARIABLE_TYPE "MethodDeclareVariable"
ASTMethod *declareVariableNode() {
	ASTMethod *method = (ASTMethod*)chp(malloc(sizeof(ASTMethod)));
	method->NodeType = METHOD_DECLARE_VARIABLE;
	struct ASTNodeDeclareVariableData *data = (struct ASTNodeDeclareVariableData*)chp(malloc(sizeof(struct ASTNodeDeclareVariableData )));
	data->Type = VARIABLE_TYPE;
	data->DataType = "NULL";
	data->Data = (void*)NULL;
	data->Name = "";
	method->NodeData = (void*)data;
	return method;
}
void dumpVariableData(FILE *fd, ASTMethod *variable_method) {
	l_assert(CheckASTMethodDataType(variable_method, VARIABLE_TYPE), "Invalid data");
	struct ASTNodeDeclareVariableData *data = variable_method->NodeData;
	fprintf(fd, "Declare Variable: \"%s\"\n", data->Name);
	fprintf(fd, "    Data Type: `%s`\n", data->DataType);
	fprintf(fd, "    Default Value: `%s`\n", data->DataStr);
}

#define FUNCTION_CALL "MethodFunctionCall"
ASTMethod *functionCallNode() {
	ASTMethod *method = (ASTMethod*)chp(malloc(sizeof(ASTMethod)));
	method->NodeType = METHOD_CALL_FUNCTION;
	struct ASTNodeCallFunctionData *data = (struct ASTNodeCallFunctionData*)chp(malloc(sizeof(struct ASTNodeCallFunctionData)));
	data->Type = FUNCTION_CALL;
	data->FunctionToCall = "NULL";
	method->NodeData = (void*)data;
	return method;
}

ASTMethod *constructAST(TokenPool *pool) {
	ASTMethod *ExecutionStart = executionStartNode();
	Token *token, *next, *nnext;
	for (size_t i = 0; i < pool->Length - 1; i++) {
		token = pool->Tokens[i];
		if (token->Type <= TOKEN_TYPE_INT && token->Type >= TOKEN_TYPE_VOID) {
			printf("Is type, %s\n", get_value(token->ParseResult));
			next = pool->Tokens[i + 1]; // !FIXME: Out of bounds read maybe
			if (next->Type == TOKEN_WORD) {
				nnext = pool->Tokens[i + 2];
				if (nnext->Type == TOKEN_OP_EQ) {
					Token *initalValue = pool->Tokens[i + 3];
					ASTMethod *variable = declareVariableNode();
					struct ASTNodeDeclareVariableData *data = (struct ASTNodeDeclareVariableData*)variable->NodeData;
					data->Name = get_value(next->ParseResult);
					data->DataType = get_value(token->ParseResult);
					data->DataStr = get_value(initalValue->ParseResult);
					if (token->Type == TOKEN_TYPE_INT) { // TODO: Evaluate expression assignment
						const char *num = data->DataStr;
						char *end;
						long value = strtol(num, &end, 10);
						if (end == num || *end != '\0' || errno == ERANGE) {
							set_error_cat("[PRE TYPE CHECKING TYPE ERROR]");
							fprintf(stderr, "Error casting `%s` to int.", data->DataStr);
							// fprintf(stderr, "Line: %d\n", get_line()) // TODO: log Lines
							panic(true);
						}
						data->Data = (void*)&value;
						executionStartAddMethod(ExecutionStart, variable);
						i += 5;
					} else {
						NOT_IMPLEMENTED;
					}
				} else if (nnext->Type == TOKEN_SEMICOLON) {
					l_assert(false, "Uninitalized variable not implemented");
				} else if (nnext->Type == TOKEN_OPEN_PAREN) {
					l_assert(false, "Not Implemented");
					// TODO: Load function decleration data
					i += 3;
					TokenPool *params_raw = tokenPool(5 * 3);
					uint32_t z = i;
					uint32_t d = 0;
					while (z < pool->Length - 1) {
						addToken(params_raw, pool->Tokens[z]);
						if (pool->Tokens[z]->Type == TOKEN_OPEN_PAREN) {
							d++;
						} else if (pool->Tokens[z]->Type == TOKEN_CLOSE_PAREN) {
							if (d > 0) {
								d--;
							} else {
								break;
							}
						}
						z++;
					}
					// TODO: Construct parameters
					TokenPool2D *params_split = tokenPoolSplit(params_raw, TOKEN_COMMA);
					// printf("%ld\n", params_split->Length);
					printf("Function decleration:\n\tName: `%s`\n\tParameters:%s\n", get_value(next->ParseResult), TERM_GREEN());
					logTokenPool2D(stdout, params_split);
				} else {
					set_error_cat("[CONSTRUCT AST]");
					fprintf(stderr, "Expected <TOKEN_OP_EQ, TOKEN_SEMICOLON, TOKEN_OPEN_PAREN> after: `%s = !`, got %s.\n", get_value(token->ParseResult), TOKEN_STRINGS[nnext->Type]);
					panic(true);
				}
			}
		} else if (token->Type == TOKEN_WORD) {
			next = pool->Tokens[i + 1];
			nnext = pool->Tokens[i + 1];
			if (next->Type == TOKEN_OPEN_PAREN) { // Function Call
				fprintf(stdout, "Detected function call\n");
        // find closing parenetheesis
        int d = 0;
        TokenPool *arguments = tokenPool(15);
        size_t j;
        for (j = i + 2; j < pool->Length; j++) {
          addToken(arguments, pool->Tokens[j]);
          if (pool->Tokens[j]->Type == TOKEN_OPEN_PAREN)
            d++;
          else if (pool->Tokens[j]->Type == TOKEN_CLOSE_PAREN) {
            if (d <= 0) {
              break;
            } else {
              d--;
            }
          }
          if (j == pool->Length - 1) {
            set_error_cat("[GENERATE AST]");
            fprintf(stderr, "Expected to close call to function `%s` with `)`\n", get_value(token->ParseResult));
            panic(true);
          }
        }
        TokenPool2D *arguments_parsed = tokenPoolSplit(arguments, TOKEN_COMMA);
        printf("Count: %ld\n", arguments->Length);
        logTokenPool2D(stdout, arguments_parsed);
        // logTokenPool(stdout, arguments);
				panic(false);
			}
		} else {
			set_error_cat("[CONSTRUCT AST]");
			fprintf(stderr, "Unexpected token `%s`\n", TOKEN_STRINGS[token->Type]);
			panic(true);
		}
	}
#ifdef DEBUG
	printf("\n");
	struct ASTNodeExecutionStartData *data = (struct ASTNodeExecutionStartData*)ExecutionStart->NodeData;
	for (size_t i = 0; i < data->MethodsLength; i++) {
		struct ASTMethodDataHL *methodData = (struct ASTMethodDataHL*)(data->Methods[i]->NodeData);
		printf("Method: `%s`\n", methodData->Type);
		if (CheckASTMethodDataType(data->Methods[i], VARIABLE_TYPE)) {
			dumpVariableData(stdout, data->Methods[i]);
		}
	}
#endif
	return ExecutionStart;
}
