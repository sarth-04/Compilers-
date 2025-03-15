#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include "parser.h"
#include "lexer.h"
#include "parserDef.h"

grammar G;
FirstAndFollow F;
table T;
int syntactically_correct = 1;
int parser_initialized = 0;

LexemeArray *lexeme_array1;
Symbol *symbol_table1;
TokenMap *token_map1;
twinBuffer *B1;
extern int lexer_initialized;
extern LexemeArray *lexeme_array1;
extern Symbol *symbol_table1;
extern TokenMap *token_map1;
extern twinBuffer *B1;


int readBufferParse(int fd, twinBuffer *B)
{
    int n = 0;
    n = readBuffer(B->buffer1, fd);
    return n;
}

// Funtion to find the location of the terminal or non terminal in the ter
int find_location(char *term)
{
    for (int i = 0; i < TERMS_SIZE; i++)
    {
        if (strcmp(grammarTerms[i], term) == 0)
        {
            return i;
        }
    }
    return -1;
}

// Function to create a new stack
Stack *newStack()
{
    Stack *s = (Stack *)malloc(sizeof(Stack));
    if (s != NULL)
        s->top = -1;
    return s;
}

// Function to return the top of the stack
parseTree top(Stack *stack)
{
    int top = stack->top;
    return stack->data[top];
}

// Function to pop the top of the stack
bool pop(Stack *stack)
{
    if (stack->top == -1)
        return false;
    (stack->top)--;
    return true;
}

// Function to check whether the stack is empty or not
bool isEmpty(Stack *stack)
{
    if (stack->top == -1)
        return true;
    else
        return false;
}

// Function to push the element which is our non terminal into the stack
bool push(Stack *s, parseTree e)
{
    if (s->top == STACK_SIZE - 1)
        return false;
    s->data[++(s->top)] = e;
    return true;
}

// Function to print all the elements of the stack
void print_stack(Stack *stack)
{
    for (int i = 0; i <= stack->top; i++)
    {
        printf("%d\n", stack->data[i]->current);
    }
}

// Function to create a new parsernode
parseTree createNewNode()
{
    // printf("inside ");
    parseTree node = (parseTree)malloc(sizeof(struct parseTree));
    node->parent = NULL;
    node->child = NULL;
    node->sizeofChild = 0;
    node->lineNo = 0;
    strcpy(node->lexeme, "");
    return node;
}
void printNode(FILE *outfile, parseTree node) {
    if (node == NULL) return;

    // Determine whether the node is a leaf
    const char *isLeaf = (node->child == NULL) ? "yes" : "no";

    // Determine the lexeme or use "----" for non-leaf nodes
    const char *lexeme = (strcmp(isLeaf, "yes") == 0) ? node->lexeme : "----";

    // Determine the parent node symbol
    const char *parentSymbol = (node->parent == NULL) ? "ROOT" : node->parent->symbol;

    // Print the information in the required format
    fprintf(outfile, "%-15s %-10d %-20s %-15s %-20s %-10s %-20s\n",
            lexeme,
            node->lineNo,
            node->tokenName,
            (strcmp(node->tokenName, "TK_NUM") == 0 || strcmp(node->tokenName, "TK_RNUM") == 0) ? node->valueIfNumber : "---",
            parentSymbol,
            isLeaf,
            node->symbol);
}

// Recursive inorder traversal to print the parse tree
void inorderTraversal(FILE *outfile, parseTree node) {
    if (node == NULL) return;

    // Visit all children before printing the current node
    for (int i = 0; i < node->sizeofChild; i++) {
        inorderTraversal(outfile, node->child[i]);
    }

    // Print the current node
    printNode(outfile, node);
}

// Function to parse the given token
void parseFile(char *token, char *lexeme, int lineNo, table T, Stack *s, grammar G)
{
    if (strcmp(token, "TK_COMMENT") == 0)
        return;
    int flag = 0;
    int error_flag = 0;
    while (flag == 0)
    {
        if (isEmpty(s))
            break;
        parseTree topNode = top(s);
        int indx = find_location(token);
        if (topNode->current > NONTERMINALS && strcmp(grammarTerms[topNode->current], token) == 0)
        {
            topNode->child = NULL;
            topNode->sizeofChild = 0;
            topNode->lineNo = lineNo;
            strcpy(topNode->lexeme, lexeme);
            pop(s);
            flag = 1;
            break;
        }
        else if (topNode->current > NONTERMINALS && strcmp(grammarTerms[topNode->current], token) != 0)
        {
            if (error_flag == 0)
                printf("Line %d Error: The token %s for lexeme %s does not match with the expected token %s\n", lineNo, token, lexeme, grammarTerms[topNode->current]);
            pop(s);
            error_flag = 1;
            syntactically_correct = 0;
        }
        else
        {
            int rule = T->parserTable[topNode->current - 1][indx - NONTERMINALS];
            if (rule == -1)
            {
                if (error_flag == 0)
                    printf("Line %d Error: Invalid token %s encountered with the value %s stack top %s\n", lineNo, token, lexeme, grammarTerms[topNode->current]);
                syntactically_correct = 0;
                error_flag = 1;
                return;
            }
            else if (rule == SYN)
            {
                if (error_flag == 0)
                    printf("Line %d Error: Invalid token %s encountered with the value %s stack top %s\n", lineNo, token, lexeme, grammarTerms[topNode->current]);
                pop(s);
                syntactically_correct = 0;
                error_flag = 1;
            }
            else if (rule != -1)
            {
                pop(s);
                int top = topNode->current;
                int flag2 = 0;
                for (int i = 10; i >= 1; i--)
                {

                    if (G->Grammar[rule][i] == -1)
                    {
                        flag2 = 1;
                        topNode->child = (parseTree *)malloc((i - 1) * sizeof(struct parseTree));

                        topNode->sizeofChild = i - 1;
                    }

                    if (flag2 == 1 && G->Grammar[rule][i] != -1)
                    {

                        if (G->Grammar[rule][i] == EPLS)
                        {
                            parseTree tempNode = createNewNode();

                            tempNode->child = NULL;
                            tempNode->parent = topNode;
                            tempNode->current = EPLS;
                            topNode->child[i - 1] = tempNode;
                        }
                        else
                        {

                            parseTree tempNode = createNewNode();

                            tempNode->current = G->Grammar[rule][i];

                            tempNode->parent = topNode;

                            topNode->child[i - 1] = tempNode;

                            push(s, tempNode);
                        }
                    }
                }
            }
        }
    }
}

// Function to parse the testcase File
parseTree parseInputSourceCode(char *testcaseFile, table T) {
    Stack *s = newStack();
    parseTree root = createNewNode();
    root->parent = NULL;
    root->current = 1; // Root node
    push(s, root);

    if (lexer_initialized == 0) {
        lexeme_array1 = initialize_lexeme_array();
        symbol_table1 = initialize_symbol_table();
        token_map1 = initialize_token_map();

        insertKeywords(symbol_table1, lexeme_array1, token_map1);
        lexical_intializer(symbol_table1, lexeme_array1, token_map1);
        B1 = (twinBuffer *)malloc(sizeof(twinBuffer));
        initialise_buffers(B1);
        lexer_initialized = 1;
    }

    int f = open(testcaseFile, O_RDONLY);
    if (f == -1) {
        printf("Error opening file\n");
        exit(1);
    }

    readBufferParse(f, B1);
    initialise_pointers(B1);

    tokenInfo *t;
    while (true) {
        t = getNextToken(B1, f);
        if (strcmp(t->token, "FINISHED") == 0)
            break;
        if (strcmp(t->token, "ERROR") == 0)
            continue;
        // Print token information
        printf("Line no:%d\tLexeme: %-20sTokenName: %-20s\n", t->line_no, t->lexeme, t->token);


        parseFile(t->token, t->lexeme, t->line_no, T, s, G);
        free(t);
    }
    printf("\nBoth lexer and syntax module developed.\n\n");

    free(s);
    close(f);

    return root;
}

// Recursive function which helps in printing the parse tree
void outputTree(parseTree root, FILE *fd) {
    if (root == NULL) return;

    // Perform inorder traversal for children
    for (int i = 0; i < root->sizeofChild; i++) {
        outputTree(root->child[i], fd);
    }

    // Determine if the node is a leaf
    const char *isLeaf = (root->sizeofChild == 0) ? "yes" : "no";

    // Determine lexeme or dummy string for non-leaf nodes
    const char *lexeme = (strcmp(isLeaf, "yes") == 0) ? root->lexeme : "----";

    // Determine parent symbol
    const char *parentSymbol = (root->parent == NULL) ? "ROOT" : grammarTerms[root->parent->current];

    // Handle numeric tokens
    if (strcmp(grammarTerms[root->current], "TK_NUM") == 0) {
        fprintf(fd, "%-15s %-20s %-10d %-20s %-15d %-20s %-10s %-20s\n",
                lexeme, grammarTerms[root->current], root->lineNo, grammarTerms[root->current], atoi(root->lexeme),
                parentSymbol, isLeaf, grammarTerms[root->current]);
    } else if (strcmp(grammarTerms[root->current], "TK_RNUM") == 0) {
        fprintf(fd, "%-15s %-20s %-10d %-20s %-15.5f %-20s %-10s %-20s\n",
                lexeme, grammarTerms[root->current], root->lineNo, grammarTerms[root->current], atof(root->lexeme),
                parentSymbol, isLeaf, grammarTerms[root->current]);
    } else {
        fprintf(fd, "%-15s %-20s %-10d %-20s %-15s %-20s %-10s %-20s\n",
                lexeme, grammarTerms[root->current], root->lineNo, grammarTerms[root->current], "---",
                parentSymbol, isLeaf, grammarTerms[root->current]);
    }
}

// Function to print the parse tree
void printParseTree(parseTree PT, char *outfile) {
    FILE *fp = fopen(outfile, "w");
    if (fp == NULL) {
        perror("Error opening output file");
        return;
    }

    // Print header for better readability
    fprintf(fp, "%-15s %-20s %-10s %-20s %-15s %-20s %-10s %-20s\n",
            "Lexeme", "CurrentNode", "LineNo", "TokenName", "ValueIfNumber", "Parent", "IsLeaf", "NodeSymbol");
    fprintf(fp, "------------------------------------------------------------------------------------------------------\n");

    // Print parse tree using recursive function
    outputTree(PT, fp);

    fclose(fp);
}


// Function to print Grammar
void print_grammar(grammar G)
{
    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if ((G->Grammar)[i][j] == -1)
            {
                printf("\n");
                break;
            }
            if (j == 0)
            {
                printf("<%s> ===> ", grammarTerms[(G->Grammar)[i][j]]);
            }
            else
            {
                if (grammarTerms[G->Grammar[i][j]][0] == 'T' || G->Grammar[i][j] == EPLS)
                    printf("%s ", grammarTerms[(G->Grammar)[i][j]]);
                else
                    printf("<%s> ", grammarTerms[(G->Grammar)[i][j]]);
            }
        }
    }
}

// Function to print the first set of grammar terms
void print_first_set(FirstAndFollow F, grammar G)
{
    for (int i = 0; i < TERMS_SIZE; i++)
    {
        printf("%s ===> ", grammarTerms[i]);
        for (int j = 0; j < 20; j++)
        {
            if (F->first[i][j] == 0)
                break;
            else
                printf("%s, ", grammarTerms[F->first[i][j]]);
        }
        printf("\n");
    }
}

// FUnction to print the follow set of non terminals
void print_follow_set(FirstAndFollow F, grammar G)
{
    for (int i = 1; i <= NONTERMINALS; i++)
    {
        printf("%s ===> ", grammarTerms[i]);
        for (int j = 0; j < 100; j++)
        {
            if (F->follow[i][j] == 0)
                break;
            else if (F->follow[i][j] == -1)
                printf("$, ");
            else
                printf("%s, ", grammarTerms[F->follow[i][j]]);
        }
        printf("\n");
    }
}

// Function to check whether the given term is already present in the set or not
int check_term(int *temp_array, int n)
{
    int i = 0, flag = 0;
    for (i = 0; temp_array[i] != 0; i++)
    {
        if (temp_array[i] == n)
        {
            flag = 1;
            break;
        }
    }
    return flag; // if b is in array a
}

// Computes the FIRST set of a term
void First(int n, grammar G, int temp_array[100], FirstAndFollow F)
{
    int j = 0;
    for (int i = 0; i < 20; i++)
    {
        if (temp_array[i] == 0)
        {
            j = i;
            break;
        }
    }
    if (n > NONTERMINALS)
    {
        temp_array[j] = n;
    }
    else
    {
        for (int i = 0; i < GRAMMAR_SIZE; i++)
        {
            if (G->Grammar[i][0] == n)
            {
                if (G->Grammar[i][1] > NONTERMINALS)
                {
                    if (!check_term(temp_array, G->Grammar[i][1]))
                    {
                        temp_array[j++] = G->Grammar[i][1];
                    }
                }
                else
                {
                    int flag = 0;
                    for (int k = 1; k < 10; k++)
                    {
                        if (G->Grammar[i][k] == -1)
                            break;
                        flag = 0;
                        First(G->Grammar[i][k], G, F->first[G->Grammar[i][k]], F);
                        for (int l = 0; F->first[G->Grammar[i][k]][l] != 0; l++)
                        {
                            if (F->first[G->Grammar[i][k]][l] == EPLS)
                                flag = 1;
                            else
                            {
                                if (!check_term(temp_array, F->first[G->Grammar[i][k]][l]))
                                {
                                    temp_array[j++] = F->first[G->Grammar[i][k]][l];
                                }
                            }
                        }
                        if (flag != 1)
                            break;
                    }
                    if (flag == 1)
                    {
                        temp_array[j++] = EPLS;
                    }
                }
            }
        }
    }
}

// Computes the FOLLOW set of a term
void Follow(int n, grammar G, int temp_array[100], FirstAndFollow F, int check_recursion[NONTERMINALS])
{
    int j = 0, k;
    if (check_recursion[n] == 1)
    {
        check_recursion[n] = 0;
        return;
    }
    if (n == 1)
    {
        temp_array[j++] = -1;
    }
    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        for (int m = 1; m < 10; m++)
        {
            if (G->Grammar[i][m] == -1)
                break;
            if (G->Grammar[i][m] == n)
            {
                for (k = m + 1; G->Grammar[i][k] != -1; k++)
                {
                    int flag = 0;
                    for (int l = 0; F->first[G->Grammar[i][k]][l] != 0; l++)
                    {
                        if (F->first[G->Grammar[i][k]][l] == EPLS)
                            flag = 1;
                        else
                        {
                            if (!check_term(temp_array, F->first[G->Grammar[i][k]][l]))
                            {
                                temp_array[j++] = F->first[G->Grammar[i][k]][l];
                            }
                        }
                    }
                    if (flag != 1)
                        break;
                }
                if (G->Grammar[i][k] == -1 && G->Grammar[i][0] != n)
                {
                    check_recursion[G->Grammar[i][0]] = 1;
                    Follow(G->Grammar[i][0], G, F->follow[G->Grammar[i][0]], F, check_recursion);
                    check_recursion[G->Grammar[i][0]] = 0;
                    for (int l = 0; F->follow[G->Grammar[i][0]][l] != 0; l++)
                    {
                        if (!check_term(temp_array, F->follow[G->Grammar[i][0]][l]))
                        {
                            temp_array[j++] = F->follow[G->Grammar[i][0]][l];
                        }
                    }
                }
            }
        }
    }
}

// Takes input as G, computes the FIRST and FOLLOW sets
FirstAndFollow ComputeFirstAndFollowSets(grammar G)
{
    FirstAndFollow F = (FirstAndFollow)malloc(sizeof(FirstAndFollow));
    F->first = (int **)malloc(TERMS_SIZE * sizeof(int *));
    for (int i = 0; i < TERMS_SIZE; i++)
    {
        F->first[i] = (int *)malloc(20 * sizeof(int));
        for (int j = 0; j < 20; j++)
        {
            F->first[i][j] = 0;
        }
    }
    for (int i = 1; i < TERMS_SIZE; i++)
    {
        First(i, G, F->first[i], F);
    }
    F->follow = (int **)malloc((NONTERMINALS + 1) * sizeof(int *));
    for (int i = 1; i <= NONTERMINALS; i++)
    {
        F->follow[i] = (int *)malloc(20 * sizeof(int));
        for (int j = 0; j < 20; j++)
        {
            F->follow[i][j] = 0;
        }
    }
    int check_recursion[NONTERMINALS] = {0};
    int count = 0;
    while (count != 5)
    {

        for (int i = 1; i <= NONTERMINALS; i++)
        {
            Follow(i, G, F->follow[i], F, check_recursion);
        }
        count++;
    }
    return F;
}

// Creates the parse table
void createParseTable(FirstAndFollow F, table T)
{
    int lhs, flag, flag2;
    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        lhs = G->Grammar[i][0];
        flag = 0;
        for (int j = 1; j < 10; j++)
        {
            flag2 = 0;
            int rhs = G->Grammar[i][j];
            if (rhs == -1)
            {
                break;
            }
            if (rhs == EPLS)
            {
                flag = 1;
                break;
            }
            if (rhs > NONTERMINALS)
            {
                T->parserTable[lhs - 1][rhs - NONTERMINALS] = i;
                break;
            }

            for (int k = 0; k < 20; k++)
            {
                if (F->first[rhs][k] == 0)
                    break;
                if (F->first[rhs][k] != EPLS)
                {
                    T->parserTable[lhs - 1][F->first[rhs][k] - NONTERMINALS] = i;
                }
                if (F->first[rhs][k] == EPLS)
                    flag2 = 1;
            }
            if (flag2 == 0)
                break;
            else if (flag2 == 1 && G->Grammar[i][j + 1] == -1)
            {
                flag = 1;
                break;
            }
        }
        if (flag == 1)
        {
            for (int j = 0; F->follow[lhs][j] != 0; j++)
            {
                int x = F->follow[lhs][j];
                if (x == -1)
                {
                    T->parserTable[lhs - 1][0] = i;
                }
                else
                {
                    T->parserTable[lhs - 1][x - NONTERMINALS] = i;
                }
            }
        }
    }
}

// Prints the parser table
void printTable(table T)
{
    for (int i = 0; i < NONTERMINALS; i++)
    {
        printf("%s ==>", grammarTerms[i + 1]);
        for (int j = 0; j <= TERMINALS; j++)
        {
            if (T->parserTable[i][j] == -1)
                printf("_ ");
            else if (T->parserTable[i][j] == SYN)
                printf("syn ");
            else
                printf("%d ", T->parserTable[i][j]);
        }
        printf("\n");
    }
}

// To initialize the grammar
grammar initialize_grammar()
{
    int temp[GRAMMAR_SIZE][10] = {
        {1, 2, 3, -1},                   // <program> ===> <otherFunctions> <mainFunction>
        {3, 69, 4, 62, -1},              // <mainFunction>===> TK_MAIN <stmts> TK_END
        {2, 5, 2, -1},                   // <otherFunctions>===> <function><otherFunctions>
        {2, 110, -1},                    // <otherFunctions>===> eps
        {5, 58, 6, 7, 80, 4, 62, -1},    // <function>===>TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END
        {6, 75, 71, 72, 73, 8, 74, -1},  // <input_par>===>TK_INPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
        {7, 76, 71, 72, 73, 8, 74, -1},  // <output_par>===>TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
        {7, 110, -1},                    // <output_par>===>eps
        {8, 9, 55, 31, -1},              // <parameter_list>===><dataType> TK_ID <remaining_list>
        {9, 10, -1},                     // <dataType>===> <primitiveDatatype>
        {9, 11, -1},                     // <dataType>===> <constructedDatatype>
        {10, 77, -1},                    // <primitiveDatatype>===> TK_INT
        {10, 78, -1},                    // <primitiveDatatype>===> TK_REAL
        {11, 12, 59, -1},                // <constructedDatatype>===> <A> TK_RUID
        {11, 59, -1},                    // <constructedDatatype>===> TK_RUID
        {31, 79, 8, -1},                 // <remaining_list>===>TK_COMMA <parameter_list> | eps
        {31, 110, -1},                   // <remaining_list>===> eps
        {4, 13, 14, 15, 16, -1},         // <stmts>===><typeDefinitions> <declarations> <otherStmts><returnStmt>
        {13, 17, 13, -1},                // <typeDefinitions>===><typeDefinition>  <typedefinitions>
        {13, 18, 13, -1},                // <typeDefinitions>===> <definetypestmt> <typeDefinitions>
        {13, 110, -1},                   // <typeDefinitions>===> eps
        {17, 97, 59, 19, 98, -1},        // <typeDefinition>===>TK_RECORD TK_RUID <fieldDefinitions>TK_ENDRECORD
        {17, 64, 59, 19, 65, -1},        // <typeDefinition>===>TK_UNION TK_RUID <fieldDefinitions> TK_ENDUNION
        {19, 20, 20, 21, -1},            // <fieldDefinitions>===> <fieldDefinition><fieldDefinition><moreFields>
        {20, 68, 9, 81, 54, 80, -1},     // <fieldDefinition>===> TK_TYPE <datatype> TK_COLON TK_FIELDID TK_SEM
        {21, 20, 21, -1},                // <moreFields>===><fieldDefinition><moreFields>
        {21, 110, -1},                   // <moreFields>===> eps
        {14, 47, 14, -1},                // <declarations> ===> <declaration><declarations>
        {14, 110, -1},                   // <declarations> ===> eps
        {47, 68, 9, 81, 55, 22, 80, -1}, // <declaration>===> TK_TYPE <dataType> TK_COLON TK_ID <global_or_not> TK_SEM
        {22, 81, 70, -1},                // <global_or_not>===>TK_COLON TK_GLOBAL
        {22, 110, -1},                   // <global_or_not>===> eps
        {15, 23, 15, -1},                // <otherStmts>===> <stmt><otherStmts>
        {15, 110, -1},                   // <otherStmts>===>  eps
        {23, 24, -1},                    // <stmt>===> <assignmentStmt>
        {23, 25, -1},                    // <stmt>===> <iterativeStmt>
        {23, 26, -1},                    // <stmt>===> <conditionalStmt>
        {23, 27, -1},                    // <stmt>===> <ioStmt>
        {23, 28, -1},                    // <stmt>===>  <funCallStmt>
        {24, 29, 109, 30, 80, -1},       // <assignmentStmt>===><SingleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM
        {29, 55, 32, -1},                // <singleOrRecId>===>TK_ID <option_single_constructed>
        {32, 33, 34, -1},                //<option_single_constructed>===>  <oneExpansion><moreExpansions>
        {32, 110, -1},
        {34, 33, 34, -1},                         // <moreExpansions>===> <oneExpansion> <moreExpansions>
        {34, 110, -1},                            // <moreExpansions>===> eps
        {33, 82, 54, -1},                         //<oneExpansion>===> TK_DOT TK_FIELDID
        {28, 35, 96, 58, 60, 61, 36, 80, -1},     // <funCallStmt>===><outputParameters> TK_CALL TK_FUNID TK_WITH TK_PARAMETERS <inputParameters> TK_SEM
        {35, 73, 37, 74, 109, -1},                // <outputParameters> ==> TK_SQL <idList> TK_SQR TK_ASSIGNOP
        {35, 110, -1},                            // <outputParameters> ==>  eps
        {36, 73, 37, 74, -1},                     // <inputParameters>===> TK_SQL <idList> TK_SQR
        {25, 63, 84, 38, 85, 23, 15, 83, -1},     // <iterativeStmt>===> TK_WHILE TK_OP <booleanExpression> TK_CL <stmt><otherStmts>TK_ENDWHILE
        {26, 86, 84, 38, 85, 87, 23, 15, 39, -1}, // <conditionalStmt>===> TK_IF TK_OP <booleanExpression> TK_CL TK_THEN <stmt><otherStmts> <factorConditionalStmt>
        {39, 99, 23, 15, 88, -1},                 // <factorConditionalStmt>===> TK_ELSE <stmt><otherStmts> TK_ENDIF | TK_ENDIF
        {39, 88, -1},                             // <factorConditionalStmt>===> TK_ELSE <stmt><otherStmts> TK_ENDIF | TK_ENDIF
        {27, 89, 84, 40, 85, 80, -1},             // <ioStmt>===>TK_READ TK_OP <var> TK_CL TK_SEM
        {27, 90, 84, 40, 85, 80, -1},             // <ioStmt>===>TK_WRITE TK_OP <var> TK_CL TK_SEM
        {30, 41, 42, -1},                         // <arithmeticExpression>===> <arith_term><arithmatic_recursion>
        {42, 43, 41, 42, -1},                     // <arithmatic_recursion>===>  <operator> <arith_term> <arithmatic_recursion>
        {42, 110, -1},                            // <arithmatic_recursion>===>  eps
        {41, 44, 45, -1},                         // <arith_term> ===> <factor> <term_recursion>
        {45, 46, 44, 45, -1},                     // <term_recursion>===> <operator_second> <factor> <term_recursion>
        {45, 110, -1},                            // <term_recursion>===> eps
        {44, 84, 30, 85, -1},                     // <factor> ====>TK_OP <arithmeticExpression> TK_CL
        {44, 40, -1},                             // <factor> ====><var>
        {43, 92, -1},                             // <operator> ===> TK_PLUS | TK_MINUS
        {43, 93, -1},                             // <operator> ===> TK_PLUS | TK_MINUS
        {46, 94, -1},                             // <operator_second> ===> TK_MUL  | TK_DIV
        {46, 95, -1},                             // <operator_second> ===> TK_MUL  | TK_DIV
        {38, 84, 38, 85, 48, 84, 38, 85, -1},     // <booleanExpression>===>TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP <booleanExpression> TK_CL
        {38, 40, 49, 40, -1},                     // <booleanExpression>===> <var> <relationalOp> <var>
        {38, 102, 84, 38, 85, -1},                // <booleanExpression>===> TK_NOT TK_OP <booleanExpression> TK_CL
        {40, 56, -1},                             // <var>===> TK_NUM | TK_RNUM | <singleOrRecId>
        {40, 57, -1},                             // <var>===> TK_NUM | TK_RNUM | <singleOrRecId>
        {40, 29, -1},                             // <var>===> TK_NUM | TK_RNUM | <singleOrRecId>
        {48, 100, -1},                            // <logicalOp>===>TK_AND | TK_OR
        {48, 101, -1},                            // <logicalOp>===>TK_AND | TK_OR
        {49, 103, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {49, 104, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {49, 105, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {49, 106, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {49, 107, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {49, 108, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {16, 91, 50, 80, -1},                     // <returnStmt>===>TK_RETURN <optionalReturn> TK_SEM
        {50, 73, 37, 74, -1},                     // <optionalReturn>===>TK_SQL <idList> TK_SQR
        {50, 110, -1},                            // <optionalReturn>===> eps
        {37, 55, 51, -1},                         // <idList>===> TK_ID <more_ids>
        {51, 79, 37, -1},                         // <more_ids>===> TK_COMMA <idList>
        {51, 110, -1},                            // <more_ids>===> eps
        {18, 66, 12, 59, 67, 59, -1},             // <definetypestmt>===>TK_DEFINETYPE <A> TK_RUID TK_AS TK_RUID
        {12, 97, -1},                             // <A>==>TK_RECORD
        {12, 64, -1}                              // <A>==>TK_UNION
    };
    grammar G = (grammar)malloc(sizeof(grammar));
    G->Grammar = (int **)malloc(GRAMMAR_SIZE * sizeof(int *));

    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        G->Grammar[i] = (int *)malloc(10 * sizeof(int));
    }
    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            G->Grammar[i][j] = temp[i][j];
            if (temp[i][j] == -1)
                break;
        }
    }
    return G;
}

// Initialize the parser table using first and follow sets
table initialize_table(FirstAndFollow F)
{
    table T = (table)malloc(sizeof(table));
    T->parserTable = (int **)malloc(NONTERMINALS * sizeof(int *));
    for (int i = 0; i < NONTERMINALS; i++)
    {
        T->parserTable[i] = (int *)malloc((TERMINALS + 1) * sizeof(int));
    }
    for (int i = 0; i < NONTERMINALS; i++)
    {
        for (int j = 0; j <= TERMINALS; j++)
        {
            T->parserTable[i][j] = -1;
        }
    }
    for (int i = 0; i < NONTERMINALS; i++)
    {
        for (int j = 0; F->follow[i + 1][j] != 0; j++)
        {
            int x = F->follow[i + 1][j];
            if (x == -1)
                T->parserTable[i][0] = SYN;
            else
                T->parserTable[i][x - NONTERMINALS] = SYN;
        }
    }
    for (int i = 0; i < NONTERMINALS; i++)
    {
        T->parserTable[i][80 - NONTERMINALS] = SYN;
        T->parserTable[i][65 - NONTERMINALS] = SYN;
        T->parserTable[i][98 - NONTERMINALS] = SYN;
        T->parserTable[i][88 - NONTERMINALS] = SYN;
        T->parserTable[i][99 - NONTERMINALS] = SYN;
        T->parserTable[i][74 - NONTERMINALS] = SYN;
        T->parserTable[i][62 - NONTERMINALS] = SYN;
        T->parserTable[i][83 - NONTERMINALS] = SYN;
        T->parserTable[i][85 - NONTERMINALS] = SYN;
    }
    return T;
}

// Frees the memory allocated to the parse tree
void freeParseTree(parseTree root)
{
    int sz = root->sizeofChild;
    for (int i = 0; i < sz; i++)
    {
        freeParseTree(root->child[i]);
    }
    free(root);
}

// Frees the memory allocated to the first and follow sets
void freeFirstandFollow(FirstAndFollow F)
{
    for (int i = 0; i < TERMS_SIZE; i++)
    {
        free(F->first[i]);
    }
    free(F->first);
    for (int i = 0; i <= NONTERMINALS; i++)
    {
        free(F->follow[i]);
    }
    free(F->follow);
    free(F);
}

// Frees the memory allocated to the parser table
void freeTable(table T)
{
    for (int i = 0; i < NONTERMINALS; i++)
    {
        free(T->parserTable[i]);
    }
    free(T->parserTable);
    free(T);
}

// Frees the memory allocated to the grammar
void freeGrammar(grammar G)
{
    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        free(G->Grammar[i]);
    }
    free(G->Grammar);
    free(G);
}

void parser(char *testcaseFile, char *outputFile)
{
    if (parser_initialized == 0)
    {
        G = initialize_grammar();
        F = ComputeFirstAndFollowSets(G);
        printf("\nFirst and Follow sets automated \n");
        T = initialize_table(F);
        createParseTable(F, T);
        parser_initialized = 1;
    }
    char dummy[5] = "----\0";
    parseTree root;
    root = parseInputSourceCode(testcaseFile, T);
    if (syntactically_correct)
        printParseTree(root, outputFile);
    else
    {
        FILE* fp=fopen(outputFile,"w");
        if(fp==NULL){
            printf("Error opening the file\n");
            exit(1);
        }
        // fprintf(fp," ");
        fclose(fp);
        printf("Code is syntactically incorrect so parse tree could not be constructed\n\n");
    }

    freeParseTree(root);
}