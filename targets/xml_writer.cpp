#include <string>
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

std::string typeToString(basic_type *type) {
  if(type->name()==basic_type::TYPE_INT) return "small";
  else if(type->name()==basic_type::TYPE_STRING) return "news";
  else if(type->name()==basic_type::TYPE_VOID) return "void";
  else if(type->name()==basic_type::TYPE_DOUBLE) return "huge";
  else if(type->name()==basic_type::TYPE_POINTER) return (typeToString(type->subtype()) + "*");
  else return "unknown";
}


//---------------------------------------------------------------------------

void gr8::xml_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << " />" << std::endl;
}
void gr8::xml_writer::do_data_node(cdk::data_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label()
       << "size ='" << node->size() << "'>" << std::endl;
  closeTag(node, lvl);
} 
void gr8::xml_writer::do_not_node(cdk::not_node * const node, int lvl) {
  do_unary_expression(node, lvl);
}
void gr8::xml_writer::do_and_node(cdk::and_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void gr8::xml_writer::do_or_node(cdk::or_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}

//---------------------------------------------------------------------------

void gr8::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  if ( node->size() > 0) {
    os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
    for (size_t i = 0; i < node->size(); i++)
      node->node(i)->accept(this, lvl + 2);
    closeTag(node, lvl);
  } else {
    os() << std::string(lvl, ' ') << "<sequence_node size='0'/>" << std::endl;
  }

}

//---------------------------------------------------------------------------

void gr8::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  process_literal(node, lvl);
}

void gr8::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  process_literal(node, lvl);
}
void gr8::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
  process_literal(node, lvl);
}

//---------------------------------------------------------------------------

void gr8::xml_writer::do_unary_expression(cdk::unary_expression_node * const node, int lvl) {
  /* ASSERT_SAFE_EXPRESSIONS; */
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void gr8::xml_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  do_unary_expression(node, lvl);
}

//---------------------------------------------------------------------------

void gr8::xml_writer::do_binary_expression(cdk::binary_expression_node * const node, int lvl) {
  /* ASSERT_SAFE_EXPRESSIONS; */
  openTag(node, lvl);
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void gr8::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void gr8::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void gr8::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void gr8::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void gr8::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void gr8::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void gr8::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void gr8::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void gr8::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void gr8::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void gr8::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}

//---------------------------------------------------------------------------

void gr8::xml_writer::do_identifier_node(cdk::identifier_node * const node, int lvl) {
  /* ASSERT_SAFE_EXPRESSIONS; */
  os() << std::string(lvl, ' ') << "<" << node->label() << ">" << node->name() << "</" << node->label() << ">" << std::endl;
}

void gr8::xml_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  /* ASSERT_SAFE_EXPRESSIONS; */
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void gr8::xml_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  /* ASSERT_SAFE_EXPRESSIONS; */
  openTag(node, lvl);
  openTag("lvalue", lvl+2);
  node->lvalue()->accept(this, lvl + 4);
  closeTag("lvalue", lvl+2);
  // reset_new_symbol();

  openTag("rvalue", lvl+2);
  node->rvalue()->accept(this, lvl + 4);
  openTag("rvalue", lvl+2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------
/*
void gr8::xml_writer::do_program_node(gr8::program_node * const node, int lvl) {
  openTag(node, lvl);
  node->statements()->accept(this, lvl + 4);
  closeTag(node, lvl);
}
*/
//---------------------------------------------------------------------------

void gr8::xml_writer::do_evaluation_node(gr8::evaluation_node * const node, int lvl) {
  /* ASSERT_SAFE_EXPRESSIONS; */
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void gr8::xml_writer::do_print_node(gr8::print_node * const node, int lvl) {
  /* ASSERT_SAFE_EXPRESSIONS; */
  os() << std::string(lvl, ' ') << "<" << node->label() 
  << " newline='" << std::boolalpha << node->newline() << "'>" << std::endl;
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void gr8::xml_writer::do_read_node(gr8::read_node * const node, int lvl) {
  /*ASSERT_SAFE_EXPRESSIONS; */
  os() << std::string(lvl, ' ') << "<" << node->label() << " />" << std::endl;
}

//---------------------------------------------------------------------------
/*
void gr8::xml_writer::do_while_node(gr8::while_node * const node, int lvl) {
   ASSERT_SAFE_EXPRESSIONS; 
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}
*/
//---------------------------------------------------------------------------

void gr8::xml_writer::do_if_node(gr8::if_node * const node, int lvl) {
  /* ASSERT_SAFE_EXPRESSIONS; */
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  closeTag(node, lvl);
}

void gr8::xml_writer::do_if_else_node(gr8::if_else_node * const node, int lvl) {
  /* ASSERT_SAFE_EXPRESSIONS; */
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  openTag("else", lvl + 2);
  node->elseblock()->accept(this, lvl + 4);
  closeTag("else", lvl + 2);
  closeTag(node, lvl);
}
//---------------------------------------------------------------------------
//TODO
//---------------------------------------------------------------------------

void gr8::xml_writer::do_stop_node(gr8::stop_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() 
       << " ncycles='" << node->ncycles() << "' />" << std::endl;
}
void gr8::xml_writer::do_again_node(gr8::again_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() 
       << " ncycles='" << node->ncycles() << "' />" << std::endl;
}
void gr8::xml_writer::do_cell_node(gr8::cell_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("baseptr", lvl + 2);
  node->baseptr()->accept(this, lvl + 4);
  closeTag("baseptr", lvl + 2);
  openTag("cell", lvl + 2);
  node->cell()->accept(this, lvl + 4);
  closeTag("cell", lvl + 2);
  closeTag(node, lvl);
}
void gr8::xml_writer::do_sweeping_node(gr8::sweeping_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("sweep", lvl+2);
  node->sweep()->accept(this, lvl+4);
  closeTag("sweep", lvl+2);
  openTag("from", lvl+2);
  node->from()->accept(this, lvl+4);
  closeTag("from", lvl+2);
  openTag("to", lvl+2);
  node->to()->accept(this, lvl+4);
  closeTag("to", lvl+2);
  openTag("by", lvl+2);
  node->by()->accept(this, lvl+4);
  closeTag("by", lvl+2);
  openTag("do", lvl+2);
  node->doBlock()->accept(this, lvl+4);
  closeTag("do", lvl+2);
  closeTag(node, lvl);
}
void gr8::xml_writer::do_var_declaration_node(gr8::var_declaration_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << " qualifier='";
  if (node->noQualifier()) os() << "";
  else if (node->isPublic()) os() << "public";
  else if (node->isUse()) os() << "use";
  os() << "' type='" <<  typeToString(node->type()) << "'"
       << " name='" << node->name() << "'"; 
  if(node->init() != nullptr) {
    os() << ">" << std::endl;
    openTag("init", lvl+2);
    node->init()->accept(this, lvl + 4);
    closeTag("init", lvl+2);
    return;
  }
  os() << " />" << std::endl;
}

void gr8::xml_writer::do_return_node(gr8::return_node *const node, int lvl) {
  openTag(node, lvl);
  node->ret()->accept(this, lvl + 2);
  closeTag(node, lvl);
}
void gr8::xml_writer::do_call_node(gr8::call_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label()
       << " name='" << node->name() << "'>" << std::endl;
  node->args()->accept(this, lvl + 2);
  closeTag(node, lvl);
}
void gr8::xml_writer::do_identity_node(gr8::identity_node *const node, int lvl) {
  do_unary_expression(node, lvl);
}
void gr8::xml_writer::do_block_node(gr8::block_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("decls", lvl + 2);
  node->declarations()->accept(this, lvl + 4);
  closeTag("decls", lvl + 2);
  openTag("instrs", lvl + 2); 
  node->instructions()->accept(this, lvl + 4);
  closeTag("instrs", lvl + 2);
  closeTag(node, lvl);
}
void gr8::xml_writer::do_address_of_node(gr8::address_of_node *const node, int lvl) {
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 2);
  closeTag(node, lvl);
}
void gr8::xml_writer::do_null_node(gr8::null_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << " />" << std::endl;
}
void gr8::xml_writer::do_function_declaration_node(gr8::function_declaration_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << " qualifier='";
  if (node->noQualifier()) os() << "";
  else if (node->isPublic()) os() << "public";
  else if (node->isUse()) os() << "use";
  os() << "' type='" <<  typeToString(node->type()) << "'"
       << " name='" << node->name() << "'>" << std::endl;
  openTag("args", lvl+2);
  node->args()->accept(this, lvl + 4);
  closeTag("args", lvl+2);
  closeTag(node, lvl);
}
void gr8::xml_writer::do_function_definition_node(gr8::function_definition_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << " qualifier='";
  if (node->noQualifier()) os() << "";
  else if (node->isPublic()) os() << "public";
  else if (node->isUse()) os() << "use";
  os() << "' type='" <<  typeToString(node->type()) << "'"
       << " name='" << node->name() << "'>" << std::endl;
  openTag("args", lvl+2); 
  node->args()->accept(this, lvl + 4);
  closeTag("args", lvl+2);
  openTag("body", lvl+2);
  node->body()->accept(this, lvl + 4);
  closeTag("body", lvl+2);
  closeTag(node, lvl);
}
void gr8::xml_writer::do_alloc_node(gr8::alloc_node *const node, int lvl) {
  do_unary_expression(node, lvl);
}
