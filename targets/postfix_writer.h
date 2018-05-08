#ifndef __GR8_SEMANTICS_POSTFIX_WRITER_H__
#define __GR8_SEMANTICS_POSTFIX_WRITER_H__

#include <string>
#include <iostream>
#include <cdk/symbol_table.h>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"

namespace gr8 {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<gr8::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<gr8::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0) {
    }

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

      inline bool bothDoubleImplicitly(basic_type *t1, basic_type *t2) {
          return ( (isDouble(t1) && isDouble(t2)) ||
                   (isDouble(t1) && isInt(t2)   ) ||
                   (isInt(t1)    && isDouble(t2)) );
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
