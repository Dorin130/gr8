// $Id: type_checker.h,v 1.1 2018/02/23 17:43:01 ist13500 Exp $ -*- c++ -*-
#ifndef __GR8_SEMANTICS_TYPE_CHECKER_H__
#define __GR8_SEMANTICS_TYPE_CHECKER_H__

#include <string>
#include <iostream>
#include <cdk/symbol_table.h>
#include <cdk/ast/basic_node.h>
#include "targets/symbol.h"
#include "targets/basic_ast_visitor.h"

namespace gr8 {

  /**
   * Print nodes as XML elements to the output stream.
   */
  class type_checker: public basic_ast_visitor {
    cdk::symbol_table<gr8::symbol> &_symtab;

    basic_ast_visitor *_parent;

  public:
    type_checker(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<gr8::symbol> &symtab, basic_ast_visitor *parent) :
        basic_ast_visitor(compiler), _symtab(symtab), _parent(parent) {
    }

  public:
    ~type_checker() {
      os().flush();
    }

  protected:
    void processUnaryExpression(cdk::unary_expression_node * const node, int lvl);
    void processBinaryExpression(cdk::binary_expression_node * const node, int lvl);
    void processAdditiveEpression(cdk::binary_expression_node * const node, int lvl);
    void processMultiplicativeExpression(cdk::binary_expression_node * const node, int lvl);
    void processBinaryLogicExpression(cdk::binary_expression_node * const node, int lvl);
    template<typename T>
    void process_literal(cdk::literal_node<T> * const node, int lvl) {
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end
  public:
      std::string typeToString(basic_type *t) {
          switch (t->name()) {
          case basic_type::TYPE_UNSPEC:
            return "unspec";
          case basic_type::TYPE_INT:
            return "small";
          case basic_type::TYPE_DOUBLE:
            return "huge";
          case basic_type::TYPE_STRING:
            return "news";
          case basic_type::TYPE_POINTER:
          {
            std::string sub = typeToString(t->subtype());
            if(sub.find("news", sub.size()-4) != std::string::npos)
                return "fake " + sub;
            else
                return sub + " fake";
          }
          case basic_type::TYPE_VOID:
            return "void";
          default:
            return "undefined type";
          }
      }

  public:
      inline bool isUnspec(basic_type *type)    { return type->name() == basic_type::TYPE_UNSPEC;   }
      inline bool isInt(basic_type *type)       { return type->name() == basic_type::TYPE_INT;      }
      inline bool isDouble(basic_type *type)    { return type->name() == basic_type::TYPE_DOUBLE;   }
      inline bool isString(basic_type *type)    { return type->name() == basic_type::TYPE_STRING;   }
      inline bool isPointer(basic_type *type)   { return type->name() == basic_type::TYPE_POINTER;  }
      inline bool isVoid(basic_type *type)      { return type->name() == basic_type::TYPE_VOID;     }

      inline bool isNumber(basic_type *type)    { return isInt(type) || isDouble(type); }

  public:
      inline bool isPointerDisplacement(basic_type *t1, basic_type *t2) {
          return ( (isInt(t1) && isPointer(t2)) ||
                   (isPointer(t1) && isInt(t2)) );
      }
      inline bool isPointerDifference(basic_type *t1, basic_type *t2) {
          return ( isPointer(t1) && isPointer(t2) );
      }
      inline bool bothDoubleImplicitly(basic_type *t1, basic_type *t2) {
          return ( (isDouble(t1) && isDouble(t2)) ||
                   (isDouble(t1) && isInt(t2)   ) ||
                   (isInt(t1)    && isDouble(t2)) );
      }

      bool sameType(basic_type *t1, basic_type *t2) {
          if(t1->type()->name() == t2->t1->type()->name()) {
              return (!isPointer(t1)) true : sameType(t1->subtype(), t2->subtype());
          }
      }

  public:
      basic_type *type_deep_copy(basic_type *oldtype) {
          basic_type *newtype = new basic_type(oldtype->size(), oldtype->name());
          if(oldtype->subtype() != nullptr) {
              newtype->_subtype = type_deep_copy(oldtype->subtype());
          }
          return newtype;
      }

      void type_unspec_converter(basic_type *t1, basic_type *t2) {
          if(isUnspec(t1) && isUnspec(t2)) {
              t1->_name = t2->_name = basic_type::TYPE_INT;
              t1->_size = t2->_size = 4;
              return;
          }

          if(isUnspec(t1)) {
              t1->_name = t2->name();
              t1->_size = t2->size();
              t1->_subtype = type_deep_copy(t2->subtype());
              return; //check if this should be removed
          }

          if(isUnspec(t2)) {
              t2->_name = t1->name();
              t2->_size = t1->size();
              t2->_subtype = type_deep_copy(t1->subtype());
              return; //check if this should be removed
          }

          //pointers may have UNSPEC in subtype (e.g. variable declaration objects expression)
          if(isPointer(t1) && isPointer(t2)) {
              type_ranking_unspec(t1->subtype(), t2->subtype());
              return;
          }

          return;
      }

  };

} // gr8

//---------------------------------------------------------------------------
//     HELPER MACRO FOR TYPE CHECKING
//---------------------------------------------------------------------------

#define CHECK_TYPES(compiler, symtab, node) { \
  try { \
    gr8::type_checker checker(compiler, symtab, this); \
    (node)->accept(&checker, 0); \
  } \
  catch (const std::string &problem) { \
    std::cerr << (node)->lineno() << ": " << problem << std::endl; \
    return; \
  } \
}

#define ASSERT_SAFE_EXPRESSIONS CHECK_TYPES(_compiler, _symtab, node)

#endif
