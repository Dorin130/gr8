#ifndef __GR8_BLOCKNODE_H__
#define __GR8_BLOCKNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/sequence_node.h>

namespace gr8 {

  /**
   * Class for describing block nodes
   */
  class block_node: public cdk::basic_node {
    cdk::sequence_node *_decl, *_instr;

  public:
    inline block_node(int lineno, cdk::sequence_node *decl, cdk::sequence_node *instr) :
        basic_node(lineno), _decl(decl), _instr(instr) {
    }

  public:
    inline cdk::sequence_node *declarations() {
      return _decl;
    }

    inline cdk::sequence_node *instructions() {
      return _instr;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_block_node(this, level);
    }

  };

} // gr8

#endif
