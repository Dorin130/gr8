#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "ast/all.h"  // all.h is automatically generated


//---------------------------------------------------------------------------

void gr8::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {  //COMPLETE (?)
  _pf.INT(node->value()); // push an integer
}

void gr8::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) { //COMPLETE (?)
  _pf.DOUBLE(node->value());
}

void gr8::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters

  /* leave the address on the stack */
  _pf.TEXT(); // return to the TEXT segment
  _pf.ADDR(mklbl(lbl1)); // the string to be printed
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if(isDouble(node->argument()->type()))
    _pf.DNEG();
  else
    _pf.NEG(); // 2-complement
}

void gr8::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  if(isDouble(node->argument()->type())) {
    _pf.DOUBLE(0);
    _pf.DCMP();
  } else {
    _pf.INT(0);
    _pf.EQ();
  }
  _pf.NOT();
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  if(isPointer(node->type())) {
    if(isInt(node->left()->type())) {
        node->left()->accept(this, lvl);
        _pf.INT(node->type()->subtype()->size());
        _pf.MUL();
        node->right()->accept(this, lvl);
        _pf.ADD();
    } else {
        node->left()->accept(this, lvl);
        node->right()->accept(this, lvl);
        _pf.INT(node->type()->subtype()->size());
        _pf.MUL();
        _pf.ADD();
    }
  } else { //normal add
    processBinaryExpressionImplicitConversion(node, lvl);
    if(bothDoubleImplicitly(node->left()->type(), node->left()->type()))
      _pf.DADD();
    else
      _pf.ADD();
  }
}

void gr8::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  processBinaryExpressionImplicitConversion(node, lvl);
  if(bothDoubleImplicitly(node->left()->type(), node->right()->type()))
    _pf.DSUB();
  else if(isPointer(node->left()->type())) {
    _pf.SUB();
    _pf.INT(node->left()->type()->subtype()->size());
    _pf.DIV();
  } else
    _pf.SUB();
}

void gr8::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}

void gr8::postfix_writer::processBinaryExpressionImplicitConversion(cdk::binary_expression_node * const node, int lvl) { //handles implicit conversion
  node->left()->accept(this, lvl);
  if(isInt(node->left()->type()) && isDouble(node->type()))
    _pf.I2D();
  node->right()->accept(this, lvl);
  if(isInt(node->right()->type()) && isDouble(node->type()))
    _pf.I2D();
}

void gr8::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  processBinaryExpressionImplicitConversion(node, lvl);
  if(bothDoubleImplicitly(node->left()->type(), node->right()->type()))
    _pf.DMUL();
  else
    _pf.MUL();
}
void gr8::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  processBinaryExpressionImplicitConversion(node, lvl);
  if(bothDoubleImplicitly(node->left()->type(), node->right()->type()))
    _pf.DDIV();
  else
    _pf.DIV();
}

void gr8::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  processBinaryExpressionImplicitConversion(node, lvl);
  if(bothDoubleImplicitly(node->left()->type(), node->right()->type())) {
    _pf.DCMP();
    _pf.INT(0);
  }
  _pf.LT();
}
void gr8::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  processBinaryExpressionImplicitConversion(node, lvl);
  if(bothDoubleImplicitly(node->left()->type(), node->right()->type())) {
    _pf.DCMP();
    _pf.INT(0);
  }
  _pf.GT();
}

void gr8::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  processBinaryExpressionImplicitConversion(node, lvl);
  if(bothDoubleImplicitly(node->left()->type(), node->right()->type())) {
    _pf.DCMP();
    _pf.INT(0);
  }
  _pf.EQ();
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_identifier_node(cdk::identifier_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  // simplified generation: all variables are global
  _pf.ADDR(node->name());
}

void gr8::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) { //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  if(isDouble(node->lvalue()->type()))
    _pf.LDDOUBLE();
  else
    _pf.LDINT();
}

void gr8::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) { //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl); // determine the new value
  node->lvalue()->accept(this, lvl); // where to store the value
  if(isDouble(node->lvalue()->type())) {
    if(isInt(node->rvalue()->type()))
      _pf.I2D();    //implicit conversion
    _pf.STDOUBLE(); // store the value at address
  } else
    _pf.STINT();    // store the value at address
}

//---------------------------------------------------------------------------
/*
void gr8::postfix_writer::do_program_node(gr8::program_node * const node, int lvl) {
  // Note that Simple didn't have functions. Thus, it didn't need
  // a function node. However, it had to start in the main function.
  // program_node (representing the whole program) doubled as a
  // main function node.

  // generate the main function (RTS mandates that its name be "_main")
  _pf.TEXT();
  _pf.ALIGN();
  _pf.GLOBAL("_main", _pf.FUNC());
  _pf.LABEL("_main");
  _pf.ENTER(0);  // Simple didn't implement local variables

  node->statements()->accept(this, lvl);

  // end the main function
  _pf.INT(0);
  _pf.STFVAL32();
  _pf.LEAVE();
  _pf.RET();

  // these are just a few library function imports
  _pf.EXTERN("readi");
  _pf.EXTERN("printi");
  _pf.EXTERN("prints");
  _pf.EXTERN("println");
}
*/
//---------------------------------------------------------------------------

void gr8::postfix_writer::do_evaluation_node(gr8::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->type()->name() == basic_type::TYPE_INT) {
    _pf.TRASH(4); // delete the evaluated value
  } else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void gr8::postfix_writer::do_print_node(gr8::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value to print
  if (node->argument()->type()->name() == basic_type::TYPE_INT) {
    _pf.CALL("printi");
    _pf.TRASH(4); // delete the printed value
  } else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
    _pf.CALL("prints");
    _pf.TRASH(4); // delete the printed value's address
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
  _pf.CALL("println"); // print a newline
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_read_node(gr8::read_node * const node, int lvl) { //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  if(isDouble(node->type()))
    _pf.CALL("readd");
  else
    _pf.CALL("readi");
  _pf.LDFVAL32();
}

//---------------------------------------------------------------------------
/*
void gr8::postfix_writer::do_while_node(gr8::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  _pf.LABEL(mklbl(lbl1 = ++_lbl));
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl2 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl1));
  _pf.LABEL(mklbl(lbl2));
}
*/
//---------------------------------------------------------------------------

void gr8::postfix_writer::do_if_node(gr8::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_if_else_node(gr8::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------
// TODO
//---------------------------------------------------------------------------

void gr8::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_stop_node(gr8::stop_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_again_node(gr8::again_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_cell_node(gr8::cell_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_sweeping_node(gr8::sweeping_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_var_declaration_node(gr8::var_declaration_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_return_node(gr8::return_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // EMPTY
}
void gr8::postfix_writer::do_call_node(gr8::call_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_identity_node(gr8::identity_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_block_node(gr8::block_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_address_of_node(gr8::address_of_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_null_node(gr8::null_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_function_declaration_node(gr8::function_declaration_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_function_definition_node(gr8::function_definition_node *const node, int lvl) {
  // EMPTY
}
void gr8::postfix_writer::do_alloc_node(gr8::alloc_node *const node, int lvl) {
  // EMPTY
}
