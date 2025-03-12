

#ifndef LEXICAL_H
#define LEXICAL_H
#include "lexerDef.h"
#include "parserDef.h"

// Symbol Table Functions
LexemeArray *initialize_lexeme_array();

TokenMap *initialize_token_map();

Symbol *initialize_symbol_table();

void free_table(Symbol *symbol);

void free_buffer(twinBuffer *B);

void insertKeywords(Symbol *symbol_table, LexemeArray *lexeme_array, TokenMap *token_map);

void insertInto_lexemeArray(char *lexeme, LexemeArray *lexeme_array);

int getIndex_mapping(TokenMap *mapping, char *token);

void insert_mapping(TokenMap *mapping, char *token);

void insert(Symbol *symbol, LexemeArray *lexeme_array, TokenMap *mapping, char *lexeme, char *token, int line_no);

// Hashset Functions
unsigned int hash(char *lexeme);

HashNode *createHashNode(char *str);

HashSet *createHashSet();

int insertToken(HashSet *set, char *str);

void freeHashSet(HashSet *set);

bool searchSet(char *str, HashSet *set);

// Lexer Functions

int readBuffer(char buffer[BUFF_SIZE], int fd);

void removeComments(char *testcaseFile);

void lexical_intializer(Symbol *s, LexemeArray *l, TokenMap *t);

void initialise_buffers(twinBuffer *B);

void initialise_pointers(twinBuffer *B);

tokenInfo *tokenise(tokenInfo *t, char *lexemebegin, char *forward, char *token, int line_no, twinBuffer *B);

tokenInfo *getNextToken(twinBuffer *B, int fd);

#endif