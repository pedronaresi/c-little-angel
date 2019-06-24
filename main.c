//  C Little Angel (cla)®
//
//  main.c
//  Main program for C- compiler.
//
//  C- compiler based on Compiler Construction Principles and Practice by
//  Kenneth C. Louden implementation.
//
//  Created by Pedro Manhez Naresi on 6/9/17.
//  Copyright © 2018 Pedro Manhez Naresi. All rights reserved.

#include "globals.h"
// Set NO_PARSE to TRUE to get a scanner-only compiler.
#define NO_PARSE FALSE

// Set NO_ANALYZE to TRUE to get a parser-only compiler.
#define NO_ANALYZE FALSE

// Set NO_INTERMIDIATE_CODE to TRUE to get a compiler that does not generate code.
#define NO_INTERMIDIATE_CODE FALSE

// Set NO_OBJECT_CODE to TRUE to get a compiler that does not generate
// object code.
#define NO_OBJECT_CODE FALSE

// Set NO_BINARY_CODE to TRUE to get a compiler that does not generate binary
// code.
#define NO_BINARY_CODE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_INTERMIDIATE_CODE
#include "cgen.h"
#if !NO_OBJECT_CODE
#include "assembly.h"
#if !NO_BINARY_CODE
#include "binary.h"
#endif
#endif
#endif
#endif
#endif

// Allocate global variables
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

// Allocate and set tracing flags
int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;
int TraceTarget = TRUE;
int TraceBinary = TRUE;

//Error flags
int Error = FALSE;

// Control flags for Synthesis
int intermediateCodeGenerated = FALSE;
int objectCodeGenerated = FALSE;
int binaryCodeGenerated = FALSE;

int main(int argc, char * argv[]) {
  TreeNode * syntaxTree;
  char pgm[120]; /* Source code file name */
  if (argc != 2) {
    fprintf(stderr,"usage: %s <filename>\n",argv[0]);
    exit(1);
    }
  strcpy(pgm,argv[1]) ;
  if (strchr (pgm, '.') == NULL)
    strcat(pgm,".cmin");
  source = fopen(pgm,"r");
  if (source==NULL) {
    fprintf(stderr,"File %s not found\n",pgm);
    exit(1);
  }
  listing = stdout; /* Send listing to screen */
  fprintf(listing,"\nC- COMPILATION: %s\n",pgm);
  #if NO_PARSE
    while (getToken()!=ENDFILE);
  #else
    syntaxTree = parse();
    if (TraceParse) {
      fprintf(listing,"\nSyntax tree:\n");
      printTree(syntaxTree);
    }
  #if !NO_ANALYZE
    if (!Error) {
      if (TraceAnalyze)
        fprintf(listing,"\nBuilding Symbol Table...\n");
      buildSymtab(syntaxTree);
      if (TraceAnalyze)
        fprintf(listing,"\nChecking Types...\n");
      typeCheck(syntaxTree);
      if (TraceAnalyze)
        fprintf(listing,"\nType Checking Finished\n");
    }
  #if !NO_INTERMIDIATE_CODE
    if (!Error) {
      char * codefile;
      int fnlen = strcspn(pgm,".");
      codefile = (char *) calloc(fnlen+4, sizeof(char));
      strncpy(codefile, pgm, fnlen);
      strcat(codefile,".txt");
      code = fopen(codefile,"w");
      if (code == NULL) {
        printf("Unable to open %s\n",codefile);
        exit(1);
      }
      if (TraceCode)
        fprintf(listing, "\nGenerating Intermediate Code...\n");
      codeGen(syntaxTree, codefile);
      fclose(code);
      if (TraceCode)
        fprintf(listing, "\nIntermediate Code generation finished!\n");
      intermediateCodeGenerated = TRUE;
    }
  #if !NO_OBJECT_CODE
  if(intermediateCodeGenerated) {
    char * codefile;
    int fnlen = strcspn(pgm, ".");
    codefile = (char *) calloc(fnlen + 4, sizeof(char));
    strncpy(codefile, pgm, fnlen);
    strcat(codefile, ".txt");
    code = fopen(codefile, "a+");
    Quadruple codigoIntermediario = getCodigoIntermediario();
    if (TraceTarget)
      fprintf(listing, "\nGenerating Object Code...\n");
    geraCodigoObjeto(codigoIntermediario);
    fclose(code);
    if (TraceTarget)
      fprintf(listing, "\nObject Code generation finished!\n");
    // Código objeto gerado com sucesso
    objectCodeGenerated = TRUE;
  }
  #if !NO_BINARY_CODE
  if(objectCodeGenerated) {
    char * codefile;
    int fnlen = strcspn(pgm, ".");
    codefile = (char *) calloc(fnlen + 4, sizeof(char));
    strncpy(codefile, pgm, fnlen);
    strcat(codefile, ".txt");
    code = fopen(codefile, "a+");
    Objeto codigoObjeto = getCodigoObjeto();
    if (TraceBinary)
      fprintf(listing, "\nGenerating Binary Code...\n");
    geraCodigoBinario(codigoObjeto);
    fclose(code);
    if (TraceBinary)
      fprintf(listing, "\nBinary Code generation finished!\n\n");
  }
  #endif
  #endif
  #endif
  #endif
  #endif
  fclose(source);
  return 0;
}
