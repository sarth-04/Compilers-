#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lexer.h"
#include"lexerDef.h"
int main(int argc, char *argv[])
{
    int command;
    while (1)
    {
        printf("Enter 0 to exit, 1 to remove comments, 2 to print tokens, 3 to get parse, 4 to get time\n");
        scanf("%d", &command);
        switch (command)
        {
        case 0:
        {
            // Exit
            freeDataStructures();
            exit(1);
        }
        break;
        case 1:
        {
            // Removing Comments
            removeComments(argv[1]);
            printf("Comments removed successfully\n");
            FILE *fp = fopen("cleanFile.txt", "r");
            if (fp == NULL)
            {
                printf("\nError in opening the cleanFile.txt\n");
                exit(1);
            }
            char c;
            while ((c = fgetc(fp)) != EOF)
            {
                printf("%c", c);
            }
            fclose(fp);
        }
        break;
        case 2:
        {
            // Running Lexer Only
            lexerOnly(argv[1]);
        }
        break;
        case 3:
        {
            // Run Lexer and Parser
           
        }
        break;
        case 4:
        {
            // Running Lexer and Parser and printing time
            
        }
        break;
        default:
            printf("Invalid input. Please enter again\n");
        }
    }
}