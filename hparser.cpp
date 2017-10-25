
#include "hparser.h"

using namespace std;

int HParser::parse()
{
    set_AST( program() );
    return 0;
}

ProgramNode*
HParser::program() {
    match( decaf::token_type::kwClass );
    string name = token_.lexeme;
    match( decaf::token_type::Identifier );
    match( decaf::token_type::ptLBrace );
    auto list_vdn = variable_declarations();
    match( decaf::token_type::ptRBrace );
    match( decaf::token_type::EOI );
    return new ProgramNode(name, list_vdn, nullptr);
}


list<VariableDeclarationNode*>*
HParser::variable_declarations()
{
    auto list_vdn = new list<VariableDeclarationNode*>();
    while ( token_.type == decaf::token_type::kwInt ||
            token_.type == decaf::token_type::kwReal ) {
        ValueType type = this->type();
        auto list_v = variable_list();
        list_vdn->push_back( new VariableDeclarationNode( type, list_v ) );
    }
    return list_vdn;
}


ValueType HParser::type()
{
    ValueType valuetype = ValueType::VoidVal;
    if ( token_.type == decaf::token_type::kwInt ) {
        match( decaf::token_type::kwInt );
        valuetype = ValueType::IntVal;
    }
    else if ( token_.type == decaf::token_type::kwReal ) {
        match( decaf::token_type::kwReal );
        valuetype = ValueType::RealVal;
    }
    else {
       error( decaf::token_type::kwInt );
    }
    return valuetype;
}


list<VariableExprNode*>*
HParser::variable_list()
{
    auto list_v = new list<VariableExprNode*>();

    while ( token_.type == decaf::token_type::ptComma ) {
        match( decaf::token_type::ptComma );
        list_v->push_back( variable() );
    }
    match( decaf::token_type::ptSemicolon );
    return list_v;
}


VariableExprNode*
HParser::variable()
{
    auto node = new VariableExprNode(token_.lexeme);
    match( decaf::token_type::Identifier );
    return node;
}

list<MethodNode*>*
HParser::method_declarations()
{
    auto list_m = new list<MethodNode*>();
    list_m->push_back();
    while()
}

ValueType HParser::method_return_type()
{
    if(token_.type == decaf::token_type::kwVoid)
    {
        match(decaf::token_type::kwVoid);
        return ValueType::VoidVal;
    }
    return type();

}

MethodNode HParser::method_declaration()
{
    match(decaf::token_type::kwStatic);
    auto mrt = method_return_type();
    string id = token_.lexeme;
    match(decaf::token_type::Identifier);
    match(decaf::token_type::ptLParen);
    auto pm = parameters();
    match(decaf::token_type::ptRParen);
    match(decaf::token_type::ptLBrace);
    auto vd = variable_declarations();
    auto sl = statement_list();
    match(decaf::token_type::ptRBrace);
    
}

list<ParameterNode*>* HParser::parameters()
{
    auto list_pl = list<ParameterNode*>();
    if(   token_.type == decaf::token_type::kwInt ||
          token_.type == decaf::token_type::kwReal)
          {
                auto pl = parameter_list();
                return pl;
          }
    return list_pl;
}

list<ParameterNode*>* HParser::parameter_list()
{
    auto list_pl = list<ParameterNode*>();
    auto tp = type();
    string id = token_.lexeme;
    match(decaf::token_type::Identifier)
    list_pl->push_back(new ParameterNode(tp,id));
    while(token_.type == decaf::token_type::ptComma)
    {
        auto tp = type();
        string id = token_.lexeme;
        match(decaf::token_type::Identifier)
        list_pl->push_back(new ParameterNode(tp,id));
    }
    return list_pl;
}
list<StmNode*>* statement_list()
{
    auto list_stm = list<StmNode*>();
    while(auto stm = statement())
    {
       list_stm->push_back(stm);
    }
    return list_stm;
}
list<StmNode*>* statement()
{
    if(token_.type == decaf::token_type::Identifier)
    {
        
    }
}

list<StmNode*>* statement_block()
{
    match(decaf::token_type::ptLBrace);
    auto sl = statement_list();
    match(decaf::token_type::ptRBrace);
    return sl;
}

