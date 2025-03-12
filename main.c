#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parserDef.h"
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <testcase file>\n", argv[0]);
        return 1;
    }

    // Open the input file
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    // Initialize the lexer components
    Symbol *symbol_table = initialize_symbol_table();
    LexemeArray *lexeme_array = initialize_lexeme_array();
    TokenMap *token_map = initialize_token_map();
    twinBuffer *buffer = (twinBuffer *)malloc(sizeof(twinBuffer));

    lexical_intializer(symbol_table, lexeme_array, token_map);
    initialise_buffers(buffer);
    insertKeywords(symbol_table, lexeme_array, token_map);

    // Read and print tokens until EOF
    tokenInfo *token;
    while (1) {
        token = getNextToken(buffer, fd);
        if (strcmp(token->token, "FINISHED") == 0) {
            free(token);
            break;
        }
        printf("Line %d: Token = %s, Lexeme = %s\n", token->line_no, token->token, token->lexeme);
        free(token);
    }

    // Clean up
    close(fd);
    free_buffer(buffer);
    free_table(symbol_table);
    freeHashSet(lexeme_array);
    freeHashSet(token_map);

    return 0;
}
