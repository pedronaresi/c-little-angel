//  C Little Angel (cla)®
//
//  cgen.c
//  TM Code emitting utilities implementation for the C- compiler.
//
//  C- compiler based on Compiler Construction Principles and Practice by
//  Kenneth C. Louden implementation.
//
//  Created by Pedro Manhez Naresi on 6/9/18.
//  Copyright © 2018 Pedro Manhez Naresi. All rights reserved.

#include "globals.h"
#include "code.h"

/* Função toString que retorna o mnemônico da instrução
 * conforme o enum de parâmetro
 */
const char * toString(enum instrucao i) {
    const char * strings[] = {  "addition", "subtraction", "multiplication", "division",
                   "vector_value", "vector_address",
                   "equal", "not_equal", "less_than", "less_than_equal_to",
                   "greater_than", "greater_than_equal_to", "assign",
                   "function", "return", "get_param", "set_param", "call", "param_list",
                   "jump_if_false", "goto", "label", "halt"};
    return strings[i];
}

void emitSpaces(int indent){
    int i;
    for(i = 0; i < indent; ++i) {
        fprintf(code, " ");
    }
}

void emitComment(const char * c, int indent) {
    if (TraceCode) {
        emitSpaces(indent);
        fprintf(code, "# %s\n", c);
    }
}

void emitCode(const char * c) {
    fprintf(code, "%s\n", c);
}

void emitObjectCode(const char * c, int indent) {
    emitSpaces(indent);
    fprintf(code, "%s\n", c);
}
