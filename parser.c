#include "parser.h"
#include "parserDef.h"
#include <stdio.h>

Grammar getGrammarFromFile(char** file_name)
{
	Grammar grammar;
	FILE *fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(*file_name, "r");
	if (fp == NULL)
		printf("Couldn't open grammar file\n");exit(1); 

	while ((read = getline(&line, &len, fp)) != -1) {
		// printf("Retrieved line of length %zu :\n", read);
		// printf("%s", line);
		// TODO: Add line processing using strtok()
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