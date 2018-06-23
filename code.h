//  C Little Angel (cla)®
//
//  cgen.c
//  Code emitting utilities for the C- compiler and interface to the TM machine.
//
//  C- compiler based on Compiler Construction Principles and Practice by
//  Kenneth C. Louden implementation.
//
//  Created by Pedro Manhez Naresi on 6/9/18.
//  Copyright © 2018 Pedro Manhez Naresi. All rights reserved.

#ifndef _CODE_H_
#define _CODE_H_

#include "cgen.h"

/* code emitting utilities */

/* Procedimento emitComment imprime uma linha de comentario
 * com a string c no aquivo
 */
void emitSpaces(int indent);

void emitComment(const char * c, int indent);

void emitCode(const char * c);

/* Procedimento toString retorna o mnemônico do enum da instrução
 * passado como parâmetro
 */
const char * toString(enum instrucao i);

void emitObjectCode(const char * c, int indent);

#endif
