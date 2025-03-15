#include "parser.h"
#include "parserDef.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stack.h>

// Functions to construct grammar from .txt file

TokenType string_to_terminal(const char* str) {
	// Map is defined in parserDef.h
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
	new_list->length = i;// Last iter does not add to list of symbols
	new_list->symbol_list = malloc(sizeof(Symbol) * (i+1)); // One extra 
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
	grammar->vars_and_rules = malloc(sizeof(VariableRule*) * NUM_VARIABLES);
	FILE *fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	Symbol* Variable;
	char* rhs_string; // Temporary string to store rhs of a particular rule
	SymbolList* RHS;
	fp = fopen(*file_name, "r");
	if (fp == NULL)
	{
		printf("Couldn't open grammar file\n");
		return NULL;
	}

	SymbolList** temp_rules_list = malloc(sizeof(SymbolList*) * 5); // Temporarily store all prodn. rules for a variable
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
bool isVariable(Symbol* sym)
{
	return (sym->type == SYMBOL_TYPE_VARIABLE);
}

// Will not lead to infinite recursion unless grammar has left recursion(cyclic dependencies)
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

SymbolList* ComputeFollowSet(Grammar* grammar, Symbol* variable)
{
	SymbolList* follow = createEmptySymbolList();

	// FOLLOW sets are only defined for non-terminals.
	if (variable->type == SYMBOL_TYPE_TERMINAL)
		return follow;

	if (variable->data.non_terminal == 0) // Checking if it's the start symbol
	{
		addSymbol(follow, createTerminalSymbol(DOLLAR_TOKEN));
	}
	
	VariableType target = variable->data.non_terminal;

	// Look through every production in the grammar.
	for (int i = 0; i < NUM_VARIABLES; i++)
	{
		VariableRule* vr = grammar->vars_and_rules[i];
		// LHS of the production.
		Symbol lhs;
		lhs.type = SYMBOL_TYPE_VARIABLE;
		lhs.data.non_terminal = vr->variable;

		for (int r = 0; r < vr->num_rules; r++)
		{
			SymbolList* production = vr->variable_rules[r];

			// Look for an occurrence of the target non-terminal in the production.
			for (int j = 0; j < production->length; j++)
			{
				Symbol* s = production->symbol_list[j];
				if (s->type == SYMBOL_TYPE_VARIABLE &&
					s->data.non_terminal == target)
				{
					bool betaNullable = true;
					// Process symbols following s (i.e. beta).
					for (int k = j + 1; k < production->length; k++)
					{
						Symbol* betaSym = production->symbol_list[k];
						SymbolList* firstBeta = ComputeFirstSet(grammar, betaSym);

						// Add FIRST(betaSym) minus ε to FOLLOW(target).
						for (int m = 0; m < firstBeta->length; m++)
						{
							if (!isEpsilon(firstBeta->symbol_list[m]))
								addSymbol(follow, firstBeta->symbol_list[m]);
						}
						if (!containsEpsilon(firstBeta))
						{
							betaNullable = false;
							break;
						}
					}
					// If there is no beta or beta can derive ε,
					// add FOLLOW(lhs) to FOLLOW(target).
					if (betaNullable)
					{
						SymbolList* followLHS = ComputeFollowSet(grammar, &lhs);
						for (int m = 0; m < followLHS->length; m++)
						{
							addSymbol(follow, followLHS->symbol_list[m]);
						}
					}
				}
			}
		}
	}
	return follow;
}


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
/*
 * For each production A -> α in the grammar (using the global grammar pointer), the
 * following is done:
 *   1. Compute FIRST(α).
 *   2. For every terminal a in FIRST(α) (except for ε), store the production in
 *      table[A][a].
 *   3. If ε is in FIRST(α), then for every terminal b in FOLLOW(A), store the production
 *      in table[A][b].
*/
// 2d array, with each element being a list of Symbols
// [Variable][Terminal]
// The Parse Table, instead of containing A -> B, will only contain 'B'(the RHS)
// Variable can be inferred from the row. i.e. The LHS is simply the variable on top of the stack
SymbolList*** createParseTable(Grammar* grammar, FirstAndFollow ff)
{
	// Allocate a 2D array: rows for non-terminals, columns for terminals.
	SymbolList*** table = malloc(NUM_VARIABLES * sizeof(SymbolList**));
	for (int i = 0; i < NUM_VARIABLES; i++) {
		table[i] = malloc(NUM_TERMINALS * sizeof(SymbolList*));
		for (int j = 0; j < NUM_TERMINALS; j++) {
			table[i][j] = NULL; // Initialize table entries to NULL.
		}
	}
	// For each non-terminal A, get its production rules from the global grammar.
	for (int i = 0; i < NUM_VARIABLES; i++) {
		VariableType A = variable_list[i];
		VariableRule* vr = NULL;
		// Look up the production rules for non-terminal A.
		for (int r = 0; r < NUM_VARIABLES; r++) {
			if (grammar->vars_and_rules[r]->variable == A) {
				vr = grammar->vars_and_rules[r];
				break;
			}
		}
		if (vr == NULL)
			continue;  // No productions found for this non-terminal.

		// For each production A -> α.
		for (int r = 0; r < vr->num_rules; r++) {
			SymbolList* production = vr->variable_rules[r];
			SymbolList* firstProd = computeFirstOfProduction(production);

			// For every terminal in FIRST(α) (except epsilon), set table[A][terminal] = production.
			for (int k = 0; k < firstProd->length; k++) {
				Symbol* term = firstProd->symbol_list[k];
				if (!isEpsilon(term)) {
					int termIndex = term->data.terminal; // Assumes terminals map into [0, NUM_TERMINALS)
					if (termIndex >= 0 && termIndex < NUM_TERMINALS) {
						table[i][termIndex] = production; 
					}
				}
			}
			// If FIRST(α) contains epsilon, for each terminal in FOLLOW(A) add production.
			if (containsEpsilon(firstProd)) {
				SymbolList* followA = ff.follow_sets[A];
				for (int k = 0; k < followA->length; k++) {
					Symbol* term = followA->symbol_list[k];
					int termIndex = term->data.terminal;
					if (termIndex >= 0 && termIndex < NUM_TERMINALS) {
						table[i][termIndex] = production;
					}
				}
			}
			free(firstProd);
		}
		
		// Only now will we dabble in the dark arts of the syn set
		SymbolList* followA = ff.follow_sets[A];
		for (int k = 0; k < followA->length; k++) {
			Symbol* term = followA->symbol_list[k];
			int termIndex = term->data.terminal;
			// If no production was defined for this terminal, add a sync entry.
			if (termIndex >= 0 && termIndex < NUM_TERMINALS && table[i][termIndex] == NULL) {
				SymbolList* syn_prodn = createEmptySymbolList();
				// For example, if createEmptySymbolList doesn't allocate space for symbols, do it here.
				syn_prodn->length = -1; // Using -1 to signal a sync production.
				Symbol* syn_symbol = malloc(sizeof(Symbol));
				syn_symbol->type = SYMBOL_TYPE_TERMINAL;
				syn_symbol->data.terminal = SYN_SET;
				// syn_prodn->symbol_list[0] = syn_symbol; // Incorrect earlier version, no space for pointer
				addSymbol(syn_prodn,syn_symbol);
				table[i][termIndex] = syn_prodn;
			}
		}
	}
	return table;
}


void print_symbol_list(SymbolList* sym_list)
{
	int n = sym_list->length;
	Symbol** list = sym_list->symbol_list;
	Symbol* curr_sym;
	for (int i = 0; i < n; i++)
	{
		curr_sym = list[i];
		if (curr_sym->type==SYMBOL_TYPE_TERMINAL)
		{
			printf("%s,",terminal_or_token_to_string(curr_sym->data.terminal));
		}else
		{
			printf("%s,",variable_to_string(curr_sym->data.non_terminal));
		}
	}
	print(" ");
}

//
void print_parse_table(SymbolList*** parse_table)
{
	SymbolList* curr_list;
	for (int var_num = 0; var_num < NUM_VARIABLES; var_num+=1)
	{
		printf("Current variable: %s\n", variable_to_string(variable_list[var_num]));
		for (int term_num = 0; term_num < NUM_TERMINALS; term_num+=1)
		{
			print("Current terminal: %s\n", terminal_or_token_to_string(mapping[term_num]));
			curr_list = parse_table[var_num][term_num];
			// print all the productions
			print_symbol_list(curr_list);
		}
	}
}

// Helper function, creates Symbols to encapsulate Variables(Non-Terminals)
Symbol* createVariableSymbol(VariableType type)
{
	Symbol* new_symbol = malloc(sizeof(Symbol));
	new_symbol->type = SYMBOL_TYPE_VARIABLE;
	new_symbol->data.non_terminal = type;
	return new_symbol;
}

// Parse Tree construction starts here
ParseTreeNode* createParseTreeNode(Symbol* symbol, ParseTreeNode* parent, Token* token)
{
	// Initialise with no children at first
	ParseTreeNode* new_node = (ParseTreeNode*) (sizeof(ParseTreeNode));
	new_node->symbol = copySymbol(symbol);
	new_node->token = token; // Set to NULL for variables(since NULL is value of third param for variables)
	new_node->parent = parent;
	new_node->children = 0;
	new_node->numChildren = 0;
	return new_node;
}

// Terminal to string
char* terminal_or_token_to_string(TokenType terminal_type)
{
	for (size_t i = 0; i < sizeof(token_map)/sizeof(token_map[0]); i++) {
		if (terminal_type == token_map[i].value) {
			return token_map[i].name;
		}
	}
}
// Variable to string
char* variable_to_string(VariableType variable_type)
{
	if (variable_type < 0)
	{
		printf("Invalid variable to convert to string");
		return NULL;
	}
	return str_variable_list[variable_type];
}
// Checks if token type is present in List of symbols
bool token_present_in_set(SymbolList* set, TokenType tok_type)
{
	Symbol** list = set->symbol_list;
	for (int i = 0; i < set->length; i+=1)
	{
		if (list[i]->data.terminal == tok_type)
		{
			return true;
		}
	}
	return false;
}

// TwinBuffer should be a part of the real lexerDef.h
ParseTreeNode* parseInputSourceCode(TwinBuffer *B, char *testcaseFile, SymbolList*** parseTable, /* for our purposes we assume */ FirstAndFollow ff) {
	// Get the first token.
	Token* tok = getNextToken(B, testcaseFile);
	
	// Create the parse tree stack.
	ParseTreeStack* stack = createStack(10);
	
	// Create and push the dollar (end-of-input) marker.
	ParseTreeNode* dollar_node = createParseTreeNode(createTerminalSymbol(DOLLAR_TOKEN), NULL, NULL);
	push(stack, dollar_node);
	
	// Create the root node for the start symbol (assumed to be variable 0, e.g. "program").
	ParseTreeNode* root = createParseTreeNode(createVariableSymbol(0), NULL, NULL);
	push(stack, root);
	
	ParseTreeNode* top_node = top(stack);
	// Main parsing loop.
	while (!isStackEmpty(stack) && tok != NULL && top(stack) != dollar_node) {
		
		// If the top symbol is epsilon, simply pop it.
		if (isEpsilon(top_node->symbol)) {
			pop(stack);
			continue;
		}
		
		// If the top is a terminal:
		if (top_node->symbol->type == SYMBOL_TYPE_TERMINAL) {
			if (top_node->symbol->data.terminal == tok->type) {
				// Terminal matches the current token.
				top_node->token = tok;  // Link the token to the parse tree node.
				pop(stack);
				tok = getNextToken(B, testcaseFile);
			} else {
				// Terminal mismatch: report error and discard token.
				printf("Error at line %d: Terminal mismatch. Expected %s but found %s. Discarding token.\n",
					   tok->line_no, terminal_or_token_to_string(top_node->token->type), terminal_or_token_to_string(tok->type));
				tok = getNextToken(B, testcaseFile);
			}
			continue;
		}
		
		// At this point, the top of stack is a non-terminal.
		int varIndex = top_node->symbol->data.non_terminal;
		int tokenIndex = tok->type;  // Assumes token types map to indices [0, NUM_TERMINALS)
		
		// Look up the production in the parse table.
		SymbolList* production = parseTable[varIndex][tokenIndex];
		
		// Case 1: Synch production. (Production marked with length == -1)
		if (production != NULL && production->length == -1) {
			printf("Error at line %d: Encountered synch production for non-terminal '%s' with token %s. Popping '%s'.\n",
				   tok->line_no, str_variable_list[varIndex], tokenToString(tok), str_variable_list[varIndex]);
			pop(stack);
			// After popping, we continue parsing
			continue;
		}
		
		// Case 2: No production found. Error state
		if (production == NULL) {
			// Error recovery: discard tokens until one is found in the union of FIRST and FOLLOW sets.
			SymbolList* first_set = ff.first_sets[varIndex];
			SymbolList* follow_set = ff.follow_sets[varIndex];
			while (tok != NULL && !token_present_in_set(first_set,tok->type) && !token_present_in_set(follow_set, tok->type)) {
				printf("Error at line %d: Token %s not in FIRST or FOLLOW set of non-terminal '%s'. Discarding token.\n",
					   tok->line_no, tokenToString(tok), str_variable_list[varIndex]);
				tok = getNextToken(B, testcaseFile);
			}
			if (tok == NULL)
				break;  // End of input.
			
			// Now tok is in the union of FIRST and FOLLOW.
			if (token_present_in_set(follow_set, tok->type)) {
				// Token belongs to FOLLOW: recover by popping the non-terminal.
				printf("Error at line %d: Token %s found in FOLLOW set of '%s'. Popping non-terminal.\n",
					   tok->line_no, tokenToString(tok), str_variable_list[varIndex]);
				pop(stack);
			} else {
				// Token is in FIRST: do not pop non-terminal, resume parsing.
				printf("Error at line %d: Token %s found in FIRST set of '%s'. Resuming parsing without popping non-terminal.\n",
					   tok->line_no, tokenToString(tok), str_variable_list[varIndex]);
			}
			continue; // Retry with the same top-of-stack (or new top if popped) and current token.
		}
		
		// Case 3: Valid production found.
		// Report production application (you may wish to print the right-hand side symbols).
		printf("Applying production for non-terminal '%s' with token %s.\n",
			   str_variable_list[varIndex], terminal_or_token_to_string(tok));
		
		// Pop the non-terminal.
		pop(stack);
		
		// Create parse tree children for the production.
		top_node->numChildren = production->length;
		top_node->children = malloc(production->length * sizeof(ParseTreeNode*));
		// Push production symbols in reverse order (skipping epsilon symbols).
		for (int i = production->length - 1; i >= 0; i--) {
			Symbol* sym = production->symbol_list[i];
			ParseTreeNode* child = createParseTreeNode(sym, top_node, NULL);
			top_node->children[i] = child;
			if (!isEpsilon(sym))
				push(stack, child);
		}
		top_node = top(stack); 
	}
	
	if (tok == NULL ) {
		if ((top_node->symbol->type==SYMBOL_TYPE_VARIABLE) || (top_node->symbol->type == SYMBOL_TYPE_TERMINAL && top_node->symbol->data.terminal != DOLLAR_TOKEN))
		{
			printf("Error: Unexpected end of input.\n");
		}
	}
	
	return root;
}

// Print the parse tree starting from the root.
void PrintParseTree(ParseTreeNode* root, char* outfile) {
	// lexeme CurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol
	FILE* f = (outfile != NULL) ? fopen(outfile, "w") : stdout; // Write to stdout if no file is provided
	if (f == NULL) {
		// Couldn't open file despite being given one
		fprintf(stderr, "Error opening file '%s' for writing!\n", outfile);
		return;
	}
	
	// Print header(easier to see)
	fprintf(f, "%-15s %-15s %-7s %-15s %-15s %-7s %-15s\n", 
			"Lexeme", "NodeSymbol", "LineNo", "TokenName", "ParentSym", "IsLeaf", "NodeType");
	fprintf(f, "-----------------------------------------------------------------------------------------\n");
	
	// Recursively print the tree.
	printParseTreeHelper(root, f);
	
	if (outfile != NULL)
		fclose(f);
}

// Recursive helper to print the parse tree.
void printParseTreeHelper(ParseTreeNode* node, FILE* f) {
	// lexeme CurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol
	if (node == NULL)
		return;
	
	// Determine if the node is a leaf.
	int isLeaf = (node->numChildren == 0);
	
	// Prepare values for printing.
	char* lexeme = "----";
	int lineNo = -1;
	char tokenName[50] = "----";
	
	if (node->symbol->type == SYMBOL_TYPE_TERMINAL) {
		// For terminals, use the token if available.
		if (node->token != NULL) {
			lexeme = node->token->lexeme;      
			lineNo = node->token->line_no;
		}
		// Get the terminal's string representation.
		strcpy(tokenName, terminal_or_token_to_string(node->symbol->data.terminal));
	}
	
	// Determine the NodeSymbol.
	char* nodeSymbol = "----";
	if (node->symbol->type == SYMBOL_TYPE_TERMINAL) {
		nodeSymbol = terminal_or_token_to_string(node->symbol);
	} else if (node->symbol->type == SYMBOL_TYPE_VARIABLE) {
		// Look up non-terminal name from the global array.
		int nt = node->symbol->data.non_terminal;
		nodeSymbol = str_variable_list[nt];
	}
	
	// Determine the parent's symbol.
	char* parentSymbol = "ROOT";
	if (node->parent != NULL) {
		int p_nt = node->parent->symbol->data.non_terminal;
		parentSymbol = str_variable_list[p_nt];
	}
	
	// Determine the node type string.
	char* nodeType = (node->symbol->type == SYMBOL_TYPE_TERMINAL) ? "Terminal" : "Non-Terminal";
	
	bool is_num = (node->token->type == TK_NUM) || ((node->token->type == TK_RNUM));
	
	double val = -1e9;
	// Check if it's a number
	if (node->token != NULL && is_num)
	{
		if (node->token->type == TK_NUM)
		{
			val = (double) atoi(node->token->lexeme);
		}
		else if (node->token->type == TK_RNUM)
		{
			val = (double) atof(node->token->lexeme);
		}
	}
	
	if (is_num)
	{
		// Print current node details.
		fprintf(f, "%-15s %-15s %-7d %-15s %-15lf %-15s %-7s %-15s\n", 
				lexeme, nodeSymbol, lineNo, tokenName, val, parentSymbol, (isLeaf ? "yes" : "no"), nodeType);
		//lexeme CurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol
	}else
	{
		// Print current node details.
		fprintf(f, "%-15s %-15s %-7d %-15s %p %-15s %-7s %-15s\n", 
				lexeme, nodeSymbol, lineNo, tokenName, NULL, parentSymbol, (isLeaf ? "yes" : "no"), nodeType);
		//lexeme CurrentNode lineno tokenName  parentNodeSymbol isLeafNode(yes/no) NodeSymbol
	}
	
	// Recursively print each child.
	for (int i = 0; i < node->numChildren; i++) {
		printParseTreeHelper(node->children[i], f);
	}
}