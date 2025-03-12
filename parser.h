#include "parserDef.h"

FirstAndFollow ComputeFirstAndFollowSet(Grammar G);

void createParseTable(FirstAndFollow f_set, ParseTable T);

ParseTree parseInputSourceCode(char *testcaseFile, ParseTable T);

void PrintParseTree(ParseTree T, char* outfile);