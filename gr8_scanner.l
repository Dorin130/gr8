%option c++ prefix="gr8_scanner_" outfile="gr8_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "gr8_parser.tab.h"

#include <stack>
#include <iostream>

bool firstTime = true;
bool closeLogicLine = false;
int currentIndent = 0;
std::vector<int> indents;

// don't change this
#define yyerror LexerError

%}
%x X_STRING X_COMMENT X_ESCAPE X_NEWLINE X_CHECK_INDENT

IDENTIFIER 						[a-zA-Z][0-9a-zA-Z-]*
SYMBOL							[()?,]
SPACE      						[ \t]+

DIGI							[0-9]
EXPONENT          				[eE][-+]?{DIGI}+
HEPT             				[0-6]
NOTZERO                         0[0]+
INT7							0{HEPT}+
INT10							[1-9]{DIGI}*|0
REAL							{DIGI}+{EXPONENT}|{DIGI}+"."{DIGI}*{EXPONENT}?|{DIGI}*"."{DIGI}+{EXPONENT}?

%%
                                { /*set_debug(1);*/ if(firstTime) { firstTime = false; BEGIN(X_CHECK_INDENT) ; } }

        /* Regular tokens */

"small"							return tSMALL;
"huge"							return tHUGE;
"news"							return tNEWS;
"fake"							return tFAKE;
"initially"						return tINITIALLY;

"use"							return tUSE;
"public"						return tPUBLIC;
"define"						return tDEFINE;
"procedure"						return tPROCEDURE;
"function"						return tFUNCTION;
"on"							return tON;
"as"							return tAS;
"do"							return tDO;
"uses"							return tUSES;
"for"							return tFOR;
"return"						return tRETURN;

"plus"							return tPLUS;
"minus"							return tMINUS;
"times"							return tTIMES;
"over"							return tOVER;
"modulus"						return tMODULUS;
"not"							return tNOT;
"and"							return tAND;
"or"							return tOR;
"assign"						return tASSIGN;
"to"							return tTO;
"cell"							return tCELL;
"at"							return tAT;
"above"							return tABOVE;
"below"							return tBELOW;
"equals"						return tEQUALS;
"input"							return tINPUT;
"objects"						return tOBJECTS;

"if"							return tIF;
"then"							return tTHEN;
"elsif"							return tELSIF;
"else"							return tELSE;
"stop"							return tSTOP;
"again"							return tAGAIN;
"post"							return tPOST;
"tweet"							return tTWEET;
"sweeping"						return tSWEEPING;
"from"							return tFROM;
"by"							return tBY;
"null"                          return tNULL;

{SYMBOL}						{ return *yytext; }


        /* Special tokens treatment section */

{IDENTIFIER}					{ yylval.s = new std::string(yytext); return tIDENTIFIER; }
{REAL}               		 	{ yylval.d = strtod(yytext, nullptr); if(errno == ERANGE) yyerror("ERANGE (Overflow)"); return tREAL; }
{NOTZERO}                       { yyerror("Not a valid representation of zero"); }
{INT7}			  		 		{ yylval.i = strtol(yytext, nullptr, 7); if(errno == ERANGE) yyerror("ERANGE (Overflow)"); return tINTEGER; }
{INT10}               			{ yylval.i = strtol(yytext, nullptr, 10); if(errno == ERANGE) yyerror("ERANGE (Overflow)"); return tINTEGER;}


\"                     		  	{ yy_push_state(X_STRING); yylval.s = new std::string("");}
<X_STRING>\"            		{ yy_pop_state (); return tSTRING;}
<X_STRING>~			   		  	{ yy_push_state(X_ESCAPE);}
<X_STRING>\n|\0			   		{ yyerror("Invalid character in string (NUL, LF)"); } /* ask teacher if necessary; separate error messages if so */
<X_STRING>.         		  	{ *yylval.s += yytext; }

<X_ESCAPE>n						{ *yylval.s += "\n"; yy_pop_state();}
<X_ESCAPE>r		        		{ *yylval.s += "\r"; yy_pop_state();}
<X_ESCAPE>t		        		{ *yylval.s += "\t"; yy_pop_state();}
<X_ESCAPE>\"         			{ *yylval.s += "\""; yy_pop_state();}
<X_ESCAPE>~						{ *yylval.s += "~" ; yy_pop_state();}

<X_ESCAPE>{HEPT}{HEPT}?{HEPT}? 	{ if(strtol(yytext, nullptr, 7) < 256) *yylval.s += std::string(1, strtol(yytext, nullptr, 7)); else yyerror(std::string(std::to_string(yylineno) + ":Char not 8-bit!").c_str());  yy_pop_state();}
<X_ESCAPE>.						yyerror("Unknown character in string!");


        /* Comment treatment section */

"<<"							yy_push_state(X_COMMENT);
<X_COMMENT>"<<"					yy_push_state(X_COMMENT);
<X_COMMENT>">>"					yy_pop_state();
<X_COMMENT>.|\n 				; /* ignore comments */

"!!".*                  		; /* ignore comments */


        /* Whitespace and continuation section */

"..."\n                         { ; /* line continuation marker */}
{SPACE}* 		              	{ ; /* ignore whitespace that is not in the start of the line*/}

        /* Indentation trigger section */

\n                              { BEGIN(X_CHECK_INDENT); currentIndent=0; return ';' ; }   //end of logic line
<INITIAL><<EOF>>                { BEGIN(X_CHECK_INDENT); currentIndent=0; return ';' ; }   //end of logic line


        /* Indentation treatment section */

<X_CHECK_INDENT>!!.*            { ; /* ignore */}
<X_CHECK_INDENT><<              { yy_push_state(X_COMMENT); }
<X_CHECK_INDENT>[ ]             { currentIndent++; }
<X_CHECK_INDENT>[\t]            { currentIndent += 8; currentIndent -= currentIndent%8; }
<X_CHECK_INDENT>\n              { currentIndent=0; }

<X_CHECK_INDENT><<EOF>>         { if(indents.size()==0) return 0;   //if an empty file
                                  currentIndent=indents[0];         //baseline indentation
                                  if(currentIndent < indents.back()) {
                                    indents.pop_back();
                                    if(currentIndent > indents.back()) {
                                      yyerror(std::string(std::to_string(yylineno) + ": Unexpected indent: expected '" + std::to_string(indents.back()) + "' spaces").c_str());
                                    }
                                    return '}';
                                  } else if(currentIndent == indents.back()) {
                                    return 0;
                                  }
                                }

<X_CHECK_INDENT>.               { yyless(0);   //logic line start
                                  if(indents.size()==0) // first level of indentation
                                    indents.push_back(currentIndent);
                                  if(currentIndent < indents[0]) {
                                    yyerror(std::string(std::to_string(yylineno) + ": Unexpected indent: expected '" + std::to_string(indents.back()) + "' spaces").c_str());
                                    return 0;
                                  }// yyerror(std::string(std::to_string(yylineno) + ": Unexpected indent: expected at least '" + std::to_string(indents.back()) + "' spaces").c_str());
                                  if(currentIndent > indents.back()) {            //more indentation
                                    indents.push_back(currentIndent);
                                    BEGIN(INITIAL);
                                    return '{';
                                  } else if(currentIndent == indents.back()) {
                                    BEGIN(INITIAL);
                                  } else if(currentIndent < indents.back()) {
                                    indents.pop_back();
                                    if(currentIndent > indents.back()) {
                                      yyerror(std::string(std::to_string(yylineno) + ": Unexpected indent: expected '" + std::to_string(indents.back()) + "' spaces").c_str());
                                    }
                                    return '}';
                                  }
                                }

        /* Everything else (unexpected) */

.                      		 	{ yyerror("Unknown character");}

%%
