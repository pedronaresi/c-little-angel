//  C Little Angel (cla)®
//
//  binary.h
//  Binary code generator interface for C- compiler.
//
//  C- compiler based on Compiler Construction Principles and Practice by
//  Kenneth C. Louden implementation.
//
//  Created by Pedro Manhez Naresi on 6/9/18.
//  Copyright © 2018 Pedro Manhez Naresi. All rights reserved.

#ifndef _BINARY_H_
#define _BINARY_H_

#include "assembly.h"

void geraCodigoBinario(Objeto codigoObjeto);

const char * toBinaryOpcode(Opcode op);

#endif
