#pragma once

#include "log.h"
#include "reader.h"
#include "tokenizer.h"

typedef enum ASTNodeType {
	METHOD_EXECUTION_START,
	METHOD_DECLARE_VARIABLE,
	METHOD_CALL_FUNCTION
} ASTNodeType;
struct ASTMethodDataHL {
	const char *Type;
};
typedef struct ASTMethod {
	ASTNodeType NodeType;
	void *NodeData; // Data should have Type: const char*
} ASTMethod;

bool CheckASTMethodDataType(ASTMethod *method, const char *expectedDataType);

struct ASTNodeExecutionStartData {
	const char *Type;
	ASTMethod **Methods;
	size_t MethodsLength;
	size_t MethodsAllocated;
};

struct ASTNodeDeclareVariableData {
	const char *Type;
	const char *Name;
	const char *DataType;
	void *Data;
	const char *DataStr;
};

struct ASTNodeValue {
	const char *Type;
	// It can be a reference to a variable example:
	// foo(x)
	bool isReference;
	const char *variableReference;
	// Or it can be a constant for example:
	// foo(4)
	// foo("Buzz")
	bool isConstant;
	const char *constantType;
	void *constantData;
};

struct ASTNodeCallFunctionData {
	const char *Type;
	const char *FunctionToCall;
	struct ASTNodeValue **Parameters;
	size_t ParameterCount;
	size_t ParameterAllocated;
	// TODO: Parameters
};

struct ASTNodeValue *parseValue(TokenPool *tokens, size_t from);
ASTMethod *constructAST(TokenPool *pool);

#define NODE_VALUE "ASTNodeValue"
#define FUNCTION_CALL "MethodFunctionCall"
#define VARIABLE_TYPE "MethodDeclareVariable"