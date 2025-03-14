#include "lexerDef.h" // TODO: reconcile differences with actual lexerDef.h
#define NUM_VARIABLES 53
// #define NUM_RULES 54 
#define NUM_TERMINALS 57
// Special tokens for end of input and epsilon
#define EPSILON_TOKEN -1    
#define DOLLAR_TOKEN  -2 
#define SYN_SET -3  

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
typedef struct grammar 
{
	VariableRule** vars_and_rules;
} Grammar;

// Stores all the rules associated with a particular variable
typedef struct variable_rules 
{
	VariableType variable;
	int num_rules;
	SymbolList** variable_rules; // A 2-D array, containing all possible RHSs of a particular variable
} VariableRule;


typedef struct SymbolList
{
	int length;
	Symbol** symbol_list; // List of pointers to symbols, so **
} SymbolList;

typedef enum SymbolType
{
	SYMBOL_TYPE_TERMINAL,
	SYMBOL_TYPE_VARIABLE,
	// SYN
} SymbolType;

typedef union type
{
	TokenType terminal;
	VariableType non_terminal;
} Type;

typedef union Symbol
{
	SymbolType type;
	Type data;
} Symbol;


typedef struct FirstAndFollow
{
	SymbolList** first_sets; // Array of pointers to SymbolLists, raw value of variable acts as index for 
	SymbolList** follow_sets; // Array of pointers to SymbolLists
} FirstAndFollow;

// typedef struct ParseTable
// {
	
// } ParseTable;

// A node in the parse tree.
typedef struct ParseTreeNode {
	Symbol* symbol;                // The grammar symbol (non-terminal or terminal) at this node.
	Token* token;                  // The token from the lexer (only applicable for terminal nodes).
	int numChildren;               // Number of children nodes.
	ParseTreeNode** children;  // Array of pointers to child nodes.
} ParseTreeNode;