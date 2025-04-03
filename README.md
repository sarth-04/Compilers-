# LL(1) Parser-Based Compiler  

This project implements a **compiler with an LL(1) parser**, built from scratch using C. It performs **lexical analysis, syntax analysis, and parsing** for a given language. This project was developed for the **Compiler Construction and Design** course.  

## Requirements  
- **GCC Version**: 11.4.0  
- **Operating System**: Ubuntu 22.04 (via WSL)  

## **Overview of Lexer and Parser**  

### **Lexer (Lexical Analyzer)**  
- Reads the source code and **tokenizes** it into meaningful components.  
- Uses **buffering techniques** to efficiently process large files.  
- Detects and reports **lexical errors** like unrecognized symbols.  
- Ignores comments while extracting tokens for the parser.  

### **Parser (Syntax Analyzer)**  
- Uses **LL(1) parsing** to verify the syntax of the source code.  
- Builds a **parse tree** based on predefined grammar rules.  
- Computes **FIRST and FOLLOW** sets to generate a **predictive parsing table**.  
- Reports **syntax errors** with precise line numbers.  
- Outputs a structured **parse tree** for further analysis.  

## **File Structure**  

- **lexer.c** → Implements the lexical analyzer  
- **parser.c** → Implements the LL(1) parser  
- **lexerDef.h / lexer.h** → Lexer-related data structures and function prototypes  
- **parserDef.h / parser.h** → Parser-related data structures and function prototypes  
- **driver.c** → Main program execution  
- **makefile** → Automates compilation  

