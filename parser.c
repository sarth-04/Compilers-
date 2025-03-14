#include "parser.h"
#include "parserDef.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define NUM_RULES 54 
// Special tokens for end of input and epsilon
#define EPSILON_TOKEN -1    
#define DOLLAR_TOKEN  -2    

// Functions to construct grammar from .txt file

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
		{"TK_COMMENT", TK_COMMENT},
		
		// Epsilon
		{"eps", EPSILON_TOKEN}
	};

	// Iterate through the map
	for (size_t i = 0; i < sizeof(token_map)/sizeof(token_map[0]); i++) {
		if (strcmp(str, token_map[i].name) == 0) {
			return token_map[i].value;
		}
	}

	// Return invalid token if not found
	return NULL; 
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


// Return appropriate symbol from string
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
		new_var_rule->variable_rules = malloc(sizeof(SymbolList) * num_var_rules);
		for (int i = 0; i < num_var_rules; i++)
		{
			new_var_rule->variable_rules[i] = temp_rules_list[i];
		}
		// 4. Finally, add the variable and its associated productions to grammar 
		grammar->vars_and_rules[num_rules] = new_var_rule; 
		num_rules += 1;
	}

	fclose(fp);
	if (line)
		free(line);
	return grammar;
}

Symbol* copySymbol(Symbol* original) {
	Symbol* copy = (Symbol*)malloc(sizeof(Symbol));
	copy->type = original->type;
	if (original->type == SYMBOL_TYPE_TERMINAL)
		copy->data.terminal = original->data.terminal;
	else
		copy->data.non_terminal = original->data.non_terminal;
	return copy;
}

// Helper functions for first and follow set computation

// Creates empty symbol list(self-explanatory)
SymbolList* createEmptySymbolList() {
	SymbolList* list = (SymbolList*)malloc(sizeof(SymbolList));
	list->length = 0;
	list->symbol_list = NULL;
	return list;
}


// Checks if two symbols are equal.
bool symbolEquals(Symbol* a, Symbol* b) {
	if (a->type != b->type)
		 return false;
	if (a->type == SYMBOL_TYPE_TERMINAL)
		 return (a->data.terminal == b->data.terminal);
	else
		 return (a->data.non_terminal == b->data.non_terminal);
}

// Returns true if the symbol list already contains the symbol.
bool symbolListContains(SymbolList* list, Symbol* sym) {
	for (int i = 0; i < list->length; i++) {
		 if (symbolEquals(list->symbol_list[i], sym))
			  return true;
	}
	return false;
}

// Adds sym to list if not already present.
void addSymbol(SymbolList* list, Symbol* sym) {
	if (symbolListContains(list, sym))
		 return ;
	list->length++;
	list->symbol_list = realloc(list->symbol_list, list->length * sizeof(Symbol*));
	list->symbol_list[list->length - 1] = copySymbol(sym);
	return ;
}

// Create a new terminal symbol for a given token.
Symbol* createTerminalSymbol(TokenType token) {
	Symbol* s = (Symbol*)malloc(sizeof(Symbol));
	s->type = SYMBOL_TYPE_TERMINAL;
	s->data.terminal = token;
	return s;
}

// Creates a symbol representing ε.
Symbol* createEpsilonSymbol() {
	return createTerminalSymbol(EPSILON_TOKEN);
}

// Returns true if the given symbol represents ε. Small wrapper function to make things easier
bool isEpsilon(Symbol* sym) {
	return (sym->type == SYMBOL_TYPE_TERMINAL && sym->data.terminal == EPSILON_TOKEN);
}

// Returns true if the given symbol list contains ε.
bool containsEpsilon(SymbolList* list) {
	for (int i = 0; i < list->length; i++) {
		 if (isEpsilon(list->symbol_list[i]))
			  return true;
	}
	return false;
}

// Don't just free the list, free the individual symbols contained in them as well
void freeSymbolList(SymbolList* list) {
	if (list) {
		// Free each symbol in the list since they are not shared.
		for (int i = 0; i < list->length; i++) {
			free(list->symbol_list[i]);
		}
		free(list->symbol_list);
		free(list);
	}
}

// Convenience function to detect terminals
bool isTerminal(Symbol* sym)
{
	return (sym->type == SYMBOL_TYPE_TERMINAL);
}

// Convenience function to detect non terminals
bool isTerminal(Symbol* sym)
{
	return (sym->type == SYMBOL_TYPE_VARIABLE);
}

SymbolList* ComputeFirstSet(Grammar* grammar, Symbol* variable)
{
	SymbolList* first_set = (SymbolList*) malloc(sizeof(SymbolList));
	// Base case, terminal includes eps here
	if (isTerminal(variable))
	{
		addSymbol(first_set, copySymbol(variable));
		return first_set;
	}
	// Now we know it's a variable, get production rules for it
	VariableType variable_type = variable->data.non_terminal;
	VariableRule* variable_ruleset = grammar->vars_and_rules[variable_type];
	if (variable_ruleset == NULL)
		return first_set; // no productions found
	int num_prodn = variable_ruleset->num_rules;
	// Now iterate across each production
	for (int production_num = 0; production_num < num_prodn; production_num += 1)
	{
		SymbolList* current_production = variable_ruleset->variable_rules[production_num];
		bool productionNullable = true; // production produces eps
		// Process the production from left to right.
		for (int j = 0; j < current_production->length; j++)
		{
			Symbol* currSym = current_production->symbol_list[j];
			SymbolList* firstOfCurr = ComputeFirstSet(grammar, currSym);

			// Add everything in FIRST(currSym) except ε.
			for (int k = 0; k < firstOfCurr->length; k++)
			{
				if (!isEpsilon(firstOfCurr->symbol_list[k]))
					addSymbol(first_set, firstOfCurr->symbol_list[k]);
			}
			// If currSym’s FIRST does not contain ε, this production cannot derive ε.
			if (!containsEpsilon(firstOfCurr))
			{
				productionNullable = false;
				break;
			}
			// Otherwise, continue with the next symbol.
		}
		// If all symbols in the production can produce ε, add ε to FIRST.
		if (productionNullable)
		{
			Symbol* eps = createEpsilonSymbol();
			addSymbol(first_set, eps);
		}
		
	}
	return first_set;
}

// SymbolList* ComputeFollowSet(Grammar* grammar, Symbol* variable)
// {
// 	SymbolList* follow = createEmptySymbolList();

// 	// FOLLOW sets are only defined for non-terminals.
// 	if (variable->type == SYMBOL_TYPE_TERMINAL)
// 		return follow;

// 	VariableType target = variable->data.non_terminal;

// 	// Look through every production in the grammar.
// 	for (int i = 0; i < NUM_RULES; i++)
// 	{
// 		VariableRule* vr = grammar->vars_and_rules[i];
// 		// LHS of the production.
// 		Symbol lhs;
// 		lhs.type = SYMBOL_TYPE_VARIABLE;
// 		lhs.data.non_terminal = vr->variable;

// 		for (int r = 0; r < vr->num_rules; r++)
// 		{
// 			SymbolList* production = vr->variable_rules[r];

// 			// Look for an occurrence of the target non-terminal in the production.
// 			for (int j = 0; j < production->length; j++)
// 			{
// 				Symbol* s = production->symbol_list[j];
// 				if (s->type == SYMBOL_TYPE_VARIABLE &&
// 					s->data.non_terminal == target)
// 				{
// 					bool betaNullable = true;
// 					// Process symbols following s (i.e. beta).
// 					for (int k = j + 1; k < production->length; k++)
// 					{
// 						Symbol* betaSym = production->symbol_list[k];
// 						SymbolList* firstBeta = ComputeFirstSet(grammar, betaSym);

// 						// Add FIRST(betaSym) minus ε to FOLLOW(target).
// 						for (int m = 0; m < firstBeta->length; m++)
// 						{
// 							if (!isEpsilon(firstBeta->symbol_list[m]))
// 								addSymbol(follow, firstBeta->symbol_list[m]);
// 						}
// 						if (!containsEpsilon(firstBeta))
// 						{
// 							betaNullable = false;
// 							break;
// 						}
// 					}
// 					// If there is no beta or beta can derive ε,
// 					// add FOLLOW(lhs) to FOLLOW(target).
// 					if (betaNullable)
// 					{
// 						SymbolList* followLHS = ComputeFollowSet(grammar, &lhs);
// 						for (int m = 0; m < followLHS->length; m++)
// 						{
// 							addSymbol(follow, followLHS->symbol_list[m]);
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
// 	return follow;
// }


FirstAndFollow* ComputeFirstAndFollowSet(Grammar* grammar)
{
	FirstAndFollow* ff = (FirstAndFollow*) malloc(sizeof(FirstAndFollow));
	// Initialise list of SymbolList pointers
	ff->first_sets = (SymbolList**) malloc(NUM_VARIABLES * sizeof(SymbolList*));
	ff->follow_sets = (SymbolList**) malloc(NUM_VARIABLES * sizeof(SymbolList*));
	// Compute FIRST sets for every non-terminal.
	for (int i = 0; i < NUM_VARIABLES; i++)
	{
		Symbol nonTerm;
		nonTerm.type = SYMBOL_TYPE_VARIABLE;
		nonTerm.data.non_terminal = variable_list[i];
		ff->first_sets[i] = ComputeFirstSet(grammar, &nonTerm);
		ff->follow_sets[i] = createEmptySymbolList();
	}

	// Initialize FOLLOW(start symbol)
	Symbol* dollar = createTerminalSymbol(DOLLAR_TOKEN);
	addSymbol(ff->follow_sets[program], dollar);

	// Iteratively update the FOLLOW sets until no changes occur.
	// Didn't use helper function because it's less efficient
	bool changed = true;
	while (changed)
	{
		changed = false;
		// For every variable in the grammar:
		for (int i = 0; i < NUM_VARIABLES; i++)
		{
			VariableRule* vr = grammar->vars_and_rules[i];
			Symbol lhs;
			lhs.type = SYMBOL_TYPE_VARIABLE;
			lhs.data.non_terminal = vr->variable;
			int lhsIndex = vr->variable;  // Raw value of variable

			// Iterate across all productions for this variable
			SymbolList* production;
			SymbolList* firstOfSymbol;
			for (int r = 0; r < vr->num_rules; r++)
			{
				production = vr->variable_rules[r];
				for (int j = 0; j < production->length; j++)
				{
					Symbol* B = production->symbol_list[j];
					if (B->type == SYMBOL_TYPE_VARIABLE)
					{
						int BIndex = B->data.non_terminal;
						SymbolList* firstBeta = createEmptySymbolList();
						bool betaNullable = true;

						// Compute FIRST(beta) where beta are the symbols after B in the production.
						for (int k = j + 1; k < production->length; k++)
						{
							firstOfSymbol = ComputeFirstSet(grammar, production->symbol_list[k]);
							for (int m = 0; m < firstOfSymbol->length; m++)
							{
								if (!isEpsilon(firstOfSymbol->symbol_list[m]))
									addSymbol(firstBeta, firstOfSymbol->symbol_list[m]);
							}
							if (!containsEpsilon(firstOfSymbol))
							{
								betaNullable = false;
								break;
							}
						}

						int oldLength = ff->follow_sets[BIndex]->length;
						// Add FIRST(beta) (without ε) to FOLLOW(B).
						for (int m = 0; m < firstBeta->length; m++)
						{
							addSymbol(ff->follow_sets[BIndex], firstBeta->symbol_list[m]);
						}
						// If beta is nullable, add FOLLOW(lhs) to FOLLOW(B).
						if (betaNullable)
						{
							for (int m = 0; m < ff->follow_sets[lhsIndex]->length; m++)
							{
								addSymbol(ff->follow_sets[BIndex], ff->follow_sets[lhsIndex]->symbol_list[m]);
							}
						}
						if (ff->follow_sets[BIndex]->length > oldLength)
							changed = true;

						freeSymbolList(firstBeta);
					}
				}
			}
		}
	}
	return ff;
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