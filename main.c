#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"

// Declare external variables from parser.c
extern table T;
extern int syntactically_correct;

int main(int argc, char *argv[]) {
    int command;

    while (1) {
        printf("Enter 0 to exit, 1 to remove comments, 2 to print tokens, 3 to get parse, 4 to get time\n");
        scanf("%d", &command);

        switch (command) {
            case 0:
                // Exit
                freeDataStructures();
                exit(1);
                break;

            case 1:
                // Removing Comments
                removeComments(argv[1]);
                printf("Comments removed successfully\n");
                FILE *fp = fopen("cleanFile.txt", "r");
                if (fp == NULL) {
                    printf("\nError in opening the cleanFile.txt\n");
                    exit(1);
                }
                char c;
                while ((c = fgetc(fp)) != EOF) {
                    printf("%c", c);
                }
                fclose(fp);
                break;

            case 2:
                // Running Lexer Only
                lexerOnly(argv[1]);
                break;

            case 3:
                // Running Lexer and Parser
                parser(argv[1], argv[2]);
                break;

                case 4: {
                    // Timing functionality for lexer and parser
                    clock_t start_time, end_time;
                    double total_CPU_time, total_CPU_time_in_seconds;
                
                    // Start the timer
                    start_time = clock();
                
                    // Invoke lexer and parser to verify syntactic correctness
                    parseTree root = parseInputSourceCode(argv[1], T); // Run lexer and parser
                
                    // End the timer
                    end_time = clock();
                
                    // Calculate CPU time taken
                    total_CPU_time = (double)(end_time - start_time);
                    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
                
                    // Check if the code is syntactically correct
                    if (syntactically_correct) {
                        printf("Code is syntactically correct.\n");
                        printParseTree(root, argv[2]); // Print parse tree if syntax is correct
                        freeParseTree(root);          // Free memory allocated to parse tree
                    } else {
                        printf("Code is syntactically incorrect. Parse tree could not be constructed.\n");
                    }
                
                    // Print timing results
                    printf("\nTotal CPU time taken: %.2lf clock ticks\n", total_CPU_time);
                    printf("Total CPU time in seconds: %.4lf seconds\n", total_CPU_time_in_seconds);
                
                    break;
                }
                
            

            default:
                printf("Invalid input. Please enter again\n");
        }
    }

    return 0;
}
