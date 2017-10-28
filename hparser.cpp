
#include "hparser.h"

using namespace std;

int HParser::parse()
{
    set_AST(program());
    return 0;
}

ProgramNode *HParser::program()
{
    match(decaf::token_type::kwClass);
    string name = token_.lexeme;
    match(decaf::token_type::Identifier);
    match(decaf::token_type::ptLBrace);
    auto list_vdn = variable_declarations();
    match(decaf::token_type::ptRBrace);
    match(decaf::token_type::EOI);
    return new ProgramNode(name, list_vdn, nullptr);
}

list<VariableDeclarationNode *> *HParser::variable_declarations()
{
    auto list_vdn = new list<VariableDeclarationNode *>();
    while (token_.type == decaf::token_type::kwInt ||
           token_.type == decaf::token_type::kwReal)
    {
        ValueType type = this->type();
        auto list_v = variable_list();
        list_vdn->push_back(new VariableDeclarationNode(type, list_v));
    }
    return list_vdn;
}

ValueType HParser::type()
{
    ValueType valuetype = ValueType::VoidVal;
    if (token_.type == decaf::token_type::kwInt)
    {
        match(decaf::token_type::kwInt);
        valuetype = ValueType::IntVal;
    }
    else if (token_.type == decaf::token_type::kwReal)
    {
        match(decaf::token_type::kwReal);
        valuetype = ValueType::RealVal;
    }
    else
    {
        error(token_.type);
    }
    return valuetype;
}

list<VariableExprNode *> *HParser::variable_list()
{
    auto list_v = new list<VariableExprNode *>();

    while (token_.type == decaf::token_type::ptComma)
    {
        match(decaf::token_type::ptComma);
        list_v->push_back(variable());
    }
    match(decaf::token_type::ptSemicolon);
    return list_v;
}

VariableExprNode *HParser::variable()
{
    auto node = new VariableExprNode(token_.lexeme);
    match(decaf::token_type::Identifier);
    return node;
}

list<MethodNode *> *HParser::method_declarations()
{
    auto list_m = new list<MethodNode *>();
    while(token_.type == decaf::token_type::kwStatic)
    {
        auto md = method_declaration();
        list_m->push_back(md);
    }
    return list_m;
}

ValueType HParser::method_return_type()
{
    if (token_.type == decaf::token_type::kwVoid)
    {
        match(decaf::token_type::kwVoid);
        return ValueType::VoidVal;
    }
    return type();
}

MethodNode *HParser::method_declaration()
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
    return new MethodNode(mrt,id,pm,vd,sl);
}

list<ParameterNode *> *HParser::parameters()
{
    auto list_pl = new list<ParameterNode *>();
    if (token_.type == decaf::token_type::kwInt ||
        token_.type == decaf::token_type::kwReal)
    {
        auto pl = parameter_list();
        return pl;
    }
    return list_pl;
}

list<ParameterNode *> *HParser::parameter_list()
{
    auto list_pl = new list<ParameterNode *>();
    auto tp = type();
    string id = token_.lexeme;
    match(decaf::token_type::Identifier);
    list_pl->push_back(new ParameterNode(tp, new VariableExprNode(id)));
    while (token_.type == decaf::token_type::ptComma)
    {
        auto tp = type();
        string id = token_.lexeme;
        match(decaf::token_type::Identifier);
        list_pl->push_back(new ParameterNode(tp, new VariableExprNode(id)));
    }
    return list_pl;
}

//===================================================================================
//STATEMENT
//===================================================================================

list<StmNode *> *HParser::statement_list()
{
    auto list_stm = new list<StmNode *>();
    while (auto stm = statement())
    {
        list_stm->push_back(stm);
    }
    return list_stm;
}

StmNode *HParser::statement()
{
    if (token_.type == decaf::token_type::kwIf)
    {
        match(decaf::token_type::kwIf);
        match(decaf::token_type::ptLParen);
        auto ex = expr();
        match(decaf::token_type::ptRParen);
        auto stmb = statement_block();
        auto opelse = optional_else();

        return new IfStmNode(ex, new BlockStmNode(stmb), new BlockStmNode(opelse));
    }
    else if (token_.type == decaf::token_type::kwFor)
    {
        match(decaf::token_type::kwFor);
        match(decaf::token_type::ptLParen);
        auto var1 = variable();
        match(decaf::token_type::OpAssign);
        auto ex1 = expr();
        match(decaf::token_type::ptSemicolon);
        auto ex2 = expr();
        match(decaf::token_type::ptSemicolon);
        auto var2 = variable();
        string val;
        if(token_.type == decaf::token_type::OpArtInc)
        {
            val = "inc";   
        }
        else if(token_.type == decaf::token_type::OpArtDec)
        {
            val = "dec";
        }
        else
        {
            error(token_.type);
        }
        match(decaf::token_type::ptRParen);
        auto stmb = statement_block();

        if (val == "inc")
        {
            return new ForStmNode(new AssignStmNode(var1, ex1), ex2, new IncrStmNode(var2), new BlockStmNode(stmb));
        }
        else
        {
            return new ForStmNode(new AssignStmNode(var1, ex1), ex2, new DecrStmNode(var2), new BlockStmNode(stmb));
        }
    }
    else if (token_.type == decaf::token_type::kwReturn)
    {
        match(decaf::token_type::kwReturn);
        ExprNode *opex = nullptr;
        if(token_.type == decaf::token_type::ptSemicolon)
        {
            match(decaf::token_type::ptSemicolon);
            return new ReturnStmNode();
        }
        else
        {
            opex = expr();
        }
        match(decaf::token_type::ptSemicolon);
        return new ReturnStmNode(opex);
    }
    else if (token_.type == decaf::token_type::kwBreak)
    {
        match(decaf::token_type::kwBreak);
        match(decaf::token_type::ptSemicolon);
        return new BreakStmNode();
    }
    else if (token_.type == decaf::token_type::kwContinue)
    {
        match(decaf::token_type::kwContinue);
        match(decaf::token_type::ptSemicolon);
        return new ContinueStmNode();
    }
    else if (token_.type == decaf::token_type::Identifier)
    {
        return id_start_stm();
    }
    else
    {
        return statement_block();
    }
}

StmNode *HParser::id_start_stm()
{

        string id = token_.lexeme;
        match(decaf::token_type::Identifier);
        if (token_.type == decaf::token_type::OpAssign)
        {
            match(decaf::token_type::OpAssign);
            auto ex = expr();
            match(decaf::token_type::ptSemicolon);
            return new AssignStmNode(new VariableExprNode(id), ex);
        }
        else if (token_.type == decaf::token_type::ptLParen)
        {
            match(decaf::token_type::ptLParen);
            list<ExprNode*> el = new list<ExprNode*>();
            while(token_.type != decaf::token_type::ptRParen)
            {
                auto ex = expr();
                el->push_back(ex);
                match(decaf::token_type::ptComma);
            }
            match(decaf::token_type::ptRParen);
            return new MethodCallExprStmNode(id, el);
        }
        else
        {
            auto var = variable();
            if (token_.type == decaf::token_type::OpArtInc)
            {
                match(decaf::token_type::ptSemicolon);
                return new IncrStmNode(var);
            }
            else
            {
                match(decaf::token_type::ptSemicolon);
                return new DecrStmNode(var);
            }
        }
}

list<StmNode *> *HParser::statement_block()
{
    match(decaf::token_type::ptLBrace);
    auto sl = statement_list();
    match(decaf::token_type::ptRBrace);
    return sl;
}

list<StmNode *> *HParser::optional_else()
{
    if (token_.type == decaf::token_type::kwElse)
    {
        match(decaf::token_type::kwElse);
        auto stmb = statement_block();
        return stmb;
    }
    return new list<StmNode *>();
}

//=================================================================================
// EXPRESSION
//=================================================================================


ExprNode *HParser::expr()
{
    auto lhs = expr_and();
    return expr_p(lhs);
}

ExprNode *HParser::expr_p(ExprNode *lhs)
{
    ExprNode* node = nullptr;
    if (token_.type == decaf::token_type::OpLogOr)
    {
        match(decaf::token_type::OpLogOr);
        auto rhs = expr_and();
        node = new OrExprNode(lhs,rhs);
        return expr_p(node);
    }
    return lhs;
}

ExprNode *HParser::expr_and()
{
    auto lhs = expr_eq();
    return expr_and_p(lhs);
}

ExprNode *HParser::expr_and_p(ExprNode *lhs)
{
    ExprNode* node = nullptr;
    if (token_.type == decaf::token_type::OpLogAnd)
    {
        match(decaf::token_type::OpLogAnd);
        auto rhs = expr_eq();
        node = new AndExprNode(lhs,rhs);
        return expr_and_p(node);
    }
    return lhs;
}

ExprNode *HParser::expr_eq()
{
    auto lhs = expr_rel();
    return expr_eq_p(lhs);
}

ExprNode *HParser::expr_eq_p(ExprNode *lhs)
{
    ExprNode *node = nullptr;
    if (token_.type == decaf::token_type::OpRelEQ)
    {
        match(decaf::token_type::OpRelEQ);
        auto rhs = expr_rel();
        node = new EqExprNode(lhs,rhs);
        return expr_eq_p(node);
    }
    else if (token_.type == decaf::token_type::OpRelNEQ)
    {
        match(decaf::token_type::OpRelNEQ);
        auto rhs = expr_rel();
        node = new NeqExprNode(lhs, rhs);
        return expr_eq_p(node);
    }
    else
    {
        return lhs;
    }
}

ExprNode *HParser::expr_rel()
{
    auto lhs = expr_add();
    return expr_add_p(lhs);
}

ExprNode *HParser::expr_rel_p(ExprNode *lhs)
{
    ExprNode *node = nullptr;
    if (token_.type == decaf::token_type::OpRelLT)
    {
        match(decaf::token_type::OpRelLT);
        auto rhs = expr_add();
        node = new LtExprNode(lhs, rhs);
        return expr_rel_p(node);
    }
    else if (token_.type == decaf::token_type::OpRelLTE)
    {
        match(decaf::token_type::OpRelLTE);
        auto rhs = expr_add();
        node = new LteExprNode(lhs, rhs);
        return expr_rel_p(node);
    }
    else if (token_.type == decaf::token_type::OpRelGT)
    {
        match(decaf::token_type::OpRelGT);
        auto rhs = expr_add();
        node = new GtExprNode(lhs, rhs);
        return expr_rel_p(node);
    }
    else if (token_.type == decaf::token_type::OpRelGTE)
    {
        match(decaf::token_type::OpRelGTE);
        auto rhs = expr_add();
        node = new GteExprNode(lhs,rhs);
        return expr_rel_p(node);
    }
    else
    {
        return lhs;
    }
}

ExprNode *HParser::expr_add()
{
    auto lhs = expr_mult();
    return expr_add_p(lhs);
}

ExprNode *HParser::expr_add_p(ExprNode *lhs)
{
    if (token_.type == decaf::token_type::OpArtPlus)
    {
        ExprNode *node = nullptr;
        match(decaf::token_type::OpArtPlus);
        auto rhs = expr_mult();
        node = new PlusExprNode(lhs,rhs);
        return expr_add_p(node);
    }
    else if (token_.type == decaf::token_type::OpArtMinus)
    {
        return new MinusExprNode(expr_mult(), expr_add_p());
        ExprNode *node = nullptr;
        match(decaf::token_type::OpArtMinus);
        auto rhs = expr_mult();
        node = new MinusExprNode(lhs,rhs);
        return expr_add_p(node);
    }
    else
    {
       return lhs;
    }
}

ExprNode *HParser::expr_mult()
{
    auto lhs = expr_unary();
    return expr_mult_p(lhs);
}

ExprNode *HParser::expr_mult_p(ExprNode *lhs)
{
    ExprNode *node = nullptr;
    if (token_.type == decaf::token_type::OpArtMult)
    {
        match(decaf::token_type::OpArtMult);
        auto rhs = expr_unary();
        node = new MultiplyExprNode(lhs,rhs);
        return expr_mult_p(node);
    }
    else if (token_.type == decaf::token_type::OpArtDiv)
    {
        match(decaf::token_type::OpArtDiv);
        auto rhs = expr_unary();
        node = new DivideExprNode(lhs,rhs);
        return expr_mult_p(node);
    }
    else if (token_.type == decaf::token_type::OpArtModulus)
    {
        match(decaf::token_type::OpArtModulus);
        auto rhs = expr_unary();
        node = new ModulusExprNode(lhs,rhs);
        return expr_mult_p(node);
    }
    else
    {
        return lhs;
    }
}

ExprNode *HParser::expr_unary()
{
    if (token_.type == decaf::token_type::OpArtPlus)
    {
        match(decaf::token_type::OpArtPlus);
        return new PlusExprNode(expr_unary());
    }
    else if (token_.type == decaf::token_type::OpArtMinus)
    {
        match(decaf::token_type::OpArtMinus);
        return new MinusExprNode(expr_unary());
    }
    else if (token_.type == decaf::token_type::OpLogNot)
    {
        match(decaf::token_type::OpLogNot);
        return new NotExprNode(expr_unary());
    }
    else
    {
        return factor();
    }
}

ExprNode *HParser::factor()
{
    if (token_.type == decaf::token_type::Number)
    {
        match(decaf::token_type::Number);
        string value = token_.lexeme;
        return new NumberExprNode(value);
    }
    else if (token_.type == decaf::token_type::ptLParen)
    {
        match(decaf::token_type::ptLParen);
        auto ex = expr();
        match(decaf::token_type::ptRParen);
        return ex;
    }
    else if (token_.type == decaf::token_type::Identifier)
    {
        match(decaf::token_type::Identifier);
        auto id = token_.lexeme;
        if (token_.type == decaf::token_type::ptLParen)
        {
            match(decaf::token_type::ptLParen);
            list<ExprNode*> el = new list<ExprNode*>();
            while(token_.type != decaf::token_type::ptRParen)
            {
                auto ex = expr();
                el->push_back(ex);
                match(decaf::token_type::ptComma);
            }
            match(decaf::token_type::ptRParen);
            return new MethodCallExprStmNode(id, el);
        }
        else
        {
            return new VariableExprNode(id);
        }
    }
    else
    {
        error(token_.type);
    }
}
