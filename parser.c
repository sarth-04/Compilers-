#include "parser.h"
#include "parserDef.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_RULES 54 

TokenType string_to_terminal(const char* str) {
	// Define token mapping structure
	typedef struct {
		const char* name;
		TokenType value;
	} TokenMap;

	// Create static lookup table
	static const TokenMap token_map[] = {
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
		{"TK_COMMENT", TK_COMMENT}
	};

	// Search through the map
	for (size_t i = 0; i < sizeof(token_map)/sizeof(token_map[0]); i++) {
		if (strcmp(str, token_map[i].name) == 0) {
			return token_map[i].value;
		}
	}

	// Return invalid token if not found
	return NULL; // Consider defining TK_INVALID in your enum
}

VariableType string_to_variable(const char* str)
{
	int n = NUM_VARIABLES;
	int index = -1;
	for (int i = 0; i < n; i++)
	{
		if (strcmp(str_variable_list[i],str) == 0)
		{
			index = i;
		}
	}
	if (index == -1)
	{
		return NULL; // Couldn't find a corresponding variable
	}else
	{
		return variable_list[index];
	}
}


// TODO: Fix assignments to union type, (how do we assign to a union type?) Rest of fn is okay
Symbol* construct_symbol(char* str)
{
	Symbol* new_symbol = malloc(sizeof(Symbol));
	// First look in variables
	VariableType res = string_to_variable(str);
	if (res == NULL) // Terminal, not variable
	{
		new_symbol->type = SYMBOL_TYPE_TERMINAL;
		new_symbol->data.terminal = (TokenType) string_to_terminal(str); // TODO: Fix this
	}else
	{
		new_symbol->type = SYMBOL_TYPE_VARIABLE;
		new_symbol->data.non_terminal = res; // TODO: Fix this
	}
	return new_symbol;
}

SymbolList* construct_symbol_list_from_string(char* str)
{
	// Take string containing space separated symbols, return array of Symbols
	Symbol** temp_list = malloc(10*sizeof(Symbol));
	char* next_symbol; 
	next_symbol = strtok(str, " ");
	int i = 0;
	while (next_symbol != NULL)
	{
		temp_list[i] = construct_symbol(next_symbol);
		next_symbol = strtok(str, " ");
		i++;
	}
	SymbolList* new_list = malloc(sizeof(SymbolList));
	new_list->length = i - 1;// Last iter does not add to list of symbols
	new_list->symbol_list = malloc(sizeof(Symbol) * (i-1));
	for (int j = 0; j < i; j++)
	{
		new_list->symbol_list[j] = temp_list[j];
	}
	return new_list;
}

Grammar* getGrammarFromFile(char** file_name)
{
	Grammar* grammar;
	int num_rules = 0;
	grammar->vars_and_rules = malloc(sizeof(VariableRule) * NUM_RULES);
	FILE *fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	Symbol* Variable;
	char* rhs_string; // Temporary string to store rhs of a particular rule
	SymbolList* RHS;
	fp = fopen(*file_name, "r");
	if (fp == NULL)
		printf("Couldn't open grammar file\n");exit(1); 

	SymbolList** temp_rules_list = malloc(sizeof(SymbolList) * 5); // Temporarily store all prodn. rules for a variable
	int num_var_rules = 0; // NUmber of productions for one variable
	while ((read = getline(&line, &len, fp)) != -1) {
		// 1. Split at "===>" and get LHS symbol
		Variable = construct_symbol(strtok(read, "===>"));
		// 2. Split RHS at | and construct a rule for each separate RHS
		num_var_rules = 0; // Reset for each variable
		while (rhs_string = strtok(read,"|"))
		{
			RHS = construct_symbol_list_from_string(rhs_string);
			temp_rules_list[num_var_rules] = RHS;
			num_var_rules += 1;
		}
		// 3. Copying from temporary variables to final location 
		VariableRule* new_var_rule = malloc(sizeof(VariableRule));
		new_var_rule->variable = Variable;
		new_var_rule->num_rules = num_var_rules;
		new_var_rule->variable_rules = malloc(sizeof(SymbolList)*num_var_rules);
		for (int i = 0; i < num_var_rules; i++)
		{
			new_var_rule->variable_rules[i] = temp_rules_list[i];
		}
		// 4. Finally, add the variable and its associated productions to grammar 
		grammar->vars_and_rules[num_rules] = new_var_rule; 
	}

	fclose(fp);
	if (line)
		free(line);
	return grammar;
}

FirstAndFollow ComputeFirstAndFollowSet(Grammar G)
{
	
}

void createParseTable(FirstAndFollow f_set, ParseTable T)
{
	
}

ParseTree parseInputSourceCode(char *testcaseFile, ParseTable T)
{
	
}

void PrintParseTree(ParseTree T, char* outfile)
{
	// lexeme CurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol
	
}