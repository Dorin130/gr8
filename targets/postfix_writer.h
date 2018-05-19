#ifndef __GR8_SEMANTICS_POSTFIX_WRITER_H__
#define __GR8_SEMANTICS_POSTFIX_WRITER_H__

#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <cdk/symbol_table.h>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"

namespace gr8 {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!

  enum Segment { NONE, BSS, DATA, RODATA, TEXT }; 

  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<gr8::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;
    std::vector<int> stack_again_lbls = std::vector<int>();
    std::vector<int> stack_stop_lbls = std::vector<int>();
    bool last_instr_return = false;
    bool _in_arguments = false;
    int _current_offset = 0;
    std::unordered_set<std::string> _extern_funcs = std::unordered_set<std::string>();
    std::vector<cdk::assignment_node *> _inits;
    Segment _segment = NONE;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<gr8::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0) {
    }

  protected:
    void processBinaryExpressionImplicitConversion(cdk::binary_expression_node * const node, int lvl);

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

  public:
      inline bool isInt(basic_type *type)       { return type->name() == basic_type::TYPE_INT;      }
      inline bool isDouble(basic_type *type)    { return type->name() == basic_type::TYPE_DOUBLE;   }
      inline bool isString(basic_type *type)    { return type->name() == basic_type::TYPE_STRING;   }
      inline bool isPointer(basic_type *type)   { return type->name() == basic_type::TYPE_POINTER;  }
      inline bool isVoid(basic_type *type)      { return type->name() == basic_type::TYPE_VOID;     }

      inline bool bothDoubleImplicitly(basic_type *t1, basic_type *t2) {
          return ( (isDouble(t1) && isDouble(t2)) ||
                   (isDouble(t1) && isInt(t2)   ) ||
                   (isInt(t1)    && isDouble(t2)) );
      }


      inline void listExtern(){
        for (std::unordered_set<std::string>::iterator func=_extern_funcs.begin(); func!=_extern_funcs.end(); func++){
          _pf.EXTERN(*func);
        }
      }

      void switchSegment(Segment segment) {
        if (_segment == segment) {
          return ;
        } else if (segment == BSS){
          _pf.BSS();
          _segment = BSS;
        } else if (segment == DATA){
          _pf.DATA();
          _segment = DATA;
        } else if (segment == RODATA){
          _pf.RODATA();
          _segment = RODATA;
        } else if (segment == TEXT){
          _pf.TEXT();
          _segment = TEXT;
        }
      }


  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // gr8

#endif
