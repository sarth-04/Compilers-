# LL(1) Parser-Based Compiler  

This repository contains a compiler built from scratch using C, featuring an LL(1) parser that successfully parses the specified language. The project was developed for the **Compiler Construction and Design** course.  

## Requirements  
- **GCC Version**: 11.4.0  
- **Operating System**: Ubuntu 22.04 (via WSL)  

## File and Function Descriptions  

### **Lexer (lexer.c)**  
#### **Functions:**  
- `FILE *getStream(FILE *fp)`:  
  - Reads source code from the given file (`fp`) and efficiently loads it into a twin buffer.  
  - Ensures minimal I/O operations by maintaining the file pointer and dynamically loading data as needed.  

- `tokenInfo getNextToken(twinBuffer B)`:  
  - Reads character streams and recognizes lexemes.  
  - Performs lexical analysis, returns token information, and detects lexical errors.  

- `removeComments(char *testcaseFile, char *cleanFile)`:  
  - Removes comments from the source code and writes the cleaned version to `cleanFile`.  
  - Lexer ignores comments while tokenizing input, but this function is provided for demonstration purposes.  

### **Parser (parser.c)**  
#### **Functions:**  
- `FirstAndFollow ComputeFirstAndFollowSets(grammar G)`:  
  - Computes **FIRST** and **FOLLOW** sets for the given grammar `G`.  
  - Can be automated or manually populated based on project needs.  

- `createParseTable(FirstAndFollow F, table T)`:  
  - Uses computed FIRST and FOLLOW sets to construct the predictive parsing table `T`.  

- `parseInputSourceCode(char *testcaseFile, table T)`:  
  - Parses the given source code file using the LL(1) parsing table.  
  - Generates a **parse tree**, reports syntax errors with line numbers, and confirms successful parsing with a message.  

- `printParseTree(parseTree PT, char *outfile)`:  
  - Outputs the parse tree in **inorder traversal** format to `outfile`.  
  - Each line contains the following structured data:  
    ```
    lexeme CurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol
    ```
  - Ensures clean formatting for readability.  

## **Header and Additional Files**  

- **lexerDef.h** → Data definitions used in `lexer.c`  
- **lexer.h** → Function prototypes for `lexer.c`  
- **parserDef.h** → Data structures for grammar, parsing table, and parse tree  
- **parser.h** → Function prototypes for `parser.c`  
- **driver.c** → Main execution flow (details will be uploaded soon)  
- **makefile** → Automates compilation using GNU Make  


