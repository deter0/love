#include "tokenizer.h"
#include "parser.h"
#include "log.h"

#include <string.h>

TokenPool *tokenPool(size_t initalAllocation) {
	TokenPool *tokenPool = (TokenPool*)chp(malloc(sizeof(TokenPool)));
	tokenPool->Allocated = initalAllocation;
	tokenPool->Tokens = (Token**)chp(calloc(1, sizeof(Token*) * tokenPool->Allocated));
	tokenPool->Length = 0;
	return tokenPool;
}
void addToken(TokenPool *pool, Token *to_add) {
	if (pool->Length + 1 >= pool->Allocated) {
		pool->Allocated = pool->Allocated * 1.5 + 10;
		pool->Tokens = (Token **)chp(realloc(pool->Tokens, sizeof(Token*) * pool->Allocated));
	}
	pool->Tokens[pool->Length] = to_add;
	pool->Length++;
}
char *indent = "";
void logTokenPool(FILE *fd, TokenPool *self) {
	fprintf(fd, "%s%sTokenPool%s {\n", indent, TERM_CYAN, TERM_DEFAULT);
	for (size_t i = 0; i < self->Length; i++) {
		char *val = get_value(self->Tokens[i]->ParseResult);
		if (!strcmp(val, "\n")) {
			val = "<NEW LINE>";
		}
		fprintf(fd,
			"\t%sToken %s<%s>%s: %s\"%s\"%s,\n",
			indent, TERM_RED,
			TOKEN_STRINGS[self->Tokens[i]->Type], TERM_DEFAULT, TERM_GREEN,
			get_value(self->Tokens[i]->ParseResult), TERM_DEFAULT
		);
	}
	fprintf(fd, "%s}\n", indent);
}
void logTokenPool2D(FILE *fd, TokenPool2D *self) {
	fprintf(fd, "%sTokenPool2D%s [\n", TERM_CYAN, TERM_DEFAULT);
	indent = "\t";
	for (size_t i = 0; i < self->Length; i++) {
		logTokenPool(fd, self->TokenPools[i]);
	}
	indent = "";
	fprintf(fd, "]\n");
}

TokenPool2D *tokenPool2D(size_t initalAllocation) {
	TokenPool2D *tokenPool2D = (TokenPool2D*)chp(malloc(sizeof(TokenPool2D)));
	tokenPool2D->Allocated = initalAllocation;
	tokenPool2D->TokenPools = (TokenPool**)chp(calloc(1, sizeof(TokenPool*) * tokenPool2D->Allocated));
	tokenPool2D->Length = 0;
	return tokenPool2D;
}
void addToken2D(TokenPool2D *pool, TokenPool *to_add) {
	if (pool->Length + 1 >= pool->Allocated) {
		pool->Allocated = pool->Allocated * 1.5 + 10;
		pool->TokenPools = (TokenPool **)chp(realloc(pool->TokenPools, sizeof(TokenPool*) * pool->Allocated));
	}
	pool->TokenPools[pool->Length] = to_add;
	pool->Length++;
}

TokenPool2D *tokenPoolSplit(TokenPool *pool, TokenType Delimeter) {
	TokenPool2D *result = tokenPool2D(pool->Allocated / 4);
	uint32_t Last = 0;
	for (size_t i = 0; i < pool->Length; i++) {
		if (pool->Tokens[i]->Type == Delimeter || i == pool->Length-1) {
			TokenPool *split = tokenPool(i - Last);
			for (size_t j = Last; j < i; j++) {
				addToken(split, pool->Tokens[j]);
			}
			addToken2D(result, split);
			Last = i + 1;
		}
	}
	return result;
}

TokenPool *Tokenize(parse_result_pool *parsed) {
	TokenPool *tokenPool = (TokenPool*)chp(calloc(1, sizeof(TokenPool)));
	tokenPool->Allocated = 200;
	tokenPool->Tokens = (Token**)chp(calloc(1, sizeof(Token) * tokenPool->Allocated));
	tokenPool->Length = 0;
	
	for (size_t i = 0; i < parsed->length; ++i) {
		char *val = get_value(parsed->results[i]);
		Token *tkn = (Token*)chp(malloc(sizeof(Token)));
		tkn->Type = TOKEN_UNDEFINED;
		if (!strcmp("return", val)) {
			tkn->Type = TOKEN_KEYWORD_RETURN;
		} else if (!strcmp("int", val)) {
			tkn->Type = TOKEN_TYPE_INT;
		} else if (!strcmp("void", val)) {
			tkn->Type = TOKEN_TYPE_VOID;
		} else if (val[0] == '"' && val[strlen(val)-1] == '"') {
			tkn->Type = TOKEN_STRING;
		} else if (val[0] == '\'' && val[strlen(val)-1] == '\'') {
			tkn->Type = TOKEN_CHAR;
		} /*else if (!strcmp("\n", val)) {
			tkn->Type = TOKEN_NEW_LINE;
		} */else if (!strcmp(";", val)) {
			tkn->Type = TOKEN_SEMICOLON;
		} else if (!strcmp("{", val)) {
			tkn->Type = TOKEN_OPEN_SCOPE;
		} else if (!strcmp("}", val)) {
			tkn->Type = TOKEN_CLOSE_SCOPE;
		} else if (!strcmp("(", val)) {
			tkn->Type = TOKEN_OPEN_PAREN;
		} else if (!strcmp(")", val)) {
			tkn->Type = TOKEN_CLOSE_PAREN;
		} else if (!strcmp("=", val)) {
			if ((i + 1) <= parsed->length) {
				char *next = get_value(parsed->results[i + 1]);
				if (!strcmp(next, "-")) {
					tkn->Type = TOKEN_COMPARE;
					parsed->results[i]->length++;
					i += 1;
				} else {
					tkn->Type = TOKEN_OP_EQ;
				}
			}
			tkn->Type = TOKEN_OP_EQ;
		} else if (!strcmp("-", val)) {
			tkn->Type = TOKEN_OP_SUB;
		} else if (!strcmp("+", val)) {
			tkn->Type = TOKEN_OP_ADD;
		} else if (!strcmp("*", val)) {
			tkn->Type = TOKEN_OP_MUL;
		} else if (!strcmp("/", val)) {
			if ((i + 1) <= parsed->length) {
				char *next = get_value(parsed->results[i + 1]);
				if (!strcmp("/", next)) {
					tkn->Type = TOKEN_COMMENT;
					parsed->results[i]->length++;
					i++;
				}
			} else {
				tkn->Type = TOKEN_OP_DIV;
			}
		} else if (!strcmp(",", val)) {
			tkn->Type = TOKEN_COMMA;
		} else if (!strcmp(">", val)) {
			tkn->Type = TOKEN_OP_MORE_THAN;
		} else if (!strcmp("<", val)) {
			if ((i + 1) <= parsed->length) {
				char *next = get_value(parsed->results[i + 1]);
				if (!strcmp("~", next)) {
					tkn->Type = TOKEN_TOGGLE_HIGH_LEVEL;
					parsed->results[i]->length++;
					i++;
				} else {
					tkn->Type = TOKEN_OP_LESS_THAN;
				}
			} else {
				tkn->Type = TOKEN_OP_LESS_THAN;
			}
		} else if (!strcmp("~", val)) {
			if ((i + 1) <= parsed->length) {
				char *next = get_value(parsed->results[i + 1]);
				if (!strcmp(next, ">")) {
					tkn->Type = TOKEN_INTERN;
					parsed->results[i]->length += 1;
					i++;
				} else {
					set_error_cat("[TOKENIZER]");
					fprintf(stderr, "Invalid token '~' on line %ld. Expected '>' got '%s'.\n",
									get_line(parsed->src, get_index(parsed, parsed->results[i])),
									next
								);
					panic(true);
				}
			}
		} else {
			tkn->Type = TOKEN_WORD;
		}
		tkn->ParseResult = parsed->results[i];
		if (tkn->Type != TOKEN_UNDEFINED) {
			if (tokenPool->Length > 0 && tokenPool->Tokens[tokenPool->Length - 1]->Type == TOKEN_COMMENT && tkn->Type != TOKEN_NEW_LINE) {
				continue;
			}
#ifdef DEBUG
			printf("Added token: <%s> : '%s'\n", TOKEN_STRINGS[(int)tkn->Type], get_value(parsed->results[i]));
#endif
			addToken(tokenPool, tkn);
		} else {
			free(tkn);
		}
	}
	Token *eof = (Token*)chp(malloc(sizeof(Token)));
	eof->ParseResult = (parse_result*)chp(malloc(sizeof(parse_result)));
	eof->ParseResult->is_end = true;
	eof->ParseResult->length = 1;
	eof->ParseResult->ptr_start = parsed->src + strlen(parsed->src-1);
	eof->Type = TOKEN_EOF;
	addToken(tokenPool, eof);
	return tokenPool;
}