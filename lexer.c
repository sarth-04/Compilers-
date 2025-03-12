
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "lexerDef.h"
#include "parserDef.h"
extern int errno;
int size;
int counter;

Symbol *symbol;
TokenMap *mapping;
LexemeArray *lexeme_array;

char *lexemebegin = NULL;
char *forward = NULL;
int line_no;
int state;
int symbol_table_max_size = 100;

// HashSet Functions
unsigned int hash(char *lexeme)
{
    unsigned int hashval = 0;
    for (; *lexeme != '\0'; lexeme++)
        hashval = *lexeme + 31 * hashval;
    return hashval % HASH_SIZE;
}

HashNode *createHashNode(char *str)
{
    HashNode *node = (HashNode *)malloc(sizeof(HashNode));
    if (node != NULL)
    {
        strcpy(node->lexeme, str);
        node->next = NULL;
    }
    return node;
}

HashSet *createHashSet()
{
    HashSet *set = (HashSet *)malloc(sizeof(HashSet));
    for (int i = 0; i < HASH_SIZE; i++)
    {
        set->buckets[i] = NULL;
    }
    return set;
}

int insertToken(HashSet *set, char *str)
{
    unsigned int index = hash(str);
    // printf("%s\n",str);
    // printf("%d\n",index);
    HashNode *node = set->buckets[index];
    HashNode *temp = node;
    int i = 0;
    while (node != NULL)
    {
        i++;
        if (strcmp(node->lexeme, str) == 0)
        {
            // string already exists in the set
            return 0;
        }
        temp = node;
        node = node->next;
    }
    HashNode *newNode = createHashNode(str);
    if (i == 0)
        set->buckets[index] = newNode;
    else
    {
        temp->next = newNode;
    }
    newNode->next = NULL;
    return 1;
}

void freeHashSet(HashSet *set)
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        HashNode *node = set->buckets[i];
        while (node != NULL)
        {
            HashNode *temp = node;
            node = node->next;
            free(temp);
        }
    }
    free(set);
}

bool searchSet(char *str, HashSet *set)
{
    // printf("search\n");
    unsigned int index = hash(str);
    HashNode *node = set->buckets[index];
    while (node != NULL)
    {
        if (strcmp(str, node->lexeme) == 0)
        {
            return true;
        }
        node = node->next;
    }
    return false;
}

// Symbol Table Functions
LexemeArray *initialize_lexeme_array()
{
    LexemeArray *array = createHashSet();
    return array;
}

TokenMap *initialize_token_map()
{
    TokenMap *map = createHashSet();
    return map;
}

Symbol *initialize_symbol_table()
{
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    Record *table = (Record *)malloc(sizeof(Record) * 100);
    symbol->table = table;
    symbol->size = 0;
    return symbol;
}

void free_table(Symbol *symbol)
{
    free(symbol->table);
    free(symbol);
}

int getIndex_mapping(TokenMap *mapping, char *token)
{
    if (searchSet(token, mapping))
        return hash(token);
    return -1; // Key not found
}

void insert_mapping(TokenMap *mapping, char *token)
{
    insertToken(mapping, token);
}

void insertInto_lexemeArray(char *lexeme, LexemeArray *lexeme_array)
{
    insertToken(lexeme_array, lexeme);
}

void insert(Symbol *symbol, LexemeArray *lexeme_array, TokenMap *mapping, char *lexeme, char *token, int line_no)
{
    if (searchSet(lexeme, lexeme_array))
    {
        return;
    }
    insert_mapping(mapping, token);
    int value = getIndex_mapping(mapping, token);
    insertInto_lexemeArray(lexeme, lexeme_array);
    if (symbol->size == symbol_table_max_size - 1)
    {
        symbol_table_max_size += 100;
        symbol->table = (Record *)realloc(symbol->table, sizeof(Record *) * symbol_table_max_size);
    }
    symbol->table[symbol->size].lexeme = hash(lexeme);
    symbol->table[symbol->size].token = value;
    symbol->table[symbol->size].line_no = line_no;
    symbol->size++;
}

void insertKeywords(Symbol *symbol_table, LexemeArray *lexeme_array, TokenMap *token_map)
{

    insert(symbol_table, lexeme_array, token_map, "<--", "TK_ASSIGNOP", 0);
    insert(symbol_table, lexeme_array, token_map, "%", "TK_COMMENT", 0);
    insert(symbol_table, lexeme_array, token_map, "with", "TK_WITH", 0);
    insert(symbol_table, lexeme_array, token_map, "parameters", "TK_PARAMETERS", 0);
    insert(symbol_table, lexeme_array, token_map, "end", "TK_END", 0);
    insert(symbol_table, lexeme_array, token_map, "while", "TK_WHILE", 0);
    insert(symbol_table, lexeme_array, token_map, "union", "TK_UNION", 0);
    insert(symbol_table, lexeme_array, token_map, "endunion", "TK_ENDUNION", 0);
    insert(symbol_table, lexeme_array, token_map, "definetype", "TK_DEFINETYPE", 0);
    insert(symbol_table, lexeme_array, token_map, "as", "TK_AS", 0);
    insert(symbol_table, lexeme_array, token_map, "type", "TK_TYPE", 0);
    insert(symbol_table, lexeme_array, token_map, "_main", "TK_MAIN", 0);
    insert(symbol_table, lexeme_array, token_map, "global", "TK_GLOBAL", 0);
    insert(symbol_table, lexeme_array, token_map, "parameter", "TK_PARAMETER", 0);
    insert(symbol_table, lexeme_array, token_map, "list", "TK_LIST", 0);
    insert(symbol_table, lexeme_array, token_map, "[", "TK_SQL", 0);
    insert(symbol_table, lexeme_array, token_map, "]", "TK_SQR", 0);
    insert(symbol_table, lexeme_array, token_map, "input", "TK_INPUT", 0);
    insert(symbol_table, lexeme_array, token_map, "parameter", "TK_OUTPUT", 0);
    insert(symbol_table, lexeme_array, token_map, "int", "TK_INT", 0);
    insert(symbol_table, lexeme_array, token_map, "real", "TK_REAL", 0);
    insert(symbol_table, lexeme_array, token_map, ",", "TK_COMMA", 0);
    insert(symbol_table, lexeme_array, token_map, ";", "TK_SEM", 0);
    insert(symbol_table, lexeme_array, token_map, ":", "TK_COLON", 0);
    insert(symbol_table, lexeme_array, token_map, ".", "TK_DOT", 0);
    insert(symbol_table, lexeme_array, token_map, "endwhile", "TK_ENDWHILE", 0);
    insert(symbol_table, lexeme_array, token_map, "(", "TK_OP", 0);
    insert(symbol_table, lexeme_array, token_map, ")", "TK_CL", 0);
    insert(symbol_table, lexeme_array, token_map, "if", "TK_IF", 0);
    insert(symbol_table, lexeme_array, token_map, "then", "TK_THEN", 0);
    insert(symbol_table, lexeme_array, token_map, "endif", "TK_ENDIF", 0);
    insert(symbol_table, lexeme_array, token_map, "read", "TK_READ", 0);
    insert(symbol_table, lexeme_array, token_map, "write", "TK_WRITE", 0);
    insert(symbol_table, lexeme_array, token_map, "return", "TK_RETURN", 0);
    insert(symbol_table, lexeme_array, token_map, "+", "TK_PLUS", 0);
    insert(symbol_table, lexeme_array, token_map, "-", "TK_MINUS", 0);
    insert(symbol_table, lexeme_array, token_map, "*", "TK_MUL", 0);
    insert(symbol_table, lexeme_array, token_map, "/", "TK_DIV", 0);
    insert(symbol_table, lexeme_array, token_map, "call", "TK_CALL", 0);
    insert(symbol_table, lexeme_array, token_map, "record", "TK_RECORD", 0);
    insert(symbol_table, lexeme_array, token_map, "endrecord", "TK_ENDRECORD", 0);
    insert(symbol_table, lexeme_array, token_map, "else", "TK_ELSE", 0);
    insert(symbol_table, lexeme_array, token_map, "&&&", "TK_AND", 0);
    insert(symbol_table, lexeme_array, token_map, "@@@", "TK_OR", 0);
    insert(symbol_table, lexeme_array, token_map, "~", "TK_NOT", 0);
    insert(symbol_table, lexeme_array, token_map, "<", "TK_LT", 0);
    insert(symbol_table, lexeme_array, token_map, "<=", "TK_LE", 0);
    insert(symbol_table, lexeme_array, token_map, "==", "TK_EQ", 0);
    insert(symbol_table, lexeme_array, token_map, ">", "TK_GT", 0);
    insert(symbol_table, lexeme_array, token_map, ">=", "TK_GE", 0);
    insert(symbol_table, lexeme_array, token_map, "!=", "TK_NE", 0);
}

// Lexer Functions

int readBuffer(char buffer[BUFF_SIZE], int fd)
{
    int sz = read(fd, buffer, BUFF_SIZE);
    for (int i = sz; i < BUFF_SIZE; i++)
    {
        buffer[i] = ' ';
    }
    return sz;
}

void removeComments(char *testcaseFile)
{
    /*This function is an additional plug in to clean the source code by removal of comments. The function takes as input the source code and writes the clean code in the file appropriately.*/
    printf("File to remove commments from is:%s\n", testcaseFile);
    char *cleanFile = "cleanFile.txt";
    int fd = open(testcaseFile, O_RDONLY);
    if (fd == -1)
    {
        printf("\nFile does not exist or does not open\n");
        exit(1);
    }
    // count number of characters in file
    int file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    FILE *fd2 = fopen(cleanFile, "w");
    if (fd == -1)
    {
        printf("\nFile does not exist or does not open\n");
        exit(1);
    }
    char buff[file_size];
    int sz = read(fd, buff, file_size);
    buff[sz] = '\0';
    int state = 0;
    char *c = buff;
    while ((*c) != '\0')
    {
        switch (state)
        {
        case 0:
        {
            if (*c == ' ' || *c == '\f' || *c == '\r' || *c == '\t' || *c == '\v')
            {
                fprintf(fd2, "%c", *c);
                state = 0;
            }
            else if (*c == '%')
            {
                state = 3;
            }
            else if (*c == '\n')
            {
                fprintf(fd2, "%c", *c);
                state = 0;
            }
            else
            {
                fprintf(fd2, "%c", *c);
                state = 2;
            }
            break;
        }
        case 2:
        {
            fprintf(fd2, "%c", *c);
            if (*c == '\n')
                state = 0;
            break;
        }
        case 3:
        {
            if (*c == '\n')
            {
                fprintf(fd2, "%c", *c);
                state = 0;
            }
            break;
        }
        }
        c++;
    }
    close(fd);
    fclose(fd2);
}

void lexical_intializer(Symbol *s, LexemeArray *l, TokenMap *t)
{
    // This function is used to initialize the symbol table, lexeme array and token map
    symbol = s;
    lexeme_array = l;
    mapping = t;
}

void initialise_buffers(twinBuffer *B)
{
    // This function is used to initialize the buffers
    B->buffer1 = (char *)malloc(BUFF_SIZE * sizeof(char));
    B->buffer2 = (char *)malloc(BUFF_SIZE * sizeof(char));
    lexemebegin = B->buffer1;
    forward = B->buffer1;
    state = 0;
    line_no = 1;
    counter = 1;
}

void initialise_pointers(twinBuffer *B)
{
    lexemebegin = B->buffer1;
    forward = B->buffer1;
    state = 0;
    line_no = 1;
}

void free_buffer(twinBuffer *B)
{
    free(B->buffer1);
    free(B->buffer2);
    free(B);
    B = NULL;
}

tokenInfo *tokenise(tokenInfo *t, char *lexemebegin, char *forward, char *token, int line_no, twinBuffer *B)
{
    char lexeme[2 * BUFF_SIZE];
    int i = 0;
    while (lexemebegin != forward)
    {
        lexeme[i] = *(lexemebegin);
        lexemebegin++;
        i++;
        if (lexemebegin == B->buffer1 + BUFF_SIZE)
            lexemebegin = B->buffer2;
        if (lexemebegin == B->buffer2 + BUFF_SIZE)
            lexemebegin = B->buffer1;
    }
    lexeme[i] = '\0';
    int length = i;

    if ((length > 20 && strcmp(token, "TK_ID") == 0) || (length > 30 && strcmp(token, "TK_FUNID") == 0))
    {
        printf("Line %d Error: Variable Identifier %s is longer than the prescribed length of 20 characters. \n \n", line_no, lexeme);
        strcpy(t->token, "ERROR");
        t->line_no = line_no;
        return t;
    }
    insert(symbol, lexeme_array, mapping, lexeme, token, line_no);
    strcpy(t->token, token);
    strcpy(t->lexeme, lexeme);
    t->line_no = line_no;
    return t;
}

tokenInfo *getNextToken(twinBuffer *B, int fd)
{
    tokenInfo *t = (tokenInfo *)malloc(sizeof(tokenInfo));
    char c;
    int flag = 0;
    int sizeRead = 0;
    int Complete_flag = 0;
    while (1)
    {
        if (forward == (B->buffer1 + BUFF_SIZE))
        {
            sizeRead = readBuffer(B->buffer2, fd);
            forward = B->buffer2;
            if (sizeRead < BUFF_SIZE)
            {
                Complete_flag = 1;
                B->buffer2[sizeRead] = EOF;
            }
        }
        if (forward == (B->buffer2 + BUFF_SIZE))
        {
            sizeRead = readBuffer(B->buffer1, fd);
            forward = B->buffer1;
            if (sizeRead < BUFF_SIZE)
            {
                Complete_flag = 1;
                B->buffer1[sizeRead] = EOF;
            }
        }
        c = *(forward);
        switch (state)
        {
        case 100: // error state
        {
            char lexeme[2 * BUFF_SIZE];
            int i = 0;
            while (lexemebegin != (forward - 1))
            {
                lexeme[i] = *(lexemebegin);
                lexemebegin++;
                i++;
                if (lexemebegin == B->buffer1 + BUFF_SIZE)
                    lexemebegin = B->buffer2;
                if (lexemebegin == B->buffer2 + BUFF_SIZE)
                    lexemebegin = B->buffer1;
            }
            lexeme[i] = '\0';
            printf("Line %d Error: Unknown Symbol %s\n", line_no, lexeme);
            state = 0;
            forward--;
            if (forward == B->buffer1 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            flag = 1;
            break;
        }
        case 0:
        {
            flag = 0;
            while (*(forward) == ' ' || *(forward) == '\t' || *(forward) == '\n' || *(forward) == '\r')
            {

                if (*(forward) == '\n')
                    line_no++;
                forward++;
                if (forward == (B->buffer1 + BUFF_SIZE))
                {
                    sizeRead = readBuffer(B->buffer2, fd);
                    forward = B->buffer2;

                    if (sizeRead < BUFF_SIZE)
                    {
                        Complete_flag = 1;
                        B->buffer2[sizeRead] = EOF;
                    }
                }
                if (forward == (B->buffer2 + BUFF_SIZE))
                {
                    sizeRead = readBuffer(B->buffer1, fd);
                    forward = B->buffer1;

                    if (sizeRead < BUFF_SIZE)
                    {
                        Complete_flag = 1;
                        B->buffer1[sizeRead] = EOF;
                    }
                }
            }
            lexemebegin = forward;
            c = *(forward);
            if (c == -1)
            {
                strcpy(t->token, "FINISHED");

                t->line_no = line_no;
                return t;
            }
            if (c == '<')
            {
                state = 1;
            }
            else if (c == '>')
                state = 7;
            else if (c == '!')
                state = 10;
            else if (c == '=')
                state = 12;
            else if (c == '@')
                state = 14;
            else if (c == '%')
                state = 33;
            else if (c == '&')
                state = 17;
            else if (c == '~')
                state = 20;
            else if (c >= 'b' && c <= 'd')
                state = 21;
            else if (c == 'a' || (c >= 'e' && c <= 'z'))
                state = 25;
            else if (c == '#')
                state = 27;
            else if (c >= '0' && c <= '9')
                state = 28;
            else if (c == '%')
                state = 33;
            else if (c == '_')
                state = 38;
            else if (c == ':')
                state = 42;
            else if (c == '.')
                state = 43;
            else if (c == ';')
                state = 44;
            else if (c == '/')
                state = 45;
            else if (c == '*')
                state = 46;
            else if (c == '-')
            {
                state = 47;
            }
            else if (c == '+')
                state = 48;
            else if (c == ',')
                state = 49;
            else if (c == ')')
                state = 50;
            else if (c == '(')
                state = 51;
            else if (c == ']')
                state = 52;
            else if (c == '[')
                state = 53;
            else if (c == '\0')
            {
                flag = 1;
                break;
            }
            else
                state = 99;
            forward++;
        }
        break;
        case 33: // comment handler
        {
            c = *(forward);
            if (c == '\n')
                state = 58;
            else
                state = 33;
            forward++;
        }
        break;

        case 1:
        {
            c = *(forward);
            if (c == '-')
                state = 2;
            else if (c == '=')
                state = 5;
            else
                state = 6;
            forward++;
        }
        break;

        case 2:
        {
            c = *(forward);
            if (c == '-')
                state = 3;
            else
                state = 56;
            forward++;
        }
        break;

        case 3:
        {
            c = *(forward);
            if (c == '-')
                state = 4;
            else
                state = 100;
            forward++;
        }
        break;
        case 7:
        {
            c = *(forward);
            if (c == '=')
                state = 8;
            else
                state = 9;
            forward++;
        }
        break;
        case 10:
        {
            c = *(forward);
            if (c == '=')
                state = 11;
            else
                state = 100;
            forward++;
        }
        break;
        case 12:
        {
            c = *(forward);
            if (c == '=')
                state = 13;
            else
                state = 100;
            forward++;
        }
        break;
        case 14:
        {
            c = *(forward);
            if (c == '@')
                state = 15;
            else
                state = 100;
            forward++;
        }
        break;

        case 15:
        {
            c = *(forward);
            if (c == '@')
                state = 16;
            else
                state = 100;
            forward++;
        }
        break;

        case 17:
        {
            c = *(forward);
            if (c == '&')
                state = 18;
            else
                state = 100;
            forward++;
        }
        break;

        case 18:
        {
            c = *(forward);
            if (c == '&')
                state = 19;
            else
                state = 100;
            forward++;
        }
        break;

        case 20:
        {
            // tokenize TK_MINUS, -

            t = tokenise(t, lexemebegin, forward, "TK_NOT", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;

        case 21:
        {
            c = *(forward);
            if (c >= '2' && c <= '7')
                state = 22;
            else if (c >= 'a' && c <= 'z')
                state = 25;
            else
                state = 26;
            forward++;
        }
        break;

        case 22:
        {
            c = *(forward);
            if (c >= 'b' && c <= 'd')
                state = 22;
            else if (c >= '2' && c <= '7')
                state = 23;
            else
                state = 24;
            forward++;
        }
        break;

        case 23:
        {
            c = *(forward);
            if (c >= '2' && c <= '7')
                state = 23;
            else
                state = 24;
            forward++;
        }
        break;

        case 25:
        {
            c = *(forward);
            if (c >= 'a' && c <= 'z')
                state = 25;
            else
                state = 26;
            forward++;
        }
        break;

        case 27:
        {
            c = *(forward);
            if (c >= 'a' && c <= 'z')
                state = 54;
            else
                state = 100;
            forward++;
        }
        break;

        case 54:
        {
            c = *(forward);
            if (c >= 'a' && c <= 'z')
                state = 54;
            else
                state = 55;
            forward++;
        }
        break;

        case 28:
        {
            c = *(forward);
            if (c == '.')
                state = 29;
            else if (c >= '0' && c <= '9')
                state = 28;
            else
                state = 30;
            forward++;
        }
        break;

        case 29:
        {
            c = *(forward);
            if (c >= '0' && c <= '9')
                state = 31;
            else
                state = 57;
            forward++;
        }
        break;

        case 31:
        {
            c = *(forward);
            if (c >= '0' && c <= '9')
                state = 32;
            else
                state = 100;
            forward++;
        }
        break;

        case 32:
        {
            c = *(forward);
            if (c == 'E')
            {
                state = 34;
                forward++;
            }
            else
                state = 37;
        }
        break;

        case 34:
        {
            c = *(forward);
            if (c == '+' || c == '-')
                state = 35;
            else if (c >= '0' && c <= '9')
                state = 36;
            else
                state = 100;
            forward++;
        }
        break;

        case 35:
        {
            c = *(forward);
            if (c >= '0' && c <= '9')
                state = 36;
            else
                state = 100;
            forward++;
        }
        break;

        case 36:
        {
            c = *(forward);
            if (c >= '0' && c <= '9')
                state = 37;
            else
                state = 100;
            forward++;
        }
        break;

        case 38:
        {
            c = *(forward);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                state = 39;
            else
                state = 100;
            forward++;
        }
        break;

        case 39:
        {
            c = *(forward);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                state = 39;
            else if (c >= '0' && c <= '9')
                state = 41;
            else
                state = 40;
            forward++;
        }
        break;

        case 41:
        {
            c = *(forward);
            if (c >= '0' && c <= '9')
                state = 41;
            else
                state = 40;
            forward++;
        }
        break;

            /*
            final states
            */

        case 4:
        {
            // tokenize TK_ASSIGNOP, <---
            t = tokenise(t, lexemebegin, forward, "TK_ASSIGNOP", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 6:
        {
            // tokenize TK_LT, < (single retract)
            forward--;
            if (forward == B->buffer1 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            t = tokenise(t, lexemebegin, forward, "TK_LT", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 5:
        {
            // tokenize, TK_LE, <=
            t = tokenise(t, lexemebegin, forward, "TK_LE", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 56:
        {
            // tokenize TK_LT, < (double retract)
            forward--;
            if (forward == B->buffer1 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            forward--;
            if (forward == B->buffer1 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            t = tokenise(t, lexemebegin, forward, "TK_LT", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 57:
        {
            // tokenize TK_RNUM, ptr (double retraction state)
            forward--;
            if (forward == B->buffer1 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            forward--;
            if (forward == B->buffer1 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            t = tokenise(t, lexemebegin, forward, "TK_RNUM", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 40:
        {

            forward--;
            if (forward == B->buffer1 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            char *dummy_pointer = lexemebegin;
            char lexeme[2 * BUFF_SIZE];
            int i = 0;
            while (dummy_pointer != forward)
            {
                lexeme[i] = *(dummy_pointer);
                dummy_pointer++;
                i++;
                if (dummy_pointer == B->buffer1 + BUFF_SIZE)
                    dummy_pointer = B->buffer2;
                if (dummy_pointer == B->buffer2 + BUFF_SIZE)
                    dummy_pointer = B->buffer1;
            }
            lexeme[i] = '\0';
            if (strcmp(lexeme, "_main") == 0)
                t = tokenise(t, lexemebegin, forward, "TK_MAIN", line_no, B); // check for _main and tokenise it
            else
                t = tokenise(t, lexemebegin, forward, "TK_FUNID", line_no, B); // tokenize TK_FUNID, ptr (single retraction)
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 42:
        {
            // tokenize TK_COLON, :
            t = tokenise(t, lexemebegin, forward, "TK_COLON", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 43:
        {
            // tokenize TK_DOT, .
            t = tokenise(t, lexemebegin, forward, "TK_DOT", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 44:
        {
            // tokenize TK_SEM, ;
            t = tokenise(t, lexemebegin, forward, "TK_SEM", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 45:
        {
            // tokenize TK_DIV, /
            t = tokenise(t, lexemebegin, forward, "TK_DIV", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 46:
        {
            // tokenize TK_MUL, *
            t = tokenise(t, lexemebegin, forward, "TK_MUL", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 47:
        {
            // tokenize TK_MINUS, -
            t = tokenise(t, lexemebegin, forward, "TK_MINUS", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 48:
        {
            // tokenize TK_PLUS, +
            t = tokenise(t, lexemebegin, forward, "TK_PLUS", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 49:
        {
            // tokenise TK_COMMA, ,
            t = tokenise(t, lexemebegin, forward, "TK_COMMA", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 50:
        {
            // tokenise TK_CL, )
            t = tokenise(t, lexemebegin, forward, "TK_CL", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 51:
        {
            // tokenise TK_OP, (
            t = tokenise(t, lexemebegin, forward, "TK_OP", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 52:
        {
            // tokenise TK_SQR, ]
            t = tokenise(t, lexemebegin, forward, "TK_SQR", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 53:
        {
            // tokenise TK_SQL, [
            t = tokenise(t, lexemebegin, forward, "TK_SQL", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 37:
        {
            // tokenise TK_RNUM, ptr
            t = tokenise(t, lexemebegin, forward, "TK_RNUM", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 30:
        {
            // tokenize TK_NUM, ptr (single retraction)
            forward--;
            if (forward == B->buffer1 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            t = tokenise(t, lexemebegin, forward, "TK_NUM", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 55:
        {
            // tokenize TK_RUID, ptr
            forward--;
            if (forward == B->buffer1 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            t = tokenise(t, lexemebegin, forward, "TK_RUID", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 8:
        {
            forward = forward;
            t = tokenise(t, lexemebegin, forward, "TK_GE", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;

        case 9:
        {
            forward = forward - 1;
            if (forward == B->buffer1 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            t = tokenise(t, lexemebegin, forward, "TK_GT", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;

        case 11:
        {
            forward = forward;
            t = tokenise(t, lexemebegin, forward, "TK_NE", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;

        case 13:
        {
            forward = forward;
            t = tokenise(t, lexemebegin, forward, "TK_EQ", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;

        case 16:
        {
            forward = forward;
            t = tokenise(t, lexemebegin, forward, "TK_OR", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;

        case 58:
        {
            forward = forward;
            line_no++;
            state = 0;
            flag = 1;
        }
        break;

        case 19:
        {
            forward = forward;
            t = tokenise(t, lexemebegin, forward, "TK_AND", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;

        case 24:
        {
            forward = forward - 1;
            if (forward == B->buffer1 - 1)
            {

                int x = lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                int x = lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            t = tokenise(t, lexemebegin, forward, "TK_ID", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;

        case 26:
        {
            forward = forward - 1;
            if (forward == B->buffer1 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer2 + BUFF_SIZE - 1;
            }
            if (forward == B->buffer2 - 1)
            {
                lseek(fd, -(sizeRead), SEEK_CUR);
                forward = B->buffer1 + BUFF_SIZE - 1;
            }
            char *dummy_pointer = lexemebegin;
            char lexeme[2 * BUFF_SIZE];
            int i = 0;
            while (dummy_pointer != forward)
            {
                lexeme[i] = *(dummy_pointer);
                dummy_pointer++;
                i++;
                if (dummy_pointer == B->buffer1 + BUFF_SIZE)
                    dummy_pointer = B->buffer2;
                if (dummy_pointer == B->buffer2 + BUFF_SIZE)
                    dummy_pointer = B->buffer1;
            }
            lexeme[i] = '\0';
            if (strcmp(lexeme, "with") == 0)
                tokenise(t, lexemebegin, forward, "TK_WITH", line_no, B);
            else if (strcmp(lexeme, "parameters") == 0)
                tokenise(t, lexemebegin, forward, "TK_PARAMETERS", line_no, B);
            else if (strcmp(lexeme, "parameter") == 0)
                tokenise(t, lexemebegin, forward, "TK_PARAMETER", line_no, B);
            else if (strcmp(lexeme, "end") == 0)
                tokenise(t, lexemebegin, forward, "TK_END", line_no, B);
            else if (strcmp(lexeme, "while") == 0)
                tokenise(t, lexemebegin, forward, "TK_WHILE", line_no, B);
            else if (strcmp(lexeme, "union") == 0)
                tokenise(t, lexemebegin, forward, "TK_UNION", line_no, B);
            else if (strcmp(lexeme, "endunion") == 0)
                tokenise(t, lexemebegin, forward, "TK_ENDUNION", line_no, B);
            else if (strcmp(lexeme, "definetype") == 0)
                tokenise(t, lexemebegin, forward, "TK_DEFINETYPE", line_no, B);
            else if (strcmp(lexeme, "as") == 0)
                tokenise(t, lexemebegin, forward, "TK_AS", line_no, B);
            else if (strcmp(lexeme, "type") == 0)
                tokenise(t, lexemebegin, forward, "TK_TYPE", line_no, B);
            else if (strcmp(lexeme, "global") == 0)
                tokenise(t, lexemebegin, forward, "TK_GLOBAL", line_no, B);
            else if (strcmp(lexeme, "list") == 0)
                tokenise(t, lexemebegin, forward, "TK_LIST", line_no, B);
            else if (strcmp(lexeme, "input") == 0)
                tokenise(t, lexemebegin, forward, "TK_INPUT", line_no, B);
            else if (strcmp(lexeme, "output") == 0)
                tokenise(t, lexemebegin, forward, "TK_OUTPUT", line_no, B);
            else if (strcmp(lexeme, "int") == 0)
                tokenise(t, lexemebegin, forward, "TK_INT", line_no, B);
            else if (strcmp(lexeme, "real") == 0)
                tokenise(t, lexemebegin, forward, "TK_REAL", line_no, B);
            else if (strcmp(lexeme, "endwhile") == 0)
                tokenise(t, lexemebegin, forward, "TK_ENDWHILE", line_no, B);
            else if (strcmp(lexeme, "if") == 0)
                tokenise(t, lexemebegin, forward, "TK_IF", line_no, B);
            else if (strcmp(lexeme, "then") == 0)
                tokenise(t, lexemebegin, forward, "TK_THEN", line_no, B);
            else if (strcmp(lexeme, "endif") == 0)
                tokenise(t, lexemebegin, forward, "TK_ENDIF", line_no, B);
            else if (strcmp(lexeme, "read") == 0)
                tokenise(t, lexemebegin, forward, "TK_READ", line_no, B);
            else if (strcmp(lexeme, "write") == 0)
                tokenise(t, lexemebegin, forward, "TK_WRITE", line_no, B);
            else if (strcmp(lexeme, "return") == 0)
                tokenise(t, lexemebegin, forward, "TK_RETURN", line_no, B);
            else if (strcmp(lexeme, "call") == 0)
                tokenise(t, lexemebegin, forward, "TK_CALL", line_no, B);
            else if (strcmp(lexeme, "record") == 0)
                tokenise(t, lexemebegin, forward, "TK_RECORD", line_no, B);
            else if (strcmp(lexeme, "endrecord") == 0)
                tokenise(t, lexemebegin, forward, "TK_ENDRECORD", line_no, B);
            else if (strcmp(lexeme, "else") == 0)
                tokenise(t, lexemebegin, forward, "TK_ELSE", line_no, B);
            else
                tokenise(t, lexemebegin, forward, "TK_FIELDID", line_no, B);
            state = 0;
            flag = 1;
            return t;
        }
        break;
        case 99:
        {

            state = 100;
            forward++;
        }
        break;
        }
    }
}