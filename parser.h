#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "parserDef.h"
#include "lexer.h"

int readBufferParse(int fd, twinBuffer *B);

void freeDataStructures();

// Takes input as G, computes the FIRST and FOLLOW sets
FirstAndFollow ComputeFirstAndFollowSets(grammar G);

// Creates the parser table
void createParseTable(FirstAndFollow F, table T);

// Function to parse the testcase File
parseTree parseInputSourceCode(char *testcaseFile, table T);

// Function to print the parse tree
void printParseTree(parseTree PT, char *outfile);

// Funtion to find the location of the terminal or non terminal in the ter
int find_location(char *term);

// Function to create a new stack
Stack *newStack();

// Function to return the top of the stack
parseTree top(Stack *stack);

// Function to pop the top of the stack
bool pop(Stack *stack);

// Function to check whether the stack is empty or not
bool isEmpty(Stack *stack);

// Function to push the element which is our non terminal into the stack
bool push(Stack *s, parseTree e);

// Function to print all the elements of the stack
void print_stack(Stack *stack);

// Function to create a new parsernode
parseTree createNewNode();

// Function to parse the given token
void parseFile(char *token, char *lexeme, int lineNo, table T, Stack *s, grammar G);

// Recursive function which helps in printing the parse tree
void outputTree(parseTree root, FILE *fd);

// Function to print Grammar
void print_grammar(grammar G);

// Function to print the first set of grammar terms
void print_first_set(FirstAndFollow F, grammar G);

// FUnction to print the follow set of non terminals
void print_follow_set(FirstAndFollow F, grammar G);

// Function to check whether the given term is already present in the set or not
int check_term(int *temp_array, int n);

// Computes the FIRST set of a term
void First(int n, grammar G, int temp_array[100], FirstAndFollow F);

// Computes the FOLLOW set of a term
void Follow(int n, grammar G, int temp_array[100], FirstAndFollow F, int check_recursion[NONTERMINALS]);

// Prints the parser table
void printTable(table T);

// To initialize the grammar
grammar initialize_grammar();

// Initialize the parser table using first and follow sets
table initialize_table(FirstAndFollow F);

// Frees the memory allocated to the parse tree
void freeParseTree(parseTree root);

// Frees the memory allocated to the first and follow sets
void freeFirstandFollow(FirstAndFollow F);

// Frees the memory allocated to the parser table
void freeTable(table T);

// Frees the memory allocated to the grammar
void freeGrammar(grammar G);

void parser(char *testcaseFile, char *outputFile);