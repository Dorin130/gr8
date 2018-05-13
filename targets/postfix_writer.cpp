#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/stack_counter.h"
#include "ast/all.h"  // all.h is automatically generated


//---------------------------------------------------------------------------

void gr8::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    if(last_instr_return) throw std::string(
    "warning: unreachable code after return instruction");
    node->node(i)->accept(this, lvl);
  }
  last_instr_return = false;
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

void gr8::postfix_writer::do_null_node(gr8::null_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS
  _pf.INT(0);
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

void gr8::postfix_writer::do_identity_node(gr8::identity_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
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

void gr8::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  int end_lbl;

  node->left()->accept(this, lvl);
  _pf.DUP32();

  _pf.JZ(mklbl(end_lbl = ++_lbl));
  _pf.TRASH(4);

  node->right()->accept(this, lvl);

  _pf.ALIGN();
  _pf.LABEL(mklbl(end_lbl));
}

void gr8::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  int end_lbl;

  node->left()->accept(this, lvl);
  _pf.DUP32();

  _pf.JNZ(mklbl(end_lbl = ++_lbl));
  _pf.TRASH(4);

  node->right()->accept(this, lvl);

  _pf.ALIGN();
  _pf.LABEL(mklbl(end_lbl));
}

//---------------------------------------------------------------------------

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

void gr8::postfix_writer::do_evaluation_node(gr8::evaluation_node * const node, int lvl) {  //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (isInt(node->argument()->type())) {
    _pf.TRASH(4); // delete the evaluated value
  } else if (isString(node->argument()->type())) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else if (isDouble(node->argument()->type())) {
    _pf.TRASH(8);
  } else if (isPointer(node->argument()->type())) {
    _pf.TRASH(4);
  } else if (isVoid(node->argument()->type())) {
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_print_node(gr8::print_node * const node, int lvl) { //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value to print
  if (isInt(node->argument()->type()) || isPointer(node->argument()->type())) {
    _pf.CALL("printi");
    _pf.TRASH(4); // delete the printed value
  } else if (isDouble(node->argument()->type())) {
    _pf.CALL("printd");
    _pf.TRASH(8); // delete the printed value's address
  }else if (isString(node->argument()->type())) {
    _pf.CALL("prints");
    _pf.TRASH(4); // delete the printed value's address
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
  _pf.CALL("println"); // print a newline
}


void gr8::postfix_writer::do_read_node(gr8::read_node * const node, int lvl) { //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  if(isDouble(node->type()))
    _pf.CALL("readd");
  else
    _pf.CALL("readi");
  _pf.LDFVAL32();
}


//---------------------------------------------------------------------------

void gr8::postfix_writer::do_if_node(gr8::if_node * const node, int lvl) { //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}


void gr8::postfix_writer::do_if_else_node(gr8::if_else_node * const node, int lvl) { //COMPLETE (?)
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

void gr8::postfix_writer::do_stop_node(gr8::stop_node *const node, int lvl) {
  size_t skips = node->ncycles();
  if(skips > stack_stop_lbls.size()) throw std::string(
    "Invalid argument for stop instruction: was " + std::to_string(skips) + " but can only break "
    + std::to_string(stack_stop_lbls.size()) + " cycles.");
  _pf.JMP(mklbl(stack_stop_lbls.at(skips-1)));
}

void gr8::postfix_writer::do_again_node(gr8::again_node *const node, int lvl) {
  size_t skips = node->ncycles();
  if(skips > stack_again_lbls.size()) throw std::string(
    "Invalid argument for again instruction: was " + std::to_string(skips) + " but can only break "
    + std::to_string(stack_again_lbls.size()) + " cycles.");
  _pf.JMP(mklbl(stack_again_lbls.at(skips-1)));
}

void gr8::postfix_writer::do_sweeping_node(gr8::sweeping_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int cond_lbl_str = ++_lbl;
  int end_lbl_str = ++_lbl;
  int incr_lbl_str = ++_lbl;
  stack_again_lbls.push_back(incr_lbl_str);
  stack_stop_lbls.push_back(end_lbl_str);

  node->sweep()->accept(this, lvl);
  _pf.DUP32(); //address is 4 bytes
  if(isDouble(node->sweep()->type()) && isInt(node->from()->type())) { //assuming 'from' is int -- ask teacher
    node->from()->accept(this, lvl);
    _pf.I2D();
    _pf.STDOUBLE();
  } else {
    node->from()->accept(this, lvl);
    _pf.STINT();
  }
  _pf.LABEL(mklbl(cond_lbl_str)); //condition
  _pf.DUP32(); //address is 4 bytes
  if(isDouble(node->sweep()->type()) && isInt(node->from()->type())) {
    _pf.LDDOUBLE();
    node->to()->accept(this, lvl);
    _pf.I2D();
    _pf.DCMP();
    _pf.INT(0);
    _pf.GT();
  } else {
    _pf.LDINT();
    node->from()->accept(this, lvl);
    _pf.GT();
  }
  _pf.JNZ(mklbl(end_lbl_str));

  node->doBlock()->accept(this, lvl);

  _pf.LABEL(mklbl(incr_lbl_str));
  _pf.DUP32();
  _pf.DUP32();
  if(isDouble(node->sweep()->type()) && isInt(node->from()->type())) {
    _pf.LDDOUBLE();
    node->by()->accept(this, lvl);
    _pf.I2D();
    _pf.DADD();
    _pf.STDOUBLE();
  } else {
    _pf.LDINT();
    node->from()->accept(this, lvl);
    _pf.ADD();
    _pf.STINT();
  }

  _pf.JMP(mklbl(cond_lbl_str));
  _pf.LABEL(mklbl(end_lbl_str));
  _pf.TRASH(4); //address that was stored at top of stack
  stack_again_lbls.pop_back();
  stack_stop_lbls.pop_back();
}

//---------------------------------------------------------------------------
// TODO
//---------------------------------------------------------------------------



void gr8::postfix_writer::do_identifier_node(cdk::identifier_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  // simplified generation: all variables are global
  _pf.ADDR(node->name());
}

void gr8::postfix_writer::do_cell_node(gr8::cell_node *const node, int lvl) { //COMPLETE (?)
  ASSERT_SAFE_EXPRESSIONS;
  node->baseptr()->accept(this, lvl);

  node->cell()->accept(this, lvl);
  
  _pf.INT(node->type()->size());
  _pf.MUL();
  _pf.ADD();
}


void gr8::postfix_writer::do_var_declaration_node(gr8::var_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS

  std::shared_ptr<gr8::symbol> symbol = _symtab.find(node->name());

  if(in_function() && _in_arguments) {              //function argument
    symbol->setOffset(_current_offset);
    _current_offset += node->type()->size();
  }

  if(in_function() && !_in_arguments) {             //local variable
    _current_offset -= node->type()->size();
    symbol->setOffset(_current_offset);

    if(node->init()) {                              //local variable with init
      node->init()->accept(this, lvl);

      if(isDouble(node->type()) && isInt(node->init()->type()))
        _pf.I2D();

      _pf.LOCAL(_current_offset);
      if(isDouble(node->type()))
        _pf.STDOUBLE();
      else
        _pf.STINT();
    }
  }

  if(!in_function() && !node->init()) {             //module global uninitialized variable
    _pf.BSS();
    _pf.ALIGN();

    if(node->isPublic())
      _pf.GLOBAL(node->name(), _pf.OBJ());

    _pf.LABEL(node->name());
    _pf.SALLOC(node->type()->size());

    _pf.TEXT(); //TODO: make sure segment switches are correct
  }

  if(!in_function() && node->init()) {             //module global initialized variable
    int lbl1;
    mklbl(lbl1 = ++_lbl);
    basic_type *t_init = node->init()->type();

    if(isString(t_init)) {                         //create referenceable string first
      _pf.RODATA();
      _pf.ALIGN();
      _pf.LABEL(mklbl(lbl1 = ++_lbl));
      cdk::string_node *init_node = dynamic_cast<cdk::string_node*>(node->init());
      _pf.SSTRING(init_node->value());
    }

    _pf.DATA();
    _pf.ALIGN();
    if(node->isPublic())
      _pf.GLOBAL(node->name(), _pf.OBJ());
    _pf.LABEL(node->name());

    if(isString(t_init)) {
      _pf.SADDR(mklbl(lbl1));                      //initialize with ReadOnly string address
    } else if(isDouble(t_init)) {
      cdk::double_node *init_node = dynamic_cast<cdk::double_node*>(node->init());
      _pf.SDOUBLE(init_node->value());
    } else if(isInt(t_init)) {
      cdk::integer_node *init_node = dynamic_cast<cdk::integer_node*>(node->init());
      _pf.SINT(init_node->value());
    } else if(isPointer(t_init)) {
      //gr8::null_node *init_node = dynamic_cast<gr8::null_node*>(node->init());
      _pf.SINT(0); //should make null_node a literal_node<int> with value 0 (more elegant)
    }

    _pf.TEXT(); //TODO: make sure segment switches are correct
  }
}

void gr8::postfix_writer::do_return_node(gr8::return_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  last_instr_return = true;

  if(node->ret() != nullptr) {
    node->ret()->accept(this, lvl);

    if(isDouble(node->ret()->type()))
      _pf.STFVAL64();
    else
      _pf.STFVAL32();
  }

  _pf.LEAVE();
  _pf.RET(); 
}

void gr8::postfix_writer::do_call_node(gr8::call_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS

  std::string id = node->name();
  if(id == "covfefe")
    id = "_main";
  else if(node->name() == "_main")
    id = "ex_main";

  std::shared_ptr<gr8::symbol> symbol = _symtab.find(id);

  //check if types are consistent with previous declaration
  int i_call = node->args()->nodes().size();

  int args_size = 0;
  for (i_call--; i_call >= 0; --i_call) {
    basic_type* t_func_arg = symbol->param_type_at(i_call);
    node->args()->node(i_call)->accept(this, lvl);

    basic_type* t_call_arg = dynamic_cast<cdk::expression_node *>(node->args()->node(i_call))->type();

    if(isDouble(t_func_arg) && isInt(t_call_arg))
      _pf.I2D();

    if(isDouble(t_func_arg)) {
      _pf.STDOUBLE();
      args_size += 8;
    } else {
      _pf.STINT();
      args_size += 4;
    }
  }

  _pf.CALL(id);
  _pf.TRASH(args_size);

  if (isDouble(symbol->type())) {
    _pf.LDFVAL64(); // delete the evaluated value
  } else if (isInt(symbol->type()) || isPointer(symbol->type()) || isString(symbol->type())) {
    _pf.LDFVAL32();
  } else if (isVoid(symbol->type())) {
    //EMPTY
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void gr8::postfix_writer::do_block_node(gr8::block_node *const node, int lvl) {
  _symtab.push();
  node->declarations()->accept(this, lvl);
  node->instructions()->accept(this, lvl);
  _symtab.pop();
}

void gr8::postfix_writer::do_address_of_node(gr8::address_of_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS
  node->lvalue()->accept(this, lvl);
}

void gr8::postfix_writer::do_function_declaration_node(gr8::function_declaration_node *const node, int lvl) { //missing things (extern etc)
  ASSERT_SAFE_EXPRESSIONS
}

void gr8::postfix_writer::do_function_definition_node(gr8::function_definition_node *const node, int lvl) { //missing things possibly (extern etc)
  ASSERT_SAFE_EXPRESSIONS;
  set_current_function_type(node->type());

  gr8::stack_counter counter(_compiler); \
  node->accept(&counter, 0); 

  _pf.TEXT(); //what if previous segment was text? may need auxiliary function. Might automatically verify if already in TEXT
  _pf.ALIGN();

  if(node->isPublic()) {
    _pf.GLOBAL(node->name(), _pf.FUNC());
  }
  _pf.LABEL(node->name());
  _pf.ENTER(counter.size());

  _symtab.push();

  _current_offset = 8;
  _in_arguments = true;
  node->args()->accept(this, lvl);
  _in_arguments = false;

  _current_offset = 0;
  node->body()->accept(this, lvl);

  _symtab.pop();

  _pf.LEAVE();
  _pf.RET();  

  reset_current_function_type();
}

void gr8::postfix_writer::do_alloc_node(gr8::alloc_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS

  node->argument()->accept(this, lvl);
  _pf.INT(node->type()->subtype()->size());
  _pf.MUL();

  _pf.ALLOC();
  _pf.SP();      //address of allocated memory
}















//-----------------------------------------------------------------------------------------------

void gr8::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
 //needs to be here but useless
}

void gr8::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
 //needs to be here but useless
}

void gr8::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
 //needs to be here but useless
}

void gr8::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
 //needs to be here but useless
}

void gr8::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}


/* old var declaration
void gr8::postfix_writer::do_var_declaration_node(gr8::var_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS

  _pf.ALIGN();

  std::shared_ptr<gr8::symbol> symbol = _symtab.find(node->name());

  if(in_function()) {
    if(_in_arguments) {                               //argument declaration
      symbol->setOffset(_current_offset);
      _current_offset += node->type()->size();

    } else {                                          //local variable declaration
      _current_offset -= node->type()->size();
      symbol->setOffset(_current_offset);

      if(node->init()) {                              //local variable's init
        node->init()->accept(this, lvl);

        if(isDouble(node->type()) && isInt(node->init()->type()))
          _pf.I2D();

        _pf.LOCAL(_current_offset);
        if(isDouble(node->type()))
          _pf.STDOUBLE();
        else
          _pf.STINT();
      }  
    }
  } else {                                            //module global variable declaration
    if(!node->init()) {
      _pf.BSS();
      _pf.ALIGN();

      if(node->isPublic())
        _pf.GLOBAL(node->name(), _pf.OBJ());

      _pf.LABEL(node->name());
      _pf.SALLOC(node->type()->size());
    } else {                                           //literal in init
      int lbl1;
      mklbl(lbl1 = ++_lbl);
      cdk::literal_node init_node = dynamic_cast<cdk::literal_node*>(node->init());
      basic_type *t_init = node->init()->type();

      if(isString(t_init)) {
        _pf.RODATA();
        _pf.ALIGN();
        _pf.LABEL(mklbl(lbl1 = ++_lbl));
        _pf.SSTRING(init_node->value())

        _pf.DATA();
        _pf.ALIGN();
        if(node->isPublic())
          _pf.GLOBAL(node->name(), _pf.OBJ());
        _pf.LABEL(node->name());
        _pf.SADDR(lbl1);
      } else {
        _pf.DATA();
        _pf.ALIGN();
        if(node->isPublic())
          _pf.GLOBAL(node->name(), _pf.OBJ());
        _pf.LABEL(node->name());
        if(isDouble(t_init))
          _pf.STDOUBLE(init_node->value())
        else
          _pf.STINT(init_node->value())
      }

      _pf.TEXT();
    }
  }
}
*/