#include "lexerDef.h" // TODO: reconcile differences with actual lexerDef.h
// A grammar consists of rules A -> B, where A and B are non-terminals
typedef struct grammar 
{
	Rule* *rules;
	
} Grammar;

typedef struct Rule
{
	SymbolList LHS;
	SymbolList RHS;
} Rule;

typedef struct SymbolList
{
	Symbol* symbol_list;
} SymbolList;

typedef struct Symbol
{
	
} Symbol;

typedef struct FirstAndFollow
{
	
} FirstAndFollow;

typedef struct ParseTable
{
	
} ParseTable;


typedef struct ParseTree
{
	
} ParseTree;