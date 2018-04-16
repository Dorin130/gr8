#ifndef __GR8_CELLNODE_H__
#define __GR8_CELLNODE_H__

#include <cdk/ast/lvalue_node.h>

namespace gr8 {

  /**
   * Class for describing cell (indexing) node.
   */
  class cell_node: public cdk::lvalue_node {
      cdk::expression_node *_baseptr, *_cell;

  public:
    inline cell_node(int lineno, cdk::expression_node *baseptr, cdk::expression_node *cell) :
        cdk::lvalue_node(lineno), _baseptr(baseptr), _cell(cell) {
    }

  public:
    inline cdk::expression_node *baseptr() {
      return _baseptr;
    }

    inline cdk::expression_node *cell() {
      return _cell;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_cell_node(this, level);
    }

  };

} // cdk

#endif
