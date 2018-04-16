#ifndef __GR8_NULLNODE_H__
#define __GR8_NULLNODE_H__

#include <cdk/ast/lvalue_node.h>

namespace gr8 {

  /**
   * Class for describing a null node
   */
  class null_node: public cdk::expression_node {

  public:
    inline null_node(int lineno) :
        cdk::expression_node(lineno) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_null_node(this, level);
    }

  };

} // gr8

#endif
