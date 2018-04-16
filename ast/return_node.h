#ifndef __GR8_RETURNNODE_H__
#define __GR8_RETURNNODE_H__

#include <cdk/ast/basic_node.h>

namespace gr8 {

  /**
   * Class for describing return instruction
   */
  class return_node: public cdk::basic_node {
    cdk::expression_node *_ret;

  public:
    inline return_node(int lineno, cdk::expression_node *ret) :
        basic_node(lineno), _ret(ret) {
    }

  public:
    inline cdk::expression_node *ret() {
      return _ret;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_return_node(this, level);
    }

  };

} // gr8

#endif
