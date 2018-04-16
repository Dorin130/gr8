#ifndef __GR8_ALLOCNODE_H__
#define __GR8_ALLOCNODE_H__

#include <cdk/ast/unary_expression_node.h>

namespace gr8 {

  /**
   * Class for describing the memory allocation node
   */
  class alloc_node: public cdk::unary_expression_node {
  public:
    alloc_node(int lineno, cdk::expression_node *arg) :
        cdk::unary_expression_node(lineno, arg) {
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_alloc_node(this, level);
    }

  };

} // gr8

#endif
