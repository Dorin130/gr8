#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

#define ASSERT_UNSPEC \
    { if (node->type() != nullptr && \
          node->type()->name() != basic_type::TYPE_UNSPEC) return; }

//---------------------------------------------------------------------------

void gr8::type_checker::do_sequence_node(cdk::sequence_node * const node, int lvl) { //Might be unnecessary (not an expression)
  for (auto i : node->nodes())
      i->accept(this);
}

//---------------------------------------------------------------------------




//---------------------------------------------------------------------------

void gr8::type_checker::do_integer_node(cdk::integer_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void gr8::type_checker::do_string_node(cdk::string_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_STRING));
}

void gr8::type_checker::do_double_node(cdk::double_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
}

//---------------------------------------------------------------------------

void gr8::type_checker::processUnaryExpression(cdk::unary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;

  node->argument()->accept(this, lvl + 2);
  basic_type *t = node->argument()->type();
  if (isInt(t))
    node->type(new basic_type(4, basic_type::TYPE_INT));
  else if (isDouble(t))
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  else throw std::string(
    "wrong type in argument of unary expression: expected 'small' or 'huge' but was '" + typeToString(t) + "'");
}

void gr8::type_checker::do_neg_node(cdk::neg_node * const node, int lvl) {
  processUnaryExpression(node, lvl);
}
void gr8::type_checker::do_identity_node(gr8::identity_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}
void gr8::type_checker::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_UNSPEC;

  node->argument()->accept(this, lvl + 2);
  basic_type *t = node->argument()->type();
  if (isNumber(t) || isString(t) || isPointer(t))
    node->type(new basic_type(4, basic_type::TYPE_INT));
  else throw std::string(
    "wrong type in argument of not expression: expected 'small', 'huge', 'fake' or 'news' but was '" + typeToString(t) + "'"); //unreachable in theory
}

//---------------------------------------------------------------------------

void gr8::type_checker::processBinaryExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  basic_type *t1 = node->left()->type();
  basic_type *t2 = node->right()->type();

  type_unspec_converter(t1, t2);

  if(!isNumber(t1) && !isPointer(t1)) throw std::string(
    "wrong type in left argument of binary expression: expected 'small' or 'huge' or 'fake' but was '" + typeToString(t1) + "'");
  if(!isNumber(t2) && !isPointer(t2)) throw std::string(
    "wrong type in right argument of binary expression: expected 'small' or 'huge' or 'fake' but was '" + typeToString(t2) + "'");

}

void gr8::type_checker::processMultiplicativeExpression(cdk::binary_expression_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  basic_type *t1 = node->left()->type();
  basic_type *t2 = node->right()->type();

  if(bothDoubleImplicitly(t1, t2)) {
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  } else if (isInt(t1) && isInt(t2)) {
    node->type(type_deep_copy(t1));
  } else if (!isNumber(t1)) throw std::string(
    "wrong type in left argument of multiplicative expression: expected 'small' or 'huge' but was '" + typeToString(t1) + "'");
  else throw std::string(
    "wrong type in right argument of multiplicative expression: expected 'small' or 'huge' but was '" + typeToString(t2) + "'");
}

void gr8::type_checker::do_mul_node(cdk::mul_node * const node, int lvl) {  //COMPLETE
  processMultiplicativeExpression(node, lvl);
}
void gr8::type_checker::do_div_node(cdk::div_node * const node, int lvl) {  //COMPLETE
  processMultiplicativeExpression(node, lvl);
}
void gr8::type_checker::do_mod_node(cdk::mod_node * const node, int lvl) {  //COMPLETE
  processBinaryExpression(node, lvl);

  basic_type *t1 = node->left()->type();
  basic_type *t2 = node->right()->type();

  if (isInt(t1) && isInt(t2)) {
    node->type(type_deep_copy(t1));
  } else if (!isInt(t1)) throw std::string(
    "wrong type in left argument of modulus expression: expected 'small' but was '" + typeToString(t1) + "'");
  else throw std::string(
    "wrong type in right argument of modulus expression: expected 'small' but was '" + typeToString(t2) + "'");
}

void gr8::type_checker::do_add_node(cdk::add_node * const node, int lvl) { //COMPLETE
  processBinaryExpression(node, lvl);
  basic_type *t1 = node->left()->type();
  basic_type *t2 = node->right()->type();

  if(bothDoubleImplicitly(t1, t2)) {
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  } else if (isInt(t1) && isInt(t2)) {
    node->type(type_deep_copy(t1));
  } else if (isPointer(t1)) {
      if(isInt(t2))
        node->type(type_deep_copy(t1));
      else throw std::string(
        "wrong type in right argument of additive expression: expected '" + typeToString(t1) + "' but was '" + typeToString(t2)) + "'";
  } else if (isPointer(t2)) {
      if(isInt(t1))
        node->type(type_deep_copy(t2));
      else throw std::string(
        "wrong type in left argument of additive expression: expected '" + typeToString(t2) + "' but was '" + typeToString(t1)) + "'";
  }
}

void gr8::type_checker::do_sub_node(cdk::sub_node * const node, int lvl) { //COMPLETE
  processBinaryExpression(node, lvl);
  basic_type *t1 = node->left()->type();
  basic_type *t2 = node->right()->type();

  if(bothDoubleImplicitly(t1, t2)) {
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  } else if (isInt(t1) && isInt(t2)) {
      node->type(type_deep_copy(t1));
  } else if (isPointer(t1) && isPointer(t2) && t1->subtype()->name() == t2->subtype()->name()) { //assuming a type with name TYPE_POINTER always has a non-null subtype
      if(t1->subtype()->name() == t2->subtype()->name())
        node->type(new basic_type(4, basic_type::TYPE_INT)); //difference of pointers is a number (integer)
      else throw std::string(
        "subtractive expression: cannot subtract fakes: '" + typeToString(t1) + "' and '" + typeToString(t1)) + "'";
  } else if (isPointer(t1)) throw std::string(
    "wrong type in right argument of subtractive expression: expected '" + typeToString(t1) + "' but was '" + typeToString(t2) + "'");
  else throw std::string(
    "wrong type in left argument of subtractive expression: expected '" + typeToString(t2) + "' but was '" + typeToString(t1) + "'");
}

void gr8::type_checker::do_eq_node(cdk::eq_node * const node, int lvl) { //COMPLETE
  processBinaryExpression(node, lvl);

  if(node->left()->type()->name() != node->left()->type()->name()) throw std::string( //check if both types have to be equal
    "wrong type in right argument of equality expression: expected '" + typeToString(t1) + "' but was '" + typeToString(t2) + "'");

  node->type(new basic_type(4, basic_type::TYPE_INT)); //INT ~ BOOLEAN
}

void gr8::type_checker::processBinaryLogicExpression(cdk::binary_expression_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  basic_type *t1 = node->left()->type();
  basic_type *t2 = node->right()->type();

  if(bothDoubleImplicitly(t1, t2) || (isInt(t1) && isInt(t2))) {
      node->type(new basic_type(4, basic_type::TYPE_INT));
  } else if (!isNumber(t1)) throw std::string(
      "wrong type in left argument of binary expression: expected 'small' or 'huge' but was '" + typeToString(t1) + "'");
  else throw std::string(
      "wrong type in right argument of binary logic expression: expected 'small' or 'huge' but was '" + typeToString(t2) + "'");
}

void gr8::type_checker::do_gt_node(cdk::gt_node * const node, int lvl) { //COMPLETE
  processBinaryLogicExpression(node, lvl);
}
void gr8::type_checker::do_lt_node(cdk::lt_node * const node, int lvl) { //COMPLETE
  processBinaryLogicExpression(node, lvl);
}
void gr8::type_checker::do_and_node(cdk::and_node * const node, int lvl) { //COMPLETE
  processBinaryLogicExpression(node, lvl);
}
void gr8::type_checker::do_or_node(cdk::or_node * const node, int lvl) { //COMPLETE
  processBinaryLogicExpression(node, lvl);
}

//---------------------------------------------------------------------------

void gr8::type_checker::do_identifier_node(cdk::identifier_node * const node, int lvl) { //COMPLETE (?)
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<gr8::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  } else {
    throw id;
  }
}

void gr8::type_checker::do_rvalue_node(cdk::rvalue_node * const node, int lvl) { //COMPLETE (?)
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void gr8::type_checker::do_assignment_node(cdk::assignment_node * const node, int lvl) { //COMPLETE (?)
  ASSERT_UNSPEC;

  node->lvalue()->accept(this, lvl + 2);
  node->rvalue()->accept(this, lvl + 2);

  basic_type *lv = node->lvalue()->type();
  basic_type *rv = node->rvalue()->type();

  type_unspec_converter(t1, t2);

  if( !sameType(lv, rv) && !( isDouble(lv) && isInt(rv) ) ) //must be of same type (1st case) OR implicitly convertible (2nd case)
    "wrong type in assignment instruction: cannot assign value of type '" + typeToString(rv) + "' to variable of type '" + typeToString(lv) + "'");

  // in gr8, assignments are untyped (are instructions)
}

//---------------------------------------------------------------------------
/*
void gr8::type_checker::do_program_node(gr8::program_node * const node, int lvl) {
  // EMPTY
}
*/
void gr8::type_checker::do_evaluation_node(gr8::evaluation_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void gr8::type_checker::do_print_node(gr8::print_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void gr8::type_checker::do_read_node(gr8::read_node * const node, int lvl) {
  ASSERT_UNSPEC //TODO: check if this is necessary
  node->type(new basic_type());  //TODO: check this
}

//---------------------------------------------------------------------------

void gr8::type_checker::do_if_node(gr8::if_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void gr8::type_checker::do_if_else_node(gr8::if_else_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}


//---------------------------------------------------------------------------
// TODO
//---------------------------------------------------------------------------

void gr8::type_checker::do_stop_node(gr8::stop_node *const node, int lvl) { //COMPLETE
}
void gr8::type_checker::do_again_node(gr8::again_node *const node, int lvl) { //COMPLETE
}
void gr8::type_checker::do_return_node(gr8::return_node *const node, int lvl) { //COMPLETE
}
void gr8::type_checker::do_block_node(gr8::block_node *const node, int lvl) { //COMPLETE
}

void gr8::type_checker::do_cell_node(gr8::cell_node *const node, int lvl) {
  // EMPTY
}
void gr8::type_checker::do_sweeping_node(gr8::sweeping_node *const node, int lvl) {
  // EMPTY
}
void gr8::type_checker::do_var_declaration_node(gr8::var_declaration_node *const node, int lvl) {
  // EMPTY
}
void gr8::type_checker::do_call_node(gr8::call_node *const node, int lvl) {
  // EMPTY
}
void gr8::type_checker::do_address_of_node(gr8::address_of_node *const node, int lvl) {
  // EMPTY
}
void gr8::type_checker::do_null_node(gr8::null_node *const node, int lvl) {
  // EMPTY
}
void gr8::type_checker::do_function_declaration_node(gr8::function_declaration_node *const node, int lvl) {
  // EMPTY
}
void gr8::type_checker::do_function_definition_node(gr8::function_definition_node *const node, int lvl) {
  // EMPTY
}
void gr8::type_checker::do_alloc_node(gr8::alloc_node *const node, int lvl) {
  // EMPTY
}
