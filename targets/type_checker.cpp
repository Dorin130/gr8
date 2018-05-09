#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

#define ASSERT_UNSPEC \
    { if (node->type() != nullptr && \
          node->type()->name() != basic_type::TYPE_UNSPEC) return; }

#define MAKE_TYPE(TYPE) node->type(TYPE)
#define NEW_TYPE_INT new basic_type(4, basic_type::TYPE_INT)
#define NEW_TYPE_STRING new basic_type(4, basic_type::TYPE_STRING)
#define NEW_TYPE_DOUBLE new basic_type(8, basic_type::TYPE_DOUBLE)
#define NEW_TYPE_POINTER new basic_type(4, basic_type::TYPE_POINTER)



//---------------------------------------------------------------------------

void gr8::type_checker::do_sequence_node(cdk::sequence_node * const node, int lvl) { //Might be unnecessary (not an expression)
  for (auto i : node->nodes())
      i->accept(this, lvl);
}

//---------------------------------------------------------------------------




//---------------------------------------------------------------------------

void gr8::type_checker::do_integer_node(cdk::integer_node * const node, int lvl) { //COMPLETE
  ASSERT_UNSPEC;
  MAKE_TYPE(NEW_TYPE_INT);
}

void gr8::type_checker::do_string_node(cdk::string_node * const node, int lvl) { //COMPLETE
  ASSERT_UNSPEC;
  MAKE_TYPE(NEW_TYPE_STRING);
}

void gr8::type_checker::do_double_node(cdk::double_node * const node, int lvl) { //COMPLETE
  ASSERT_UNSPEC;
  MAKE_TYPE(NEW_TYPE_DOUBLE);
}

//---------------------------------------------------------------------------

void gr8::type_checker::processUnaryExpression(cdk::unary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;

  node->argument()->accept(this, lvl + 2);
  basic_type *t = node->argument()->type();

  if (isInt(t))
    MAKE_TYPE(NEW_TYPE_INT);
  else if (isDouble(t))
    MAKE_TYPE(NEW_TYPE_DOUBLE);
  else throw std::string(
    "wrong type in argument of unary expression: expected 'small' or 'huge' but was '" + typeToString(t) + "'");
}

void gr8::type_checker::do_neg_node(cdk::neg_node * const node, int lvl) { //COMPLETE
  processUnaryExpression(node, lvl);
}
void gr8::type_checker::do_identity_node(gr8::identity_node *const node, int lvl) { //COMPLETE
  processUnaryExpression(node, lvl);
}
void gr8::type_checker::do_not_node(cdk::not_node * const node, int lvl) { //COMPLETE
  ASSERT_UNSPEC;

  node->argument()->accept(this, lvl + 2);
  basic_type *t_arg = node->argument()->type();

  if (isNumber(t_arg) || isString(t_arg) || isPointer(t_arg))
    MAKE_TYPE(NEW_TYPE_INT);
  else throw std::string(
    "wrong type in argument of not expression: expected 'small', 'huge', 'fake' or 'news' but was '" + typeToString(t_arg) + "'"); //unreachable in theory
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
    MAKE_TYPE(NEW_TYPE_DOUBLE);
  } else if (isInt(t1) && isInt(t2)) {
    MAKE_TYPE(type_deep_copy(t2));
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
    MAKE_TYPE(type_deep_copy(t1));
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
    MAKE_TYPE(NEW_TYPE_DOUBLE);
  } else if (isInt(t1) && isInt(t2)) {
    MAKE_TYPE(type_deep_copy(t1));
  } else if (isPointer(t1)) {
      if(isInt(t2))
        MAKE_TYPE(type_deep_copy(t1));
      else throw std::string(
        "wrong type in right argument of additive expression: expected '" + typeToString(t1) + "' but was '" + typeToString(t2)) + "'";
  } else if (isPointer(t2)) {
      if(isInt(t1))
        MAKE_TYPE(type_deep_copy(t2));
      else throw std::string(
        "wrong type in left argument of additive expression: expected '" + typeToString(t2) + "' but was '" + typeToString(t1)) + "'";
  }
}

void gr8::type_checker::do_sub_node(cdk::sub_node * const node, int lvl) { //COMPLETE
  processBinaryExpression(node, lvl);
  basic_type *t1 = node->left()->type();
  basic_type *t2 = node->right()->type();

  if(bothDoubleImplicitly(t1, t2)) {
    MAKE_TYPE(NEW_TYPE_DOUBLE);;
  } else if (isInt(t1) && isInt(t2)) {
      MAKE_TYPE(type_deep_copy(t1));
  } else if (isPointer(t1) && isPointer(t2) && t1->subtype()->name() == t2->subtype()->name()) { //assuming a type with name TYPE_POINTER always has a non-null subtype
      if(t1->subtype()->name() == t2->subtype()->name())
        MAKE_TYPE(NEW_TYPE_INT); //difference of pointers is a number (integer)
      else throw std::string(
        "subtractive expression: cannot subtract fakes: '" + typeToString(t1) + "' and '" + typeToString(t1)) + "'";
  } else if (isPointer(t1)) throw std::string(
    "wrong type in right argument of subtractive expression: expected '" + typeToString(t1) + "' but was '" + typeToString(t2) + "'");
  else throw std::string(
    "wrong type in left argument of subtractive expression: expected '" + typeToString(t2) + "' but was '" + typeToString(t1) + "'");
}

void gr8::type_checker::do_eq_node(cdk::eq_node * const node, int lvl) { //COMPLETE
  processBinaryExpression(node, lvl);
  basic_type *t1 = node->left()->type();
  basic_type *t2 = node->right()->type();

  if(!sameType(t1, t2) && !bothDoubleImplicitly(t1, t2)) throw std::string( //must be of same type (1st case) OR implicitly convertible (2nd case)
    "wrong type in right argument of equality expression: expected '" + typeToString(t1) + "' but was '" + typeToString(t2) + "'");

  MAKE_TYPE(NEW_TYPE_INT);; //INT ~ BOOLEAN
}

void gr8::type_checker::processBinaryLogicExpression(cdk::binary_expression_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  basic_type *t1 = node->left()->type();
  basic_type *t2 = node->right()->type();

  if(bothDoubleImplicitly(t1, t2) || (isInt(t1) && isInt(t2))) {
      MAKE_TYPE(NEW_TYPE_INT);;
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



void gr8::type_checker::do_rvalue_node(cdk::rvalue_node * const node, int lvl) { //COMPLETE (?)
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "undeclared identifier '" + id + "'";
  }
}

void gr8::type_checker::do_assignment_node(cdk::assignment_node * const node, int lvl) { //COMPLETE (?)
  ASSERT_UNSPEC;

  try {
    node->lvalue()->accept(this, lvl + 2);
  } catch (const std::string &id) {
    throw "undeclared identifier '" + id + "'";
  }
  node->rvalue()->accept(this, lvl + 2);

  basic_type *lv = node->lvalue()->type();
  basic_type *rv = node->rvalue()->type();

  type_unspec_converter(lv, rv);

  if( !sameType(lv, rv) && !( isDouble(lv) && isInt(rv) ) ) throw std::string( //must be of same type (1st case) OR implicitly convertible (2nd case)
    "wrong type in assignment instruction: cannot assign value of type '" + typeToString(rv) + "' to variable of type '" + typeToString(lv) + "'");

  // in gr8, assignments are untyped (are instructions)
}

//---------------------------------------------------------------------------

void gr8::type_checker::do_evaluation_node(gr8::evaluation_node * const node, int lvl) { //COMPLETE
  node->argument()->accept(this, lvl + 2);

  type_unspec_converter(node->argument()->type());
}

void gr8::type_checker::do_print_node(gr8::print_node * const node, int lvl) { //COMPLETE
  node->argument()->accept(this, lvl + 2);

  type_unspec_converter(node->argument()->type());
}

//---------------------------------------------------------------------------

void gr8::type_checker::do_if_node(gr8::if_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void gr8::type_checker::do_if_else_node(gr8::if_else_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void gr8::type_checker::do_sweeping_node(gr8::sweeping_node *const node, int lvl) {
  node->sweep()->accept(this, lvl + 4);
  /*basic_type *t_sweep = node->sweep()->type();
  if(!isNumber(t_sweep)) throw std::string(
    "wrong type of lvalue argument of sweeping expression: expected lvalue of type 'small' or 'huge' but was '" + typeToString(t_sweep) + "'"); //ask if double can be in sweeping*/
  node->from()->accept(this, lvl + 4);
  node->to()->accept(this, lvl + 4);
  node->by()->accept(this, lvl + 4);
  //need to verify if types make sense ?
}


//---------------------------------------------------------------------------
// TODO
//---------------------------------------------------------------------------

void gr8::type_checker::do_stop_node(gr8::stop_node *const node, int lvl) { //COMPLETE
}
void gr8::type_checker::do_again_node(gr8::again_node *const node, int lvl) { //COMPLETE
}
void gr8::type_checker::do_return_node(gr8::return_node *const node, int lvl) { //INCOMPLETE
}
void gr8::type_checker::do_block_node(gr8::block_node *const node, int lvl) { //COMPLETE
}


//---------------------------------------------------------------------------
// The lvalue bunch
//---------------------------------------------------------------------------

void gr8::type_checker::do_read_node(gr8::read_node * const node, int lvl) { //COMPLETE
  ASSERT_UNSPEC

  MAKE_TYPE(new basic_type()); //type UNSPECIFIED
}

void gr8::type_checker::do_null_node(gr8::null_node *const node, int lvl) { //COMPLETE
  ASSERT_UNSPEC

  MAKE_TYPE(NEW_TYPE_POINTER);
  node->type()->_subtype = new basic_type(); //type UNSPECIFIED
}

void gr8::type_checker::do_alloc_node(gr8::alloc_node *const node, int lvl) { //COMPLETE (?)
  ASSERT_UNSPEC

  node->argument()->accept(this, lvl + 2);
  basic_type *t = node->argument()->type();

  if (isInt(t)) {
    MAKE_TYPE(NEW_TYPE_POINTER);
    node->type()->_subtype = new basic_type(); //type UNSPECIFIED
  } else throw std::string(
    "wrong type in argument of 'objects' expression: expected 'small' but was '" + typeToString(t) + "'");
}

void gr8::type_checker::do_cell_node(gr8::cell_node *const node, int lvl) { //COME BACK TO THIS LATER
  ASSERT_UNSPEC

  node->cell()->accept(this, lvl + 2);
  node->baseptr()->accept(this, lvl + 2);

  basic_type *t_base = node->baseptr()->type();
  basic_type *t_cell = node->cell()->type();

  type_unspec_converter(t_base); //COME BACK TO THIS LATER
  type_unspec_converter(t_cell); //cast to int if UNSPECIFIED (e.g. cell input at p)

  if(!isPointer(t_base)) throw std::string(
    "wrong type in 'base' argument of cell expression: expected 'fake' but was '" + typeToString(t_base) + "'");
  if(!isInt(t_cell)) throw std::string(
    "wrong type in 'cell' argument of cell expression: expected 'small' but was '" + typeToString(t_base) + "'"); //ask if double can be converted

  node->type(type_deep_copy(t_base->subtype())); //type is UNSPECIFIED if t_base is null_node. Otherwise should be ok.
}

void gr8::type_checker::do_address_of_node(gr8::address_of_node *const node, int lvl) {  //CHECK FOR CORRECTNESS
  ASSERT_UNSPEC

  node->lvalue()->accept(this, lvl + 2);
  basic_type *t = node->lvalue()->type();
  type_unspec_converter(t); //e.g. 'cell 1 at null ?' -> 'cell 1 at null' is of type UNSPECIFIED

  MAKE_TYPE(NEW_TYPE_POINTER);
  node->type()->_subtype = type_deep_copy(t);
}

//---------------------------------------------------------------------------
// The identifier bunch
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

void gr8::type_checker::do_var_declaration_node(gr8::var_declaration_node *const node, int lvl) { //COMPLETE (?)
  basic_type *declared_type = node->type();
  std::string id = node->name();

  std::shared_ptr<gr8::symbol> symbol = std::make_shared<gr8::symbol>(declared_type, id, node->noQualifier(), node->isPublic(), node->isUse());
  if (!_symtab.insert(id, symbol))
    throw id + " redeclared";

  if (node->init()) {
    node->init()->accept(this, lvl + 2);
    type_unspec_converter(declared_type, node->init()->type());
    if (!sameType(declared_type, node->init()->type())) throw std::string( //Error example: small i (initially 3 objects)
      "wrong type for initially expression: expected '" + typeToString(declared_type) + "' but was '" + typeToString(node->init()->type()) + "'");
  }

  _parent->set_new_symbol(symbol);
}

void gr8::type_checker::do_function_declaration_node(gr8::function_declaration_node *const node, int lvl) { //COMPLETE (?)
  basic_type *declared_type = node->type();

  std::string id = node->name();
  if(id == "covfefe")
    id = "_main";
  else if(node->name() == "_main")
    id = "ex_main";

  std::vector<basic_type*> param_types;
  for(cdk::basic_node *arg : node->args()->nodes()) {
    param_types.push_back(type_deep_copy(dynamic_cast<var_declaration_node*>(arg)->type()));
  }

  std::shared_ptr<gr8::symbol> symbol = std::make_shared<gr8::symbol>(declared_type, id,
    node->noQualifier(), node->isPublic(), node->isUse(), true, param_types, false);
  if (!_symtab.insert(id, symbol))
    throw id + " redeclared";

  _parent->set_new_symbol(symbol);
}

void gr8::type_checker::do_function_definition_node(gr8::function_definition_node *const node, int lvl) { //COMPLETE (?)
  basic_type *defined_type = node->type();

  std::string id = node->name();
  if(id == "covfefe")
    id = "_main";
  else if(node->name() == "_main")
    id = "ex_main";

  std::shared_ptr<gr8::symbol> symbol = _symtab.find(id);
  if(symbol) {

    if(symbol->isDefined()) throw std::string(
      "attempt to redefine function/procedure '" + node->name() + "'");

    else if(!sameType(defined_type, symbol->type())) throw std::string(
      "type mismatch between declaration and definition of '" + id + "' in return type: expected '" +
          typeToString(symbol->type()) + "' but was '" + typeToString(defined_type) + "'");

    else {//check if types are consistent with previous declaration
      int arg_no = 1;
      std::vector<cdk::basic_node*>::iterator it_def = node->args()->nodes().begin();
      std::vector<basic_type*>::iterator it_sym = symbol->param_types().begin();
      for(; it_def != node->args()->nodes().end() && it_sym != symbol->param_types().end(); it_def++, it_sym++, arg_no++) {

        basic_type* t_arg = dynamic_cast<var_declaration_node*>(*it_def)->type();
        if(!sameType(t_arg, *it_sym)) throw std::string(                                                 //type mismatch
          "type mismatch between declaration and definition of '" + id + "' in argument " + std::to_string(arg_no) + ": expected '" +
          typeToString(*it_sym) + "' but was '" + typeToString(t_arg) + "'");
      }

      if(it_def == node->args()->nodes().end() && it_sym != symbol->param_types().end()) throw std::string( //too many params in definition
          "missing arguments in definition of '" + id + "'. Previous declaration expects " + std::to_string(symbol->param_types().size()) + " arguments.");

      if(it_def != node->args()->nodes().end() && it_sym == symbol->param_types().end()) throw std::string( //too little params in definition
          "extra arguments in definition of '" + id + "'. Previous declaration expects " + std::to_string(symbol->param_types().size()) + " arguments.");

      symbol->setDefined(true);
    }
  } else { //same as declaration node
    std::vector<basic_type*> param_types;
    for(cdk::basic_node *arg : node->args()->nodes())
      param_types.push_back(type_deep_copy(dynamic_cast<var_declaration_node*>(arg)->type()));

    std::shared_ptr<gr8::symbol> symbol = std::make_shared<gr8::symbol>(defined_type, id, node->noQualifier(), node->isPublic(), node->isUse(), true, param_types, true);
    _symtab.insert(id, symbol);

    _parent->set_new_symbol(symbol);
  }
}

void gr8::type_checker::do_call_node(gr8::call_node *const node, int lvl) { //COMPLETE (?)
  ASSERT_UNSPEC

  std::string id = node->name();
  if(id == "covfefe")
    id = "_main";
  else if(node->name() == "_main")
    id = "ex_main";

  std::shared_ptr<gr8::symbol> symbol = _symtab.find(id);
  if(symbol) {

    if(!symbol->isFunction()) throw std::string(
      "attempt to call '" + node->name() + "' when it is not a function or procedure");

    else if(!symbol->isDefined()) throw std::string( //RECHECK this for imported (use) functions
      "attempt to call undefined function/procedure '" + node->name() + "'");

    else {//check if types are consistent with previous declaration
      int arg_no = 1;
      std::reverse_iterator<std::vector<cdk::basic_node*>::iterator> it_call = node->args()->nodes().rbegin();
      std::vector<basic_type*>::iterator it_sym = symbol->param_types().begin();
      for(; it_call != node->args()->nodes().rend() && it_sym != symbol->param_types().end(); it_call++, it_sym++, arg_no++) {

        (*it_call)->accept(this, lvl); //might be expression
        basic_type* t_arg = dynamic_cast<var_declaration_node*>(*it_call)->type();
        type_unspec_converter(t_arg, *it_sym);

        if(!sameType(t_arg, *it_sym)) throw std::string(                                                 //type mismatch
          "type mismatch between declaration and definition of '" + id + "' in argument " + std::to_string(arg_no) + ": expected '" +
          typeToString(*it_sym) + "' but was '" + typeToString(t_arg) + "'");
      }

      if(it_call == node->args()->nodes().rend() && it_sym != symbol->param_types().end()) throw std::string( //too many params in call
          "missing arguments in call for '" + id + "'. Previous declaration expects " + std::to_string(symbol->param_types().size()) + " arguments.");

      if(it_call != node->args()->nodes().rend() && it_sym == symbol->param_types().end()) throw std::string( //too little params in call
          "extra arguments in call for '" + id + "'. Previous declaration expects " + std::to_string(symbol->param_types().size()) + " arguments.");
    }
  } else throw std::string(
      "attempt to call undeclared function/procedure '" + node->name() + "'");
}
