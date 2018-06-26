/*
//  C Little Angel (cla)®
//
//  cminus.l
//  Lex specification for C-.
//
//  C- compiler based on Compiler Construction Principles and Practice by
//  Kenneth C. Louden implementation.
//
//  Created by Pedro Manhez Naresi on 6/9/17.
//  Copyright © 2018 Pedro Manhez Naresi. All rights reserved.
*/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
int comment = 0;
char tokenString[MAXTOKENLEN+1];
static int yylex(void);
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"else"          {return ELSE;}
"if"            {return IF;}
"int"           {return INT;}
"return"        {return RETURN;}
"void"          {return VOID;}
"while"         {return WHILE;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"<"             {return LT;}
"<="            {return LTEQ;}
">"             {return GT;}
">="            {return GTEQ;}
"="             {return ASSIGN;}
"!="            {return NEQ;}
"=="            {return EQ;}
";"             {return SEMI;}
","             {return COMMA;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"["             {return LBRACK;}
"]"             {return RBRACK;}
"{"             {return LBRACE;}
"}"             {return RBRACE;}
{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"            { char c;
                  do {
                    c = input();
                    if (c == EOF || c == 0)
                      break;
                    if (c == '\n')
                      lineno++;
                    if (c == '*')
                      comment = 1;
                    else if (c == '/' && comment == 1)
                      comment = 2;
                    else
                      comment = 0;
                  } while (comment != 2);
                }
.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}
