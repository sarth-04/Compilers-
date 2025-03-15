#include "parserDef.h"
#include "lexerDef.h"
#include <stdbool.h>
#include <stdio.h>

// Function declarations for parser construction
TokenType string_to_terminal(const char* str);
VariableType string_to_variable(const char* str);
Symbol* construct_symbol(char* str);
SymbolList* construct_symbol_list_from_string(char* str);
Grammar* getGrammarFromFile(char** file_name);

// Helper functions for symbol manipulation
Symbol* copySymbol(Symbol* original);
SymbolList* createEmptySymbolList();
bool symbolEquals(Symbol* a, Symbol* b);
bool symbolListContains(SymbolList* list, Symbol* sym);
void addSymbol(SymbolList* list, Symbol* sym);
Symbol* createTerminalSymbol(TokenType token);
Symbol* createEpsilonSymbol();
bool isEpsilon(Symbol* sym);
bool containsEpsilon(SymbolList* list);
void freeSymbolList(SymbolList* list);
bool isTerminal(Symbol* sym);
bool isVariable(Symbol* sym);

// Functions for computing FIRST and FOLLOW sets
SymbolList* ComputeFirstSet(Grammar* grammar, Symbol* variable);
SymbolList* ComputeFollowSet(Grammar* grammar, Symbol* variable);
FirstAndFollow* ComputeFirstAndFollowSet(Grammar* grammar);
SymbolList* computeFirstOfProduction(SymbolList* production);

// Parse table creation and management
SymbolList*** createParseTable(Grammar* grammar, FirstAndFollow ff);
void print_parse_table(SymbolList*** parse_table);

// Parse tree construction and manipulation
ParseTreeNode* createParseTreeNode(Symbol* symbol, ParseTreeNode* parent, Token* token);
Symbol* createVariableSymbol(VariableType type);

// Parsing functions
ParseTreeNode* parseInputSourceCode(twinBuffer *B, int fd, SymbolList*** parseTable, FirstAndFollow ff);

// Utility functions
char* terminal_or_token_to_string(TokenType terminal_type);
char* variable_to_string(VariableType variable_type);
bool token_present_in_set(SymbolList* set, TokenType tok_type);

// Parse tree printing functions
void PrintParseTree(ParseTreeNode* root, char* outfile);
void printParseTreeHelper(ParseTreeNode* node, FILE* f);
void print_symbol_list(SymbolList* sym_list);