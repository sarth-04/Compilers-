// token.h
#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    // Keywords
    TK_WITH, TK_PARAMETERS, TK_END, TK_WHILE, TK_UNION,
    TK_ENDUNION, TK_DEFINETYPE, TK_AS, TK_TYPE, TK_MAIN,
    TK_GLOBAL, TK_PARAMETER, TK_LIST, TK_INPUT, TK_OUTPUT,
    TK_INT, TK_REAL, TK_ENDWHILE, TK_IF, TK_THEN, TK_ENDIF,
    TK_READ, TK_WRITE, TK_RETURN, TK_CALL, TK_RECORD,
    TK_ENDRECORD, TK_ELSE,
    
    // Operators
    TK_ASSIGNOP, TK_PLUS, TK_MINUS, TK_MUL, TK_DIV,
    TK_AND, TK_OR, TK_NOT, TK_LT, TK_LE, TK_EQ,
    TK_GT, TK_GE, TK_NE,
    
    // Delimiters
    TK_SQL, TK_SQR, TK_COMMA, TK_SEM, TK_COLON,
    TK_DOT, TK_OP, TK_CL,
    
    // Complex tokens
    TK_ID, TK_NUM, TK_RNUM, TK_FUNID, TK_RUID,
    TK_FIELDID, TK_COMMENT
} TokenType;

typedef struct {
    TokenType type;
    char* lexeme;
    int line;
    int column;
} Token;

#endif
