#include "lexerDef.h" // TODO: reconcile differences with actual lexerDef.h
#define NUM_VARIABLES 53
// #define NUM_RULES 54 
#define NUM_TERMINALS 57
// Special tokens for end of input and epsilon
#define EPSILON_TOKEN -1    
#define DOLLAR_TOKEN  -2 
#define SYN_SET -3  
#define TK_ERROR -4


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
	TK_FIELDID, TK_COMMENT, 
	
} TokenType;

TokenType mapping[] = {
	TK_WITH, TK_PARAMETERS, TK_END, TK_WHILE, TK_UNION,
	TK_ENDUNION, TK_DEFINETYPE, TK_AS, TK_TYPE, TK_MAIN,
	TK_GLOBAL, TK_PARAMETER, TK_LIST, TK_INPUT, TK_OUTPUT,
	TK_INT, TK_REAL, TK_ENDWHILE, TK_IF, TK_THEN, TK_ENDIF,
	TK_READ, TK_WRITE, TK_RETURN, TK_CALL, TK_RECORD,
	TK_ENDRECORD, TK_ELSE,
	TK_ASSIGNOP, TK_PLUS, TK_MINUS, TK_MUL, TK_DIV,
	TK_AND, TK_OR, TK_NOT, TK_LT, TK_LE, TK_EQ,
	TK_GT, TK_GE, TK_NE,
	TK_SQL, TK_SQR, TK_COMMA, TK_SEM, TK_COLON,
	TK_DOT, TK_OP, TK_CL,
	TK_ID, TK_NUM, TK_RNUM, TK_FUNID, TK_RUID,
	TK_FIELDID, TK_COMMENT, 
	
	// EPSILON_TOKEN, DOLLAR_TOKEN // Not present
};

typedef struct Token
{
	char* lexeme;
	TokenType type;
	int line_no;

} Token;

// Define token mapping structure
typedef struct TokMap{
	const char* name;
	TokenType value;
} TokMap;

// Create static lookup table
const TokMap token_map[] = {
	// Keywords
	{"TK_WITH", TK_WITH},
	{"TK_PARAMETERS", TK_PARAMETERS},
	{"TK_END", TK_END},
	{"TK_WHILE", TK_WHILE},
	{"TK_UNION", TK_UNION},
	{"TK_ENDUNION", TK_ENDUNION},
	{"TK_DEFINETYPE", TK_DEFINETYPE},
	{"TK_AS", TK_AS},
	{"TK_TYPE", TK_TYPE},
	{"TK_MAIN", TK_MAIN},
	{"TK_GLOBAL", TK_GLOBAL},
	{"TK_PARAMETER", TK_PARAMETER},
	{"TK_LIST", TK_LIST},
	{"TK_INPUT", TK_INPUT},
	{"TK_OUTPUT", TK_OUTPUT},
	{"TK_INT", TK_INT},
	{"TK_REAL", TK_REAL},
	{"TK_ENDWHILE", TK_ENDWHILE},
	{"TK_IF", TK_IF},
	{"TK_THEN", TK_THEN},
	{"TK_ENDIF", TK_ENDIF},
	{"TK_READ", TK_READ},
	{"TK_WRITE", TK_WRITE},
	{"TK_RETURN", TK_RETURN},
	{"TK_CALL", TK_CALL},
	{"TK_RECORD", TK_RECORD},
	{"TK_ENDRECORD", TK_ENDRECORD},
	{"TK_ELSE", TK_ELSE},
	
	// Operators
	{"TK_ASSIGNOP", TK_ASSIGNOP},
	{"TK_PLUS", TK_PLUS},
	{"TK_MINUS", TK_MINUS},
	{"TK_MUL", TK_MUL},
	{"TK_DIV", TK_DIV},
	{"TK_AND", TK_AND},
	{"TK_OR", TK_OR},
	{"TK_NOT", TK_NOT},
	{"TK_LT", TK_LT},
	{"TK_LE", TK_LE},
	{"TK_EQ", TK_EQ},
	{"TK_GT", TK_GT},
	{"TK_GE", TK_GE},
	{"TK_NE", TK_NE},
	
	// Delimiters
	{"TK_SQL", TK_SQL},
	{"TK_SQR", TK_SQR},
	{"TK_COMMA", TK_COMMA},
	{"TK_SEM", TK_SEM},
	{"TK_COLON", TK_COLON},
	{"TK_DOT", TK_DOT},
	{"TK_OP", TK_OP},
	{"TK_CL", TK_CL},
	
	// Complex tokens
	{"TK_ID", TK_ID},
	{"TK_NUM", TK_NUM},
	{"TK_RNUM", TK_RNUM},
	{"TK_FUNID", TK_FUNID},
	{"TK_RUID", TK_RUID},
	{"TK_FIELDID", TK_FIELDID},
	{"TK_COMMENT", TK_COMMENT},
	
	// Epsilon
	{"eps", EPSILON_TOKEN}
};


// Fill in all the non terminals
typedef enum 
{
program, 
mainFunction,
otherFunctions,
function,
input_par,
output_par,
parameter_list,
dataType,
primitiveDatatype,
constructedDatatype,
remaining_list,
stmts,
typeDefinitions,
actualOrRedefined,
typeDefinition,
fieldDefinitions,
fieldDefinition,
fieldtype,
moreFields,
declarations,
declaration,
global_or_not,
otherStmts,
stmt,
assignmentStmt,
singleOrRecId,
option_single_constructed,
oneExpansion,
moreExpansions,
funCallStmt,
outputParameters,
inputParameters,
iterativeStmt,
conditionalStmt,
elsePart,
ioStmt,
arithmeticExpression,
expPrime,
term,
termPrime,
factor,
highPrecedenceOperator,
lowPrecedenceOperators,
booleanExpression,
var,
logicalOp,
relationalOp,
returnStmt,
optionalReturn,
idList,
more_ids,
definetypestmt,
A
} VariableType;

VariableType variable_list[] = 
{
program,
mainFunction,
otherFunctions,
function,
input_par,
output_par,
parameter_list,
dataType,
primitiveDatatype,
constructedDatatype,
remaining_list,
stmts,
typeDefinitions,
actualOrRedefined,
typeDefinition,
fieldDefinitions,
fieldDefinition,
fieldtype,
moreFields,
declarations,
declaration,
global_or_not,
otherStmts,
stmt,
assignmentStmt,
singleOrRecId,
option_single_constructed,
oneExpansion,
moreExpansions,
funCallStmt,
outputParameters,
inputParameters,
iterativeStmt,
conditionalStmt,
elsePart,
ioStmt,
arithmeticExpression,
expPrime,
term,
termPrime,
factor,
highPrecedenceOperator,
lowPrecedenceOperators,
booleanExpression,
var,
logicalOp,
relationalOp,
returnStmt,
optionalReturn,
idList,
more_ids,
definetypestmt,
A
};

char* str_variable_list[] = 
{
"program",
"mainFunction",
"otherFunctions",
"function",
"input_par",
"output_par",
"parameter_list",
"dataType",
"primitiveDatatype",
"constructedDatatype",
"remaining_list",
"stmts",
"typeDefinitions",
"actualOrRedefined",
"typeDefinition",
"fieldDefinitions",
"fieldDefinition",
"fieldtype",
"moreFields",
"declarations",
"declaration",
"global_or_not",
"otherStmts",
"stmt",
"assignmentStmt",
"singleOrRecId",
"option_single_constructed",
"oneExpansion",
"moreExpansions",
"funCallStmt",
"outputParameters",
"inputParameters",
"iterativeStmt",
"conditionalStmt",
"elsePart",
"ioStmt",
"arithmeticExpression",
"expPrime",
"term",
"termPrime",
"factor",
"highPrecedenceOperator",
"lowPrecedenceOperators",
"booleanExpression",
"var",
"logicalOp",
"relationalOp",
"returnStmt",
"optionalReturn",
"idList",
"more_ids",
"definetypestmt",
"A"
};

// Redundant grammar defns.(might be useful later)
// typedef struct RulesList
// {
// 	int num_rules;
// 	Rule** rules;
// } RuleList;

// typedef struct Rule
// {
// 	Symbol* LHS;
// 	SymbolList* RHS;
// } Rule;
// A grammar consists of rules A -> B, where A and B are non-terminals

typedef enum SymbolType
{
	SYMBOL_TYPE_TERMINAL,
	SYMBOL_TYPE_VARIABLE
} SymbolType;

typedef union type
{
	TokenType terminal;
	VariableType non_terminal;
} Type;

typedef struct ParserSymbol
{
	SymbolType type;
	Type data;
} ParserSymbol;

typedef struct SymbolList
{
	int length;
	ParserSymbol** symbol_list; // List of pointers to symbols, so **
} SymbolList;

typedef struct variable_rules 
{
	VariableType variable;
	int num_rules;
	SymbolList** variable_rules; // A 2-D array, containing all possible RHSs of a particular variable
} VariableRule;


typedef struct grammar 
{
	VariableRule** vars_and_rules;
} Grammar;

// Stores all the rules associated with a particular variable




typedef struct FirstAndFollow
{
	SymbolList** first_sets; // Array of pointers to SymbolLists, raw value of variable acts as index for 
	SymbolList** follow_sets; // Array of pointers to SymbolLists
} FirstAndFollow;

// typedef struct ParseTable
// {
	
// } ParseTable;

// A node in the parse tree.
typedef struct parseTreeNode {
	ParserSymbol* symbol;                // The grammar symbol (non-terminal or terminal) at this node.
	Token* token;                  // The token from the lexer (only applicable for terminal nodes).
	struct ParseTreeNode* parent;
	int numChildren;               // Number of children nodes.
	struct ParseTreeNode** children;  // Array of pointers to child nodes.
} ParseTreeNode;