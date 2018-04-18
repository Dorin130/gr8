%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE               compiler->scanner()->lineno()
#define yylex()            compiler->scanner()->scan()
#define yyerror(s)         compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!
%}

%union {
  int                   i;	/* integer value */
  double                d;
  std::string           *s;	/* symbol name or string literal */
  cdk::basic_node       *node;	/* node pointer */
  cdk::sequence_node    *sequence;
  cdk::expression_node  *expression; /* expression nodes */
  cdk::lvalue_node      *lvalue;
  basic_type            *type;
  gr8::block_node       *block;
};

%token <i> tINTEGER
%token <d> tREAL
%token <s> tSTRING tIDENTIFIER
%token tSMALL tHUGE tNEWS tFAKE tINITIALLY tUSE tPUBLIC tDEFINE tPROCEDURE tFUNCTION tON tAS tDO tUSES tFOR tRETURN tPLUS tMINUS tTIMES tOVER tMODULUS tNOT tAND tOR tASSIGN tTO tCELL tAT tABOVE tBELOW tEQUALS tINPUT tOBJECTS tIF tTHEN tELSIF tELSE tSTOP tAGAIN tPOST tTWEET tSWEEPING tFROM tBY tNULL 

%nonassoc xLVAL
%nonassoc '?'

%nonassoc tOBJECTS

%left tAT
%left tOR
%left tAND
%nonassoc tNOT
%left tEQUALS
%left tBELOW tABOVE
%left tPLUS tMINUS
%left tTIMES tMODULUS tOVER

%nonassoc tUNARY

%type <node> declaration variable parameter function localvariable instr ifconditions
%type <block> block
%type <sequence> exprs declseq funcparamdecl funcparamdef parameters localvars instrs
%type <expression> expr literal functioncall
%type <s> string
%type <lvalue> lval
%type <type> type fakehuge fakesmall fakenews faketype


%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file
    : declseq                                                       { compiler->ast($1);}
    |                                                               { compiler->ast(new cdk::sequence_node(LINE));}
    ;

declseq
    : declaration                                                   { $$ = new cdk::sequence_node(LINE, $1);}
    | declseq declaration                                           { $$ = new cdk::sequence_node(LINE, $2, $1);}
    ;
    
declaration
    : variable ';'                                                  { $$ = $1;}
    | function                                                      { $$ = $1;}
    ;
    
function
    :                     type tFUNCTION  tIDENTIFIER funcparamdecl             { $$ = new gr8::function_declaration_node(LINE, gr8::function_declaration_node::QUALIFIER_NONE  , $1, $3, $4); }
    |           tPUBLIC   type tFUNCTION  tIDENTIFIER funcparamdecl             { $$ = new gr8::function_declaration_node(LINE, gr8::function_declaration_node::QUALIFIER_PUBLIC, $2, $4, $5); }
    |           tUSE      type tFUNCTION  tIDENTIFIER funcparamdecl             { $$ = new gr8::function_declaration_node(LINE, gr8::function_declaration_node::QUALIFIER_USE   , $2, $4, $5); }
    |                          tPROCEDURE tIDENTIFIER funcparamdecl             { $$ = new gr8::function_declaration_node(LINE, gr8::function_declaration_node::QUALIFIER_NONE  , new basic_type(0, basic_type::TYPE_VOID), $2, $3); }
    |           tPUBLIC        tPROCEDURE tIDENTIFIER funcparamdecl             { $$ = new gr8::function_declaration_node(LINE, gr8::function_declaration_node::QUALIFIER_PUBLIC, new basic_type(0, basic_type::TYPE_VOID), $3, $4); }
    |           tUSE           tPROCEDURE tIDENTIFIER funcparamdecl             { $$ = new gr8::function_declaration_node(LINE, gr8::function_declaration_node::QUALIFIER_USE   , new basic_type(0, basic_type::TYPE_VOID), $3, $4); }
    | tDEFINE             type tFUNCTION  tIDENTIFIER funcparamdef tAS block    { $$ = new gr8::function_definition_node(LINE, gr8::function_definition_node::QUALIFIER_NONE  , $2, $4, $5, $7); }
    | tDEFINE   tPUBLIC   type tFUNCTION  tIDENTIFIER funcparamdef tAS block    { $$ = new gr8::function_definition_node(LINE, gr8::function_definition_node::QUALIFIER_PUBLIC, $3, $5, $6, $8); }
    | tDEFINE   tUSE      type tFUNCTION  tIDENTIFIER funcparamdef tAS block    { $$ = new gr8::function_definition_node(LINE, gr8::function_definition_node::QUALIFIER_USE   , $3, $5, $6, $8); }
    | tDEFINE                  tPROCEDURE tIDENTIFIER funcparamdef tAS block    { $$ = new gr8::function_definition_node(LINE, gr8::function_definition_node::QUALIFIER_NONE  , new basic_type(0, basic_type::TYPE_VOID), $3, $4, $6 ); }
    | tDEFINE   tPUBLIC        tPROCEDURE tIDENTIFIER funcparamdef tAS block    { $$ = new gr8::function_definition_node(LINE, gr8::function_definition_node::QUALIFIER_PUBLIC, new basic_type(0, basic_type::TYPE_VOID), $4, $5, $7 ); }
    | tDEFINE   tUSE           tPROCEDURE tIDENTIFIER funcparamdef tAS block    { $$ = new gr8::function_definition_node(LINE, gr8::function_definition_node::QUALIFIER_USE   , new basic_type(0, basic_type::TYPE_VOID), $4, $5, $7 ); }
    ;
    
funcparamdecl
    : tUSES parameters ";"                                          { $$ = $2; }
    | ";"                                                           { $$ = new cdk::sequence_node(LINE); }
    ;
    
funcparamdef
    : tON parameters                                                { $$ = $2; }
    |                                                               { $$ = new cdk::sequence_node(LINE); }
    ;

parameters
    : parameter                                                     { $$ = new cdk::sequence_node(LINE, $1); }
    | parameters ',' parameter                                      { $$ = new cdk::sequence_node(LINE, $3 , $1); }
    ;
    
parameter
    : type tIDENTIFIER                                              { $$ = new gr8::var_declaration_node(LINE, gr8::var_declaration_node::QUALIFIER_NONE, $1, $2, nullptr);   }
    ;
    
variable
    : type tIDENTIFIER                                              { $$ = new gr8::var_declaration_node(LINE, gr8::var_declaration_node::QUALIFIER_NONE, $1, $2, nullptr);   }
    | type tIDENTIFIER '(' tINITIALLY  expr ')'                     { $$ = new gr8::var_declaration_node(LINE, gr8::var_declaration_node::QUALIFIER_NONE, $1, $2, $5);        }
    | tPUBLIC type tIDENTIFIER                                      { $$ = new gr8::var_declaration_node(LINE, gr8::var_declaration_node::QUALIFIER_PUBLIC, $2, $3, nullptr); }
    | tPUBLIC type tIDENTIFIER '(' tINITIALLY  expr ')'             { $$ = new gr8::var_declaration_node(LINE, gr8::var_declaration_node::QUALIFIER_PUBLIC, $2, $3, $6);      }
    | tUSE type tIDENTIFIER                                         { $$ = new gr8::var_declaration_node(LINE, gr8::var_declaration_node::QUALIFIER_USE, $2, $3, nullptr);    }
    ;
    
type
    : tSMALL                                                        { $$ = new basic_type(4, basic_type::TYPE_INT);    }
    | tHUGE                                                         { $$ = new basic_type(8, basic_type::TYPE_DOUBLE); }
    | tNEWS                                                         { $$ = new basic_type(4, basic_type::TYPE_STRING); }
    | faketype                                                      { $$ = $1;                                         }
    ;
    
faketype
    : fakesmall tFAKE                                               { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $1; }
    | fakehuge  tFAKE                                               { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $1; }
    | fakenews  tNEWS                                               { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $1; }
    ;

fakesmall
    : fakesmall tSMALL                                              { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $1;}
    | tSMALL                                                        { $$ = new basic_type(4, basic_type::TYPE_INT); }
    ;

fakehuge
    : fakehuge tHUGE                                                { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $1;}
    | tHUGE                                                         { $$ = new basic_type(8, basic_type::TYPE_DOUBLE); }
    ;

fakenews
    : fakenews tFAKE                                                { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $1;}
    | tFAKE                                                         { $$ = new basic_type(4, basic_type::TYPE_STRING); }
    ;

block 
    : '{' '}'                                                       { $$ = new gr8::block_node(LINE, new cdk::sequence_node(LINE), new cdk::sequence_node(LINE));} /* NIL node? */
    | '{' localvars '}'                                             { $$ = new gr8::block_node(LINE, $2, new cdk::sequence_node(LINE));}
    | '{' instrs '}'                                                { $$ = new gr8::block_node(LINE, new cdk::sequence_node(LINE), $2);}
    | '{' localvars instrs '}'                                      { $$ = new gr8::block_node(LINE, $2, $3);}
    ;
    
localvars
    : localvars localvariable ';'                                   { $$ = new cdk::sequence_node(LINE, $2, $1); }
    | localvariable ';'                                             { $$ = new cdk::sequence_node(LINE, $1); }
    ;

localvariable
    : type tIDENTIFIER                                              { $$ = new gr8::var_declaration_node(LINE, gr8::var_declaration_node::QUALIFIER_NONE, $1, $2, nullptr);   }
    | type tIDENTIFIER '(' tINITIALLY  expr ')'                     { $$ = new gr8::var_declaration_node(LINE, gr8::var_declaration_node::QUALIFIER_NONE, $1, $2, $5);        }
    ;
    
instrs
    : instrs instr                                                  { $$ = new cdk::sequence_node(LINE, $2, $1);}
    | instr                                                         { $$ = new cdk::sequence_node(LINE, $1);    }
    ;

instr
    : expr ';'                                                      { $$ = new gr8::evaluation_node(LINE, $1);   }
    | tPOST expr ';'                                                { $$ = new gr8::print_node(LINE, $2, true);  }
    | tTWEET expr ';'                                               { $$ = new gr8::print_node(LINE, $2, false); }
    | tSTOP tINTEGER ';'                                            { $$ = new gr8::stop_node(LINE, $2);         }
    | tSTOP ';'                                                     { $$ = new gr8::stop_node(LINE, 1);          }
    | tAGAIN tINTEGER ';'                                           { $$ = new gr8::again_node(LINE, $2);        }
    | tAGAIN ';'                                                    { $$ = new gr8::again_node(LINE, 1);         }
    | tRETURN expr ';'                                              { $$ = new gr8::return_node(LINE, $2);       }
    | tRETURN ';'                                                   { $$ = new gr8::return_node(LINE, nullptr);  }
    | tIF ifconditions                                              { $$ = $2;   }
    | tSWEEPING lval tFROM expr tTO expr tDO block                  { $$ = new gr8::sweeping_node(LINE, $2, $4, $6, new cdk::integer_node(LINE, 1), $8); }
    | tSWEEPING lval tFROM expr tTO expr tBY expr tDO block         { $$ = new gr8::sweeping_node(LINE, $2, $4, $6, $8, $10); }
    ;

ifconditions
    : expr tTHEN block                                              { $$ = new gr8::if_node(LINE, $1, $3);          }
    | expr tTHEN block tELSE block                                  { $$ = new gr8::if_else_node(LINE, $1, $3, $5); }
    | expr tTHEN block tELSIF ifconditions                          { $$ = new gr8::if_else_node(LINE, $1, $3, $5); }
    ;

lval
    : tCELL expr tAT expr                                           { $$ = new gr8::cell_node(LINE, $4, $2);                 }
    | tIDENTIFIER                                                   { $$ = new cdk::identifier_node(LINE, $1); delete $1;    }
    ;

functioncall
    : tUSE exprs tFOR tIDENTIFIER                                   { $$ = new gr8::call_node(LINE, $4, $2);                           }
    | tDO tIDENTIFIER                                               { $$ = new gr8::call_node(LINE, $2, new cdk::sequence_node(LINE)); }
    ;
    
exprs
    : exprs ',' expr                                                { $$ = new cdk::sequence_node(LINE, $3, $1);}
    | expr                                                          { $$ = new cdk::sequence_node(LINE, $1);}
    ;
    
literal
    : tINTEGER                                                      { $$ = new cdk::integer_node(LINE, $1); }
    | tREAL                                                         { $$ = new cdk::double_node(LINE, $1);  }
    | string                                                        { $$ = new cdk::string_node(LINE, new std::string($1->c_str())); delete $1; }
    | tNULL                                                         { $$ = new gr8::null_node(LINE);        }
    ;

string
    : string tSTRING                                                { $$ = new std::string(*$1 + *$2); delete $1; delete $2;}
    | tSTRING                                                       { $$ = $1;}
    ;     

expr   
    : tINPUT                                                        { $$ = new gr8::read_node(LINE);               }
    | tASSIGN expr tTO lval                                         { $$ = new cdk::assignment_node(LINE, $4, $2); }
    | literal                                                       { $$ = $1;                                     }
    | functioncall                                                  { $$ = $1;                                     }
    | tMINUS expr %prec tUNARY                                      { $$ = new cdk::neg_node(LINE, $2);            }
    | tPLUS expr %prec tUNARY                                       { $$ = new gr8::identity_node(LINE, $2);       }
    | tNOT expr                                                     { $$ = new cdk::not_node(LINE, $2);            }
    | expr tPLUS expr                                               { $$ = new cdk::add_node(LINE, $1, $3);        }
    | expr tMINUS expr                                              { $$ = new cdk::sub_node(LINE, $1, $3);        }
    | expr tTIMES expr                                              { $$ = new cdk::mul_node(LINE, $1, $3);        }
    | expr tOVER expr                                               { $$ = new cdk::div_node(LINE, $1, $3);        }
    | expr tMODULUS expr                                            { $$ = new cdk::mod_node(LINE, $1, $3);        }
    | expr tBELOW expr                                              { $$ = new cdk::lt_node(LINE, $1, $3);         }
    | expr tABOVE expr                                              { $$ = new cdk::gt_node(LINE, $1, $3);         }
    | expr tEQUALS expr                                             { $$ = new cdk::eq_node(LINE, $1, $3);         }
    | expr tOR expr                                                 { $$ = new cdk::or_node(LINE, $1, $3);         }
    | expr tAND expr                                                { $$ = new cdk::and_node(LINE, $1, $3);        }
    | lval '?'                                                      { $$ = new gr8::address_of_node(LINE, $1);     }
    | lval %prec xLVAL                                              { $$ = new cdk::rvalue_node(LINE, $1);         }
    | expr tOBJECTS                                                 { $$ = new gr8::alloc_node(LINE, $1);          }
    | '(' expr ')'                                                  { $$ = $2;                                     }
    ;

%%

/*

fakesmall
    : tSMALL fakesmall                                              { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $2;                                      }
    | tSMALL tFAKE                                                  { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = new basic_type(4, basic_type::TYPE_INT); }
    ;
    
fakehuge
    : tHUGE fakehuge                                                { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $2;                                         }
    | tHUGE tFAKE                                                   { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = new basic_type(8, basic_type::TYPE_DOUBLE); }
    ;

fakenews
    : tFAKE fakenews                                                { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $2;                                         }
    | tFAKE tNEWS                                                   { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = new basic_type(4, basic_type::TYPE_STRING); }
    ;
*/