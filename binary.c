//  C Little Angel (cla)®
//
//  binary.c
//  Binary code generator implementation for C- compiler.
//
//  C- compiler based on Compiler Construction Principles and Practice by
//  Kenneth C. Louden implementation.
//
//  Created by Pedro Manhez Naresi on 6/9/18.
//  Copyright © 2018 Pedro Manhez Naresi. All rights reserved.

#include "assembly.h"
#include "code.h"
#include "util.h"

char temp[100];

const char * toBinaryOpcode(Opcode op) {
    const char * strings[] = {
        // "add", "addi", "sub", "subi", "mul", "div", "and"
        "000000", "001011", "000001", "001100", "000110", "000111", "000010",
        // "or", "xor", "not", "sll", "srl", "mov", "lw"
        "000011", "000100", "010001", "010010", "010011", "100000", "001111",
        // "li", "la", "sw", "beq", "bne", "blt", "blet",
        "100001", "010001", "010000", "001001", "001010", "011100", "011101",
        // "bgt", "bge", "j", "jal", "jr", "nop", "hlt",
        "011110", "011111", "010111", "011010", "011011", "011100", "011101",
        //  "reset", "jr", "nop", "halt", "reset", "in", "out"
        "011001", "111111", "111110", "011000", "010101", "010101", "010110"
    };
    return strings[op];
}

const char * toBinaryRegister(RegisterName rn) {
    const char * strings[] = {
        // "$rz", "$v0",  "$out",  "$sp",  "$inv",   "$gp",   "$fp",   "$a0",
        "00000", "00001", "00010", "00011", "00100", "00101", "00110", "00111",
        // "$a1", "$a2",  "$a3",   "$s0",   "$s1",   "$s2",   "$s3",   "$s4",
        "01000", "01001", "01010", "01011", "01100", "01101", "01110", "01111",
        // "$s5", "$s6",  "$s7",   "$s8",   "$s9",   "$t0",   "$t1",   "$t2",
        "10000", "10001", "10010", "10011", "10100", "10101", "10110", "10111",
        // "$t3", "$t4",  "$t5",   "$t6",   "$t7",   "$t8",   "$t9",   "$ra"
        "11000", "11001", "11010", "11011", "11100", "11101", "11110", "11111"
    };
    return strings[rn];
}

const char * getZeros(int n) {
    int i = 0;
    char * zeros = (char *) malloc(n + 1);
    while(i < n) {
        zeros[i++] = '0';
    }
    zeros[i] = '\0';
    return zeros;
}

/* Função que converte um número decimal para uma string binária em complemento
 * de 2. Essa função não trata casos de overflow!!! Fica a cargo do programador
 * alocar a quantidade de bits suficiente para a conversão
 */
const char * decimalToBinaryStr(unsigned x, int qtdBits) {
    int i = 0;
    qtdBits--;
    char * bin = (char *) malloc(qtdBits + 1);
    for (unsigned bit = 1u << qtdBits; bit != 0; bit >>= 1) {
        bin[i++] = (x & bit) ? '1' : '0';
    }
    bin[i] = '\0';
    return bin;
}

void geraCodigoBinario(Objeto codigoObjeto) {
    emitCode("\n********** Código binário **********\n");
    Objeto obj = codigoObjeto;
    char str[26];
    int linha = 0;

    // Limpa o vetor de caracteres auxiliar
    memset(temp, '\0', sizeof(temp));
    // Boilerplate
    strcat(temp, "instructionRAM[");
    sprintf(str, "%d", linha++);
    strcat(temp, str);
    strcat(temp, "] = 32'b");
    strcat(temp, toBinaryOpcode(_JUMP));
    strcat(temp, "_");
    strcat(temp, decimalToBinaryStr(getLinhaLabel((char*) "main"), 26));
    strcat(temp, "; \t// Jump to Main");
    emitCode(temp);

    while(obj != NULL) {
        // Limpa o vetor de caracteres auxiliar
        memset(temp, '\0', sizeof(temp));
        // Boilerplate
        strcat(temp, "instructionRAM[");
        sprintf(str, "%d", linha++);
        strcat(temp, str);
        strcat(temp, "] = 32'b");

        // Traduz o opcode para binário
        strcat(temp, toBinaryOpcode(obj->opcode));
        strcat(temp, "_");

        switch(obj->type) {
            case TYPE_R:
                strcat(temp, toBinaryRegister(obj->op2->enderecamento.registrador));
                strcat(temp, "_");
                strcat(temp, toBinaryRegister(obj->op3->enderecamento.registrador));
                strcat(temp, "_");
                strcat(temp, toBinaryRegister(obj->op1->enderecamento.registrador));
                strcat(temp, "_");
                strcat(temp, getZeros(11));
                break;
            case TYPE_I:
                if(obj->opcode == _LOADI) {
                    strcat(temp, getZeros(5));
                    strcat(temp, "_");
                    strcat(temp, toBinaryRegister(obj->op1->enderecamento.registrador));
                    strcat(temp, "_");
                    strcat(temp, decimalToBinaryStr(obj->op2->enderecamento.imediato, 16));
                    break;
                } else if(obj->opcode == _BLT || obj->opcode == _BLET || obj->opcode == _BGT || obj->opcode == _BGET
                    || obj->opcode == _BEQ || obj->opcode == _BNE) {
                        strcat(temp, toBinaryRegister(obj->op1->enderecamento.registrador));
                        strcat(temp, "_");
                        strcat(temp, toBinaryRegister(obj->op2->enderecamento.registrador));
                        strcat(temp, "_");
                        strcat(temp, decimalToBinaryStr(getLinhaLabel(obj->op3->enderecamento.label), 16));
                        break;
                }

                if(obj->op2 == NULL) {
                    strcat(temp, getZeros(5));
                } else {
                    if(obj->op2->tipoEnderecamento == RegisterAddressing) {
                        strcat(temp, toBinaryRegister(obj->op2->enderecamento.registrador));
                    } else if(obj->op2->tipoEnderecamento == IndexedAddressing) {
                        strcat(temp, toBinaryRegister(obj->op2->enderecamento.indexado.registrador));
                        strcat(temp, "_");
                        strcat(temp, toBinaryRegister(obj->op1->enderecamento.registrador));
                        strcat(temp, "_");
                        strcat(temp, decimalToBinaryStr(obj->op2->enderecamento.indexado.offset, 16));
                        break;
                    }
                }
                strcat(temp, "_");

                if(obj->op1 == NULL) {
                    strcat(temp, getZeros(5));
                } else {
                    if(obj->op1->tipoEnderecamento == RegisterAddressing) {
                        strcat(temp, toBinaryRegister(obj->op1->enderecamento.registrador));
                    }
                }
                strcat(temp, "_");

                if(obj->op3 == NULL) {
                    strcat(temp, getZeros(16));
                } else {
                    if(obj->op3->tipoEnderecamento == ImmediateAddressing) {
                        strcat(temp, decimalToBinaryStr(obj->op3->enderecamento.imediato, 16));
                    } else if(obj->op3->tipoEnderecamento == LabelAddressing) {
                        strcat(temp, decimalToBinaryStr(getLinhaLabel(obj->op3->enderecamento.label), 16));
                    }
                }

                break;
            case TYPE_J:
                if(obj->opcode == _JUMP || obj->opcode == _JUMPAL) {
                    strcat(temp, decimalToBinaryStr(getLinhaLabel(obj->op1->enderecamento.label), 26));
                } else { // HALT, NOP
                    strcat(temp, getZeros(26));
                }
                break;
        }
        strcat(temp, "; \t// ");
        strcat(temp, toStringOpcode(obj->opcode));
        emitCode(temp);
        obj = obj->next;
    }
}
