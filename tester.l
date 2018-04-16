%option stack noyywrap yylineno 8bit
%{
// make relevant includes before including the parser's tab file

#include <stack>
#include <iostream>

bool firstTime = true;
bool closeBracket = false;
int currentIndent = 0;
std::stack<int> indents;

int dealWithIndent(int indentLvl) {
    if(indents.size()==0) {                     //1st level indentation
        indents.push(indentLvl);
        return 's';
    } 
    if(indentLvl == indents.top()) {     //same indentation
        return ';';
    } else if(indentLvl > indents.top()) {      //more indentation
        indents.push(indentLvl);
        return '{';
    } else {                                    //dedent
        indents.pop();
        if(indentLvl > indents.top()) {
            return -1;
        }
        return '}';
    }
}
inline void yyerror (const char* s) { std::cerr << s << std::endl;}

%}
%x X_STRING X_COMMENT X_ESCAPE X_IGNORE X_NEWLINE X_CHECK_INDENT CLOSEBRACKET

IDENTIFIER                      [a-zA-Z][0-9a-zA-Z-]*
SYMBOL                          [()?,]
SPACE                           [ \t]+

DIGI                            [0-9]
EXPONENT                        [eE][-+]?{DIGI}+
REAL                            {DIGI}+{EXPONENT}|{DIGI}+"."{DIGI}*{EXPONENT}?|{DIGI}*"."{DIGI}+{EXPONENT}?
HEPT                            [0-6]
INT10                           [1-9]{DIGI}*|0
INT7                            0{HEPT}+

%%
                                {if(firstTime) { firstTime = false; BEGIN(X_CHECK_INDENT) ; } }

"small"                         std::cout << "tSMALL";
"huge"                          std::cout << "tHUGE";
"news"                          std::cout << "tNEWS";
"fake"                          std::cout << "tFAKE";
"initially"                     std::cout << "tINITIALLY";

"use"                           std::cout << "tUSE";
"public"                        std::cout << "tPUBLIC";
"define"                        std::cout << "tDEFINE";
"procedure"                     std::cout << "tPROCEDURE";
"function"                      std::cout << "tFUNCTION";
"on"                            std::cout << "tON";
"as"                            std::cout << "tAS";
"do"                            std::cout << "tDO";
"uses"                          std::cout << "tUSES";
"for"                           std::cout << "tFOR";
"return"                        std::cout << "tRETURN";

"plus"                          std::cout << "tPLUS";
"minus"                         std::cout << "tMINUS";
"times"                         std::cout << "tTIMES";
"over"                          std::cout << "tOVER";
"modulus"                       std::cout << "tMODULUS";
"not"                           std::cout << "tNOT";
"and"                           std::cout << "tAND";
"or"                            std::cout << "tOR";
"assign"                        std::cout << "tASSIGN";
"to"                            std::cout << "tTO";
"cell"                          std::cout << "tCELL";
"at"                            std::cout << "tAT";
"above"                         std::cout << "tABOVE";
"below"                         std::cout << "tBELOW";
"equals"                        std::cout << "tEQUALS";
"input"                         std::cout << "tINPUT";
"objects"                       std::cout << "tOBJECTS";

"if"                            std::cout << "tIF";
"then"                          std::cout << "tTHEN";
"elsif"                         std::cout << "tELSIF";
"else"                          std::cout << "tELSE";
"stop"                          std::cout << "tSTOP";
"again"                         std::cout << "tAGAIN";
"post"                          std::cout << "tPOST";
"tweet"                         std::cout << "tTWEET";
"sweeping"                      std::cout << "tSWEEPING";
"from"                          std::cout << "tFROM";
"by"                            std::cout << "tBY";
"null"                          std::cout << "tNULL";

"<<"                            yy_push_state(X_COMMENT);
<X_COMMENT>"<<"                 yy_push_state(X_COMMENT);
<X_COMMENT>">>"                 yy_pop_state();
<X_COMMENT>">>"{SPACE}*\n       yy_pop_state();                 /* easier indentation */
<X_COMMENT>.|\n                 ; /* ignore comments */

"!!".*$                         ; /* ignore comments */

{IDENTIFIER}                    {  std::cout << "tIDENTIFIER"; }
{SYMBOL}                        { std::cout << *yytext; }
{REAL}                          { if(errno == ERANGE) yyerror("ERANGE (Overflow)"); std::cout << "tREAL"; }
{INT7}                          { if(errno == ERANGE) yyerror("ERANGE (Overflow)"); std::cout << "tINTEGER"; }
{INT10}                         {  if(errno == ERANGE) yyerror("ERANGE (Overflow)"); std::cout << "tINTEGER";}


\"                              { yy_push_state(X_STRING); }
<X_STRING,X_IGNORE>\"           { yy_pop_state (); std::cout << "tSTRING";}
<X_STRING>~                     { yy_push_state(X_ESCAPE);}
<X_STRING>\n|\0                 { yyerror("Invalid character in string (NUL, LF)"); } /* ask teacher if necessary; separate error messages if so */
<X_STRING>.                     { ; }

<X_ESCAPE>0                     { yy_pop_state(); yy_pop_state(); yy_push_state(X_IGNORE);}
<X_ESCAPE>00                    { yy_pop_state(); yy_pop_state(); yy_push_state(X_IGNORE);}
<X_ESCAPE>000                   { yy_pop_state(); yy_pop_state(); yy_push_state(X_IGNORE);}

<X_ESCAPE>n                     { yy_pop_state();}
<X_ESCAPE>r                     { yy_pop_state();}
<X_ESCAPE>t                     {  yy_pop_state();}
<X_ESCAPE>\"                    {  yy_pop_state();}
<X_ESCAPE>~                     {  yy_pop_state();}

<X_ESCAPE>{HEPT}{HEPT}?{HEPT}?  { if(strtol(yytext, nullptr, 7) >= 256) yyerror("Char not 8-bit!");  yy_pop_state();}
<X_ESCAPE>.                     yyerror("Unknown character in string!");

<X_IGNORE>~\"                   { ; /* ignore */}
<X_IGNORE>~~                    { ; /* ignore */}
<X_IGNORE>.|\n                  { ; /* ignore */}


"..."\n                         { ; /* line continuation marker */}
{SPACE}*                        { ; /* ignore whitespace that is not in the start of the line*/}
\n                              { BEGIN(X_CHECK_INDENT); currentIndent=0; }              //end of logic line
<INITIAL><<EOF>>                         { yyless(0); BEGIN(X_CHECK_INDENT); }   //end of logic line

        /* INDENTATION TREATMENT SECTION */
<X_CHECK_INDENT>"!!".*$         { ; /* ignore */}
<X_CHECK_INDENT><<              { yy_push_state(X_COMMENT); }
<X_CHECK_INDENT>[ ]             { currentIndent++; }
<X_CHECK_INDENT>[\t]            { currentIndent += 8; currentIndent -= currentIndent%8; }
<X_CHECK_INDENT>\n              { currentIndent=0; }

<X_CHECK_INDENT><<EOF>>         { currentIndent=0;
                                  switch(dealWithIndent(currentIndent)) {
                                    case '}':
                                        std::cout << ';';
                                        BEGIN(CLOSEBRACKET);
                                        break;
                                    default:
                                        return 0;
                                        break;
                                  }
                                }
                                
<X_CHECK_INDENT>.               { yyless(0);   //logic line start
                                  int c = dealWithIndent(currentIndent);
                                  switch(c) {
                                    case '{':
                                        BEGIN(INITIAL);
                                        std::cout << '{';
                                        break;
                                    case '}':
                                        std::cout << ';';
                                        BEGIN(CLOSEBRACKET);
                                        break;
                                    case ';':
                                        std::cout << ';';
                                        BEGIN(INITIAL);
                                        break;
                                    case 's':
                                        BEGIN(INITIAL);
                                        break;
                                    case -1:
                                        std::string error = "Unexpected indent: expected '" + std::to_string(indents.top()) + "' spaces";
                                        yyerror(error.c_str());
                                        //BEGIN(INITIAL);
                                  }
                                }
        /* END OF INDENTATION TREATMENT SECTION */


<CLOSEBRACKET>.|\n              {yyless(0); std::cout << '}';BEGIN(X_CHECK_INDENT);}
<CLOSEBRACKET><<EOF>>           {yyless(0); std::cout << '}';BEGIN(X_CHECK_INDENT);}

.                      		 	{ yyerror("Unknown character");}



%%

int main() {
    return yylex();
}