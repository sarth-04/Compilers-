#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEXEME_LENGTH 256

typedef struct {
    const char* lexeme;
    TokenType type;
} Keyword;

// Creating an array of type keyword to store all the <keywords,Token> pairs

static const Keyword keywords[] = {
    {"with", TK_WITH},
    {"parameters", TK_PARAMETERS},
    {"end", TK_END},
    {"while", TK_WHILE},
    {"union", TK_UNION},
    {"endunion", TK_ENDUNION},
    {"definetype", TK_DEFINETYPE},
    {"as", TK_AS},
    {"type", TK_TYPE},
    {"_main", TK_MAIN},
    {"global", TK_GLOBAL},
    {"parameter", TK_PARAMETER},
    {"list", TK_LIST},
    {"input", TK_INPUT},
    {"output", TK_OUTPUT},
    {"int", TK_INT},
    {"real", TK_REAL},
    {"endwhile", TK_ENDWHILE},
    {"if", TK_IF},
    {"then", TK_THEN},
    {"endif", TK_ENDIF},
    {"read", TK_READ},
    {"write", TK_WRITE},
    {"return", TK_RETURN},
    {"call", TK_CALL},
    {"record", TK_RECORD},
    {"endrecord", TK_ENDRECORD},
    {"else", TK_ELSE},
    {NULL, 0} 
};

static char current_char(Lexer* lexer) {
    if (lexer->position >= lexer->length) return '\0';//if we reach the end of the source code file
    return lexer->source[lexer->position];
}

static char peek_char(Lexer* lexer, int offset) {
    if (lexer->position + offset >= lexer->length) return '\0';//if we reach the end of the source code file
    return lexer->source[lexer->position + offset];
}

static void advance(Lexer* lexer) {
    if (lexer->position < lexer->length) {
        if (current_char(lexer) == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        lexer->position++;
    }
}

static void skip_whitespace(Lexer* lexer) {
    while (isspace(current_char(lexer))) {  //using built-in isspace() function
        advance(lexer);
    }
}

static void skip_comment(Lexer* lexer) {
    while (current_char(lexer) != '\0' && current_char(lexer) != '\n') {
        advance(lexer);
    }
    if (current_char(lexer) == '\n') {
        advance(lexer);
    }
}

static Token* create_token(TokenType type, const char* lexeme, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->lexeme = strdup(lexeme);
    token->line = line;
    token->column = column;
    return token;
}

static int is_valid_id_char(char c) {
    return (c >= 'b' && c <= 'd') || (c >= '2' && c <= '7');
}

static int is_valid_identifier(const char* str) {
    if (!str || !*str) return 0;
    
    // First character must be b-d
    if (*str < 'b' || *str > 'd') return 0;
    str++;
    
    // Second character must be 2-7
    if (!*str || *str < '2' || *str > '7') return 0;
    str++;
    
    // Rest can be b-d or 2-7
    while (*str) {
        if (!is_valid_id_char(*str)) return 0;
        str++;
    }
    
    return 1;
}

static TokenType get_keyword_type(const char* lexeme) {
    for (int i = 0; keywords[i].lexeme != NULL; i++) {
        if (strcmp(keywords[i].lexeme, lexeme) == 0) {
            return keywords[i].type;
        }
    }
    return TK_ID;  // Not a keyword
}

// Main lexer functions
Lexer* create_lexer(const char* source) {
    //initialize a lexer
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->source = strdup(source);
    lexer->length = strlen(source);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    return lexer;
}

void destroy_lexer(Lexer* lexer) {
    // freeing up space of the lexer
    if (lexer) {
        free(lexer->source);
        free(lexer);
    }
}

void destroy_token(Token* token) {
    // freeing up space assigned to token structre
    if (token) {
        free(token->lexeme);
        free(token);
    }
}

static Token* get_number_token(Lexer* lexer) {
    char buffer[MAX_LEXEME_LENGTH];
    int i = 0;
    int start_column = lexer->column;
    int is_real = 0;
    
    // Get integer part
    while (isdigit(current_char(lexer))) {
        buffer[i++] = current_char(lexer);
        advance(lexer);
    }
    
    // Check for decimal point
    if (current_char(lexer) == '.') {
        is_real = 1;
        buffer[i++] = current_char(lexer);
        advance(lexer);
        
        // Get decimal part
        int decimal_count = 0;
        while (isdigit(current_char(lexer))) {
            buffer[i++] = current_char(lexer);
            decimal_count++;
            advance(lexer);
        }
        
        if (decimal_count != 2) {
            // Error: Real numbers must have exactly 2 decimal places
            return NULL;
        }
    }
    
    buffer[i] = '\0';
    return create_token(is_real ? TK_RNUM : TK_NUM, buffer, lexer->line, start_column);
}

static Token* get_identifier_token(Lexer* lexer) {
    char buffer[MAX_LEXEME_LENGTH];
    int i = 0;
    int start_column = lexer->column;
    
    // Handle function identifiers
    if (current_char(lexer) == '_') {
        buffer[i++] = current_char(lexer);
        advance(lexer);
        while (isalnum(current_char(lexer))) {
            buffer[i++] = current_char(lexer);
            advance(lexer);
        }
        buffer[i] = '\0';
        return create_token(TK_FUNID, buffer, lexer->line, start_column);
    }
    
    // Handle record/union identifiers
    if (current_char(lexer) == '#') {
        buffer[i++] = current_char(lexer);
        advance(lexer);
        while (islower(current_char(lexer))) {
            buffer[i++] = current_char(lexer);
            advance(lexer);
        }
        buffer[i] = '\0';
        return create_token(TK_RUID, buffer, lexer->line, start_column);
    }
    
    // Handle regular identifiers
    while (is_valid_id_char(current_char(lexer))) {
        buffer[i++] = current_char(lexer);
        advance(lexer);
    }
    buffer[i] = '\0';
    
    // Check if it's a keyword
    TokenType type = get_keyword_type(buffer);
    if (type != TK_ID) {
        return create_token(type, buffer, lexer->line, start_column);
    }
    
    // Validate identifier pattern
    if (is_valid_identifier(buffer)) {
        return create_token(TK_ID, buffer, lexer->line, start_column);
    }
    
    return NULL;  // Invalid identifier
}

Token* get_next_token(Lexer* lexer) {
    while (current_char(lexer) != '\0') {
        // Skip whitespace
        if (isspace(current_char(lexer))) {
            skip_whitespace(lexer);
            continue;
        }
        
        // Skip comments
        if (current_char(lexer) == '%') {
            skip_comment(lexer);
            continue;
        }
        
        // Handle numbers
        if (isdigit(current_char(lexer))) {
            return get_number_token(lexer);
        }
        
        // Handle identifiers and keywords
        if (isalpha(current_char(lexer)) || current_char(lexer) == '_' || current_char(lexer) == '#') {
            return get_identifier_token(lexer);
        }
        
        // Handle operators and special characters
        char curr = current_char(lexer);
        int start_column = lexer->column;
        
        
            if(curr == '<'){
                advance(lexer);
                if (peek_char(lexer, 0) == '-' && 
                    peek_char(lexer, 1) == '-' && 
                    peek_char(lexer, 2) == '-') {
                    advance(lexer);  // skip -
                    advance(lexer);  // skip -
                    advance(lexer);  // skip -
                    return create_token(TK_ASSIGNOP, "<---", lexer->line, start_column);
                } else if (current_char(lexer) == '=') {
                    advance(lexer);
                    return create_token(TK_LE, "<=", lexer->line, start_column);
                }
                return create_token(TK_LT, "<", lexer->line, start_column);
            }
            // Add other operators and special characters here
            
            else{
                //Handle single character tokens
                char buffer[2] = {curr, '\0'};
                advance(lexer);
                return create_token(TK_ID, buffer, lexer->line, start_column);
            }
    }
    
    return NULL;  // End of input
}