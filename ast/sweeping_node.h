#ifndef __GR8_SWEEPINGNODE_H__
#define __GR8_SWEEPINGNODE_H__

#include <cdk/ast/expression_node.h>

namespace gr8 {

  /**
   * Class for describing sweeping-cycle nodes.
   */
  class sweeping_node: public cdk::basic_node {
    cdk::lvalue_node *_sweep;
    cdk::expression_node *_from, *_to, *_by;

    cdk::basic_node *_doBlock;

  public:
    inline sweeping_node(int lineno, cdk::lvalue_node *sweep, cdk::expression_node *from,
        cdk::expression_node *to, cdk::expression_node *by, cdk::basic_node *doBlock) :
        basic_node(lineno), _sweep(sweep), _from(from), _to(to), _by(by), _doBlock(doBlock){ 
    }

  public:
    inline cdk::lvalue_node *sweep() {
        return _sweep;
    }

    inline cdk::expression_node *from() {
        return _from;
    }

    inline cdk::expression_node *to() {
        return _to;
    }

    inline cdk::expression_node *by() {
        return _by;
    }
    inline cdk::basic_node *doBlock() {
        return _doBlock;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_sweeping_node(this, level);
    }

  };

} // gr8

#endif
