#ifndef __GR8_STOPNODE_H__
#define __GR8_STOPNODE_H__

#include <cdk/ast/basic_node.h>

namespace gr8 {

  /**
   * Class for describing stop instruction
   */
  class stop_node: public cdk::basic_node {
    int _ncycles;

  public:
    inline stop_node(int lineno, int ncycles) :
        basic_node(lineno), _ncycles(ncycles) {
    }

  public:
    inline int ncycles() {
      return _ncycles;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_stop_node(this, level);
    }

  };

} // gr8

#endif
