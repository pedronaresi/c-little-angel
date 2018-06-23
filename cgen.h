//  C Little Angel (cla)®
//
//  cgen.h
//  The code generator interface to the C- compiler.
//
//  C- compiler based on Compiler Construction Principles and Practice by
//  Kenneth C. Louden implementation.
//
//  Created by Pedro Manhez Naresi on 6/9/18.
//  Copyright © 2018 Pedro Manhez Naresi. All rights reserved.

#include "globals.h"

#ifndef _CGEN_H_
#define _CGEN_H_

typedef enum {Empty, IntConst, String} OperandKind;

typedef enum instrucao {ADD, SUB, MULT, DIV,
      VEC, VEC_ADDR,
      EQUAL, NE, LTHAN, LET, GTHAN, GET, ASN,
      FUNC, RTN, GET_PARAM, SET_PARAM, CALL, PARAM_LIST,
      JPF, GOTO, LBL, HALT} InstructionKind;

typedef struct  {
    OperandKind kind;
    union {
        int val;
        struct {
            char * name;
            struct ScopeRec * scope;
        } variable;
    } contents;
} Operand;

/* Estrutura Quádrupla que armazena os dados do código
 * de três endereços
 */
typedef struct Quad {
    InstructionKind instruction;
    Operand op1, op2, op3;
    int linha;
    int display;
    struct Quad * next;
} * Quadruple;

typedef struct Location {
    Quadruple * quad;
    struct Location * next;
} * LocationStack;

typedef struct Param {
    int * count;
    struct Param * next;
} * ParamStack;

Quadruple * insertQuad(Quadruple q);

Quadruple createQuad(InstructionKind instruction, Operand op1, Operand op2, Operand op3);

void pushLocation(LocationStack ls);

void popLocation();

LocationStack createLocation(Quadruple * quad);

void updateLocation(Operand op);

void pushParam(int * count);

void popParam();

void preparaVazio();

void printIntermediateCode();

Quadruple getCodigoIntermediario(void);

/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode * syntaxTree, char * codefile);

void verificaFimInstrucaoAnterior(void);

#endif
