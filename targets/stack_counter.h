#ifndef __GR8_STACK_COUNTER_H__
#define __GR8_STACK_COUNTER_H__

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
  class stack_counter: public basic_ast_visitor {
  private:
    int _size;

  public:
    stack_counter(std::shared_ptr<cdk::compiler> compiler):
     basic_ast_visitor(compiler), _size(0) {}

  public:
    ~stack_counter() {
      os().flush();
    }

  int size() { return _size; }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // gr8

//---------------------------------------------------------------------------
//     DO THIS TO COUNT FUNCTION ENTER SIZE (preferably after asserts)
//---------------------------------------------------------------------------

/*

gr8::stack_counter counter(compiler); \
(node)->accept(&counter, 0); 
counter.size();

*/

#endif

