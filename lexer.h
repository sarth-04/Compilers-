#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <stdio.h>

typedef struct {
    char* source; // pointer to the input source code 
    int position; // current character being processed
    int line;
    int column;
    int length; // Total length of the source code
} Lexer;

Lexer* create_lexer(const char* source);
void destroy_lexer(Lexer* lexer);
Token* get_next_token(Lexer* lexer);
void destroy_token(Token* token);

#endif