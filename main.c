
#include "lexer.h"
#include <stdio.h>

void print_token(Token* token) {
    if (!token) return;
    
    const char* type_strings[] = {
        "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE", "TK_UNION",
        "TK_ENDUNION", "TK_DEFINETYPE", "TK_AS", "TK_TYPE", "TK_MAIN",
        // ... add all token types
    };
    
    printf("Token(type=%s, lexeme='%s', line=%d, column=%d)\n",
           type_strings[token->type], token->lexeme, token->line, token->column);
}

int main() {
    const char* source = 
        "_main\n"
        "type int: b2d567;\n"
        "b2d567 <--- 42;\n"
        "return;\n"
        "end\n";
    
    Lexer* lexer = create_lexer(source);
    Token* token;
    
    while ((token = get_next_token(lexer)) != NULL) {
        print_token(token);
        destroy_token(token);
    }
    
    destroy_lexer(lexer);
    return 0;
}