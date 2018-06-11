//  C Little Angel (cla)®
//
//  symtab.c
//  Symbol table implementation for the C- compiler.
//
//  C- compiler based on Compiler Construction Principles and Practice by
//  Kenneth C. Louden implementation.
//
//  Created by Pedro Manhez Naresi on 6/9/17.
//  Copyright © 2018 Pedro Manhez Naresi. All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symtab.h"

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

#define MAX_SCOPE 20

#define ESCOPO_GLOBAL 0
#define ESCOPO_NAO_GLOBAL 1

static Scope scopes[MAX_SCOPE];
static int nScope = 0;
static Scope scopeStack[MAX_SCOPE];
static int nScopeStack = 0;

/* the hash function */
static int hash (char * key) {
    int temp = 0;
    int i = 0;
    while (key[i] != '\0') {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

/* Função toString do enum ExpType */
const char * expTypeToString(ExpType e) {
    const char * strings[] = {"Void", "Integer"};
    return strings[e];
}

const char * dataTypeToString(ExpKind k) {
    if(k == IdK) {
        return "Variable";
    } else if(k == VectorK) {
        return "Array";
    } else {
        return "Function";
    }
}

const char * varOrigemToString(VarMemK k) {
    if(k == ParamK) {
        return "Parameters";
    } else if(k == LocalK) {
        return "Local";
    } else if(k == GlobalK){
        return "Global";
    } else {
        return "-";
    }
}

Scope sc_top(void) {
    return scopeStack[nScopeStack - 1];
}

void sc_pop(void) {
    --nScopeStack;
}

void incScope() {
    ++nScopeStack;
}

void sc_push(Scope scope) {
    scopeStack[nScopeStack++] = scope;
}

Scope sc_create(char * funcName) {
    Scope newScope = (Scope) malloc(sizeof(struct ScopeRec));
    newScope->funcName = funcName;
    newScope->tamanhoBlocoMemoria = 0;
    if(!strcmp(funcName, "Global Scope")) {
        newScope->parent = NULL;
    } else {
        newScope->parent = globalScope;
    }
    scopes[nScope++] = newScope;
    return newScope;
}

Scope st_scopeVar(char * name) {
    int h = hash(name);
    Scope sc = sc_top();
    while(sc != NULL) {
        BucketList l = sc->hashTable[h];
        while ((l != NULL) && (strcmp(name, l->name))) {
            l = l->next;
        }
        if (l != NULL) return sc;
        sc = sc->parent;
    }
    return NULL;
}

BucketList st_bucket(char * name) {
    int h = hash(name);
    Scope sc = sc_top();
    while(sc != NULL) {
        BucketList l = sc->hashTable[h];
        while ((l != NULL) && (strcmp(name,l->name))) {
            l = l->next;
        }
        if (l != NULL) return l;
        sc = sc->parent;
    }
    return NULL;
}

int verifyGlobalScope(TreeNode * treeNode) {
    int h = hash(treeNode->attr.name);
    Scope sc = globalScope;
    BucketList l = sc->hashTable[h];
    while ((l != NULL) && (strcmp(treeNode->attr.name, l->name))) {
        l = l->next;
    }
    if (l != NULL) {
        return TRUE;
    } else {
        return FALSE;
    }
    return 0;
}

/**
 *  Procura por uma função no escopo global
 */
BucketList st_bucket_func (char * name) {
	int h = hash(name);
	Scope sc = globalScope;
	BucketList l = sc->hashTable[h];
	while ((l != NULL) && (strcmp(name,l->name))) {
        l = l->next;
    }
    if (l != NULL) {
        return l;
    } else {
        return NULL;
    }
}

BucketList getVarFromSymtab(char * nome, Scope escopo) {
    int h = hash(nome);
    BucketList l = escopo->hashTable[h];
    while (l != NULL) {
        /* !strcmp(const char *s1, const char *s2) verifica se as duas
         * strings passadas como parâmetro são iguais. Retorna 0 em caso
         * verdadeiro, por isso o símbolo '!' antes da função
         */
        if(!strcmp(l->name, nome)) {
            return l;
        }
        l = l->next;
    }
    return NULL;
}

int getMemoryLocation(char * nome, Scope escopo) {
    int h = hash(nome);
    BucketList l = escopo->hashTable[h];
    while (l != NULL) {
        /* !strcmp(const char *s1, const char *s2) verifica se as duas
         * strings passadas como parâmetro são iguais. Retorna 0 em caso
         * verdadeiro, por isso o símbolo '!' antes da função
         */
        if(!strcmp(l->name, nome)) {
            return l->memloc;
        }
        l = l->next;
    }
    return -1;
}

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert(char * name, int lineno, int loc, TreeNode * treeNode, int tamanho) {
	int h = hash(name);
	Scope top = sc_top();

    if(top->hashTable[h] == NULL) {
        // Adiciona o escopo ao nó da árvore sintática
        treeNode->scope = top;
        top->hashTable[h] = st_create(name, lineno, loc, treeNode, tamanho);
        treeNode->scope->tamanhoBlocoMemoria += tamanho;
        return;
    } else {
        BucketList l = top->hashTable[h];
        while ((l->next != NULL) && (strcmp(name, l->name))) {
            l = l->next;
        }
        if (l->next == NULL && (strcmp(name, l->name))) { /* Variável ainda não existente na tabela */
            // Adiciona o escopo ao nó da árvore sintática
            treeNode->scope = top;
            // Adiciona um novo item na tabela de símbolos
            l->next = st_create(name, lineno, loc, treeNode, tamanho);
            treeNode->scope->tamanhoBlocoMemoria += tamanho;
        }
    }
} /* st_insert */

BucketList st_create(char * name, int lineno, int loc, TreeNode * treeNode, int tamanho) {
    BucketList l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->lines->next = NULL;
    l->memloc = loc;
    l->tamanho = tamanho;
    l->treeNode = treeNode;
    l->next = NULL;
    return l;
} /* st_create */

void st_add_lineno(TreeNode * treeNode) {
    // Adiciona o escopo ao nó da árvore sintática
    treeNode->scope = st_scopeVar(treeNode->attr.name);
    int lineno = treeNode->lineno;
	BucketList l = st_bucket(treeNode->attr.name);
  	LineList ll = l->lines;
  	while (ll->next != NULL) {
		ll = ll->next;
    }
	if (ll->lineno != lineno) {
	 	ll->next = (LineList) malloc(sizeof(struct LineListRec));
	  	ll->next->lineno = lineno;
	  	ll->next->next = NULL;
	}
}

void st_insert_func(char * name, int lineno, TreeNode * treeNode) {
    int h = hash(name);
	Scope top = globalScope;
  	BucketList l = top->hashTable[h];

  	while ((l != NULL) && (strcmp(name,l->name) != 0)) {
    	l = l->next;
    }
  	if (l == NULL) { /* Variável ainda não existente na tabela */
        // Adiciona o escopo ao nó da árvore sintática
        treeNode->scope = top;
        // Adiciona um novo item na tabela de símbolos
  		l = (BucketList) malloc(sizeof(struct BucketListRec));
    	l->name = name;
    	l->lines = (LineList) malloc(sizeof(struct LineListRec));
	    l->lines->lineno = lineno;
        l->lines->next = NULL;
        l->memloc = -1;
        l->tamanho = 0;
        l->treeNode = treeNode;
	    l->next = top->hashTable[h];
	   	top->hashTable[h] = l;
	}
}

/* Function st_lookup returns the memory
 * location of a variable or -1 if not found
 */
int st_lookup (char * name) {
    BucketList l = st_bucket(name);
    if (l != NULL) return l->memloc;
    return -1;
}

/**
 *  Procura por uma função no escopo global
 */
int st_lookup_func (char * name ) {
	int h = hash(name);
	Scope sc = globalScope;
	BucketList l = sc->hashTable[h];
	while ((l != NULL) && (strcmp(name,l->name))) {
        l = l->next;
    }
    if (l != NULL) {
        return TRUE;
    } else {
        return FALSE;
    }
}

int st_lookup_top (char * name) {
    int h = hash(name);
    Scope sc = sc_top();
    BucketList l = sc->hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name))) {
        l = l->next;
    }
    if (l != NULL) {
        return l->memloc;
    } else {
        return -1;
    }
}

int getQuantidadeParametros(TreeNode * functionNode) {
    int qtd = 0;
    TreeNode * temp = functionNode->child[0];
    if(temp != NULL) {
        ++qtd;
        while(temp->sibling != NULL) {
            temp = temp->sibling;
            ++qtd;
        }
    }
    return qtd;
}

int getQuantidadeVariaveis(TreeNode * functionNode) {
    int qtd = 0;
    TreeNode * temp = functionNode->child[1]->child[0];
    if(temp != NULL) {
        ++qtd;
        while(temp->sibling != NULL) {
            temp = temp->sibling;
            ++qtd;
        }
    }
    return qtd;
}

int getTamanhoBlocoMemoriaEscopo(char * scopeName) {
    int i, tamanho = 0;
    for (i = 0; i < nScope; ++i) {
        if(!strcmp(scopeName, scopes[i]->funcName)) {
            tamanho = scopes[i]->tamanhoBlocoMemoria;
            break;
        }
    }
    return tamanho;
}

BucketList verificaGlobal(char * name) {
    int j;
    Scope global = scopes[0];
    BucketList * hashTable = global->hashTable;
    for (j = 0; j < SIZE; ++j) {
		if (hashTable[j] != NULL) {
			BucketList l = hashTable[j];
      		while (l != NULL) {
                if(!strcmp(name, l->name)) {
                    return l;
                }
                l = l->next;
            }
        }
    }
    return NULL;
}

int getTamanhoBlocoMemoriaEscopoGlobal(void) {
    int j, tamanho = 0;
    Scope global = scopes[0];
    BucketList * hashTable = global->hashTable;
    for (j = 0; j < SIZE; ++j) {
		if (hashTable[j] != NULL) {
			BucketList l = hashTable[j];
      		while (l != NULL) {
                if(strcmp("Funcao", dataTypeToString(l->treeNode->kind.exp))) {
                    tamanho += l->tamanho;
                }
                l = l->next;
            }
        }
    }
    return tamanho;
}

void printSymTabRows(BucketList *hashTable, FILE *listing, int escopo) {
	int j;
	for (j = 0; j < SIZE; ++j) {
		if (hashTable[j] != NULL) {
			BucketList l = hashTable[j];

      		while (l != NULL) {
				LineList t = l->lines;
        		fprintf(listing, "%-18s", l->name);
				fprintf(listing, "%-10s", expTypeToString(l->treeNode->type));
                fprintf(listing, "%-12s", dataTypeToString(l->treeNode->kind.exp));

                /*
                 * Verifica se o item armazenado nessa posição da tabela de
                 * símbolos é uma função. Se for, imprime também o número de
                 * parâmetros
                 */
                if(escopo == ESCOPO_GLOBAL) {
                    if(strcmp("Funcao", dataTypeToString(l->treeNode->kind.exp)) == 0) {
                        int numParams = getQuantidadeParametros(l->treeNode);
                        fprintf(listing, "%-15d", numParams);

                        if(l->treeNode->child[0] == NULL) {
                            fprintf(listing, "%-17s", "void");
                        } else {
                            fprintf(listing, "%-17s", "int");
                        }
                    } else {
                        l->treeNode->varMemK = GlobalK;
                        fprintf(listing, "%-32s", "");
                    }
                }

                fprintf(listing, "%-17s", varOrigemToString(l->treeNode->varMemK));
                fprintf(listing, "%-9d", l->tamanho);

                if(l->memloc == -1) {
                    fprintf(listing, "%-8c", '-');
                } else {
                    fprintf(listing, "%-8d", l->memloc);
                }

                while (t != NULL) {
					if (t->lineno != -1) {
						fprintf(listing, "%-4d", t->lineno);
					} else {
						fprintf(listing, " ");
					}
          			t = t->next;
        		}
                fprintf(listing, "\n");
       			l = l->next;
     		}
   		}
  	}
}

/* Procedure printSymTab prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void printSymTab(FILE * listing) {
	int i;
	for (i = 0; i < nScope; ++i) {
		Scope scope = scopes[i];
		BucketList * hashTable = scope->hashTable;

		if (i == 0) { // Escopo global
			fprintf(listing, "<Global Escope>\n");
            fprintf(listing, "Nome da variavel  Tipo ID   Tipo dados  Nº parametros  Tipo parametros  Origem Variavel  Tamanho  MemLoC  Numero das linhas\n");
      		fprintf(listing, "----------------  --------  ----------  -------------  ---------------  ---------------  -------  ------  -----------------\n");
            printSymTabRows(hashTable, listing, ESCOPO_GLOBAL);
		} else {
            if(!strcmp(scope->funcName, "input") || !strcmp(scope->funcName, "output")) {
                continue;
            }
            fprintf(listing, "Function name: %s\n", scope->funcName);
            fprintf(listing, "Nome da variavel  Tipo ID   Tipo dados  Origem Variavel  Tamanho  MemLoc  Numero das linhas\n");
            fprintf(listing, "----------------  --------  ----------  ---------------  -------  ------  -----------------\n");
            printSymTabRows(hashTable, listing, ESCOPO_NAO_GLOBAL);
		}

		fputc('\n', listing);
	}
} /* printSymTab */
