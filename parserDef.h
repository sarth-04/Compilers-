#ifndef PARSERDEF_H
#define PARSERDEF_H

#define NONTERMINALS 52
#define TERMINALS 57
#define TERMS_SIZE 111
#define GRAMMAR_SIZE 91
#define EPLS 110
#define STACK_SIZE 200
#define SYN -2

typedef struct parseTree *parseTree;
struct parseTree
{
    char lexeme[30];
    parseTree *child;
    parseTree parent;
    int current;
    int sizeofChild;
    int lineNo;
    char tokenName[30];
    char valueIfNumber[30]; // Value if it's a number (e.g., integer or real)
    char symbol[30];        // Grammar symbol of this node
};
struct FirstAndFollow
{
    int **first;
    int **follow;
};

struct Stack
{
    int top;
    parseTree data[STACK_SIZE];
};
typedef struct Stack Stack;

struct grammar
{
    int **Grammar;
};

struct Table
{
    int **parserTable;
};



typedef struct Table *table;
typedef struct grammar *grammar;
typedef struct FirstAndFollow *FirstAndFollow;

static char *grammarTerms[111] = {
    "$",                         // 0
    "program",                   // 1
    "otherFunctions",            // 2
    "mainFunction",              // 3
    "stmts",                     // 4
    "function",                  // 5
    "input_par",                 // 6
    "output_par",                // 7
    "parameter_list",            // 8
    "dataType",                  // 9
    "primitiveDataType",         // 10
    "constructedDataType",       // 11
    "A",                         // 12
    "typeDefinitions",           // 13
    "declarations",              // 14
    "otherStmts",                // 15
    "returnStmt",                // 16
    "typeDefinition",            // 17
    "definetypestmt",            // 18
    "fieldDefinitions",          // 19
    "fieldDefinition",           // 20
    "moreFields",                // 21
    "global_or_not",             // 22
    "stmt",                      // 23
    "assignmentStmt",            // 24
    "iterativeStmt",             // 25
    "conditionalStmt",           // 26
    "ioStmt",                    // 27
    "funCallStmt",               // 28
    "SingleOrRecId",             // 29
    "arithmeticExpression",      // 30
    "remaining_list",            // 31
    "option_single_constructed", // 32
    "oneExpansion",              // 33
    "moreExpansions",            // 34
    "outputParameters",          // 35
    "inputParameters",           // 36
    "idList",                    // 37
    "booleanExpression",         // 38
    "factorConditionalStmt",     // 39
    "var",                       // 40
    "arith_term",                // 41
    "arithmetic_recursion",      // 42
    "operator",                  // 43
    "factor",                    // 44
    "term_recursion",            // 45
    "operator_second",           // 46
    "declaration",               // 47
    "logicalOp",                 // 48
    "relationalOp",              // 49
    "optionalReturn",            // 50
    "more_ids",                  // 51
    "booleanTerm",               // 52
    "TK_COMMENT",                // 53
    "TK_FIELDID",                // 54
    "TK_ID",                     // 55
    "TK_NUM",                    // 56
    "TK_RNUM",                   // 57
    "TK_FUNID",                  // 58
    "TK_RUID",                   // 59
    "TK_WITH",                   // 60
    "TK_PARAMETERS",             // 61
    "TK_END",                    // 62
    "TK_WHILE",                  // 63
    "TK_UNION",                  // 64
    "TK_ENDUNION",               // 65
    "TK_DEFINETYPE",             // 66
    "TK_AS",                     // 67
    "TK_TYPE",                   // 68
    "TK_MAIN",                   // 69
    "TK_GLOBAL",                 // 70
    "TK_PARAMETER",              // 71
    "TK_LIST",                   // 72
    "TK_SQL",                    // 73
    "TK_SQR",                    // 74
    "TK_INPUT",                  // 75
    "TK_OUTPUT",                 // 76
    "TK_INT",                    // 77
    "TK_REAL",                   // 78
    "TK_COMMA",                  // 79
    "TK_SEM",                    // 80
    "TK_COLON",                  // 81
    "TK_DOT",                    // 82
    "TK_ENDWHILE",               // 83
    "TK_OP",                     // 84
    "TK_CL",                     // 85
    "TK_IF",                     // 86
    "TK_THEN",                   // 87
    "TK_ENDIF",                  // 88
    "TK_READ",                   // 89
    "TK_WRITE",                  // 90
    "TK_RETURN",                 // 91
    "TK_PLUS",                   // 92
    "TK_MINUS",                  // 93
    "TK_MUL",                    // 94
    "TK_DIV",                    // 95
    "TK_CALL",                   // 96
    "TK_RECORD",                 // 97
    "TK_ENDRECORD",              // 98
    "TK_ELSE",                   // 99
    "TK_AND",                    // 100
    "TK_OR",                     // 101
    "TK_NOT",                    // 102
    "TK_LT",                     // 103
    "TK_LE",                     // 104
    "TK_EQ",                     // 105
    "TK_GT",                     // 106
    "TK_GE",                     // 107
    "TK_NE",                     // 108
    "TK_ASSIGNOP",               // 109
    "eps"                        // 110
};

#endif