
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
    // list_m->push_back();
    return nullptr;
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
    if (token_.type == decaf::token_type::Identifier)
    {
        string id = token_.lexeme;
        match(decaf::token_type::Identifier);
        if (token_.type == decaf::token_type::OpAssign)
        {
            match(decaf::token_type::OpAssign);
            auto ex = expr();
            match(decaf::token_type::ptSemicolon);
            return new AssignStmNode(new VariableExprNode(id), new ExprNode(ex));
        }
        else if (token_.type == decaf::token_type::ptLParen)
        {
            match(decaf::token_type::ptLParen);
            auto exl = expr_list();
            match(decaf::token_type::ptRParen);
            match(decaf::token_type::ptSemicolon);
            return new MethodCallExprStmNode(id, exl);
        }
        else
        {
            auto var = variable();
            auto opicdc = op_incr_decr();
            match(decaf::token_type::ptSemicolon);
            if (opicdc == decaf::token_type::OpArtInc)
            {
                return new IncrStmNode(var);
            }
            else
            {
                return new DecrStmNode(var);
            }
        }
    }
    else if (token_.type == decaf::token_type::kwIf)
    {
        match(decaf::token_type::kwIf);
        match(decaf::token_type::ptLParen);
        auto ex = expr();
        match(decaf::token_type::ptRParen);
        auto stmb = statement_block();
        auto opelse = optional_else();

        return new IfStmNode(new ExprNode(ex), new BlockStmNode(stmb), new BlockStmNode(opelse));
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
        auto opicdc = op_incr_decr();
        match(decaf::token_type::ptRParen);
        auto stmb = statement_block();

        if (opicdc == decaf::token_type::OpArtInc)
        {
            return new ForStmNode(new AssignStmNode(var1, ex1), new ExprNode(ex2), new IncrStmNode(var2), new BlockStmNode(stmb));
        }
        else
        {
            return new ForStmNode(new AssignStmNode(var1, ex1), new ExprNode(ex2), new DecrStmNode(var2), new BlockStmNode(stmb));
        }
    }
    else if (token_.type == decaf::token_type::kwReturn)
    {
        match(decaf::token_type::kwReturn);
        auto opex = optional_expr();
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
    else
    {
        auto stmb = statement_block();
        return new StmNode(stmb);
    }
}

ExprNode *HParser::optional_expr()
{
    auto ex = expr();
    if (ex == new ExprNode())
    {
        return new ExprNode();
    }
    return ex;
    //TODO: this probably doenst work, fix for epsilon
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

ExprNode *HParser::expr()
{
    return new ExprNode(expr_and(), expr_p());
}

ExprNode *HParser::expr_p()
{
    if (token_.type == decaf::token_type::OpLogOr)
    {
        match(decaf::token_type::OpLogOr);
        return new OrExprNode(expr_and(), expr_p());
    }
    return nullptr;
}

ExprNode *HParser::expr_and()
{
    return new ExprNode(expr_eq(), expr_and_p());
}

ExprNode *HParser::expr_and_p()
{
    if (token_.type == decaf::token_type::OpLogAnd)
    {
        match(decaf::token_type::OpLogAnd);
        return new AndExprNode(expr_eq(), expr_and_p());
    }
    return nullptr;
}

ExprNode *HParser::expr_eq()
{
    return new ExprNode(expr_rel(), expr_eq_p());
}

ExprNode *HParser::expr_eq_p()
{
    if (token_.type == decaf::token_type::OpRelEQ)
    {
        match(decaf::token_type::OpRelEQ);
        return new EqExprNode(expr_rel(), expr_eq_p());
    }
    else if (token_.type == decaf::token_type::OpRelNEQ)
    {
        match(decaf::token_type::OpRelNEQ);
        return new NeqExprNode(expr_rel(), expr_eq_p());
    }
    else
    {
        return nullptr;
    }
}

ExprNode *HParser::expr_rel()
{
    return new ExprNode(expr_add(), expr_rel_p());
}

ExprNode *HParser::expr_rel_p()
{
    if (token_.type == decaf::token_type::OpRelLT)
    {
        match(decaf::token_type::OpRelLT);
        return new LtExprNode(expr_add(), expr_rel_p());
    }
    else if (token_.type == decaf::token_type::OpRelLTE)
    {
        match(decaf::token_type::OpRelLTE);
        return new LteExprNode(expr_add(), expr_rel_p());
    }
    else if (token_.type == decaf::token_type::OpRelGT)
    {
        match(decaf::token_type::OpRelGT);
        return new GtExprNode(expr_add(), expr_rel_p());
    }
    else if (token_.type == decaf::token_type::OpRelGTE)
    {
        match(decaf::token_type::OpRelGTE);
        return new GteExprNode(expr_add(), expr_rel_p());
    }
    else
    {
        return nullptr;
    }
}

ExprNode *HParser::expr_add()
{
    return new ExprNode(expr_mult(), expr_add_p());
}

ExprNode *HParser::expr_add_p()
{
    if (token_.type == decaf::token_type::OpArtPlus)
    {
        match(decaf::token_type::OpArtPlus);
        return new PlusExprNode(expr_mult(), expr_add_p());
    }
    else if (token_.type == decaf::token_type::OpArtMinus)
    {
        match(decaf::token_type::OpArtMinus);
        return new MinusExprNode(expr_mult(), expr_add_p());
    }
    else
    {
        return nullptr;
    }
}

ExprNode *HParser::expr_mult()
{
    return new ExprNode(expr_unary(), expr_mult_p());
}

ExprNode *HParser::expr_mult_p()
{
    if (token_.type == decaf::token_type::OpArtMult)
    {
        match(decaf::token_type::OpArtMult);
        return new MultiplyExprNode(expr_unary(), expr_mult_p());
    }
    else if (token_.type == decaf::token_type::OpArtDiv)
    {
        match(decaf::token_type::OpArtDiv);
        return new DivideExprNode(expr_unary(), expr_mult_p());
    }
    else if (token_.type == decaf::token_type::OpArtModulus)
    {
        match(decaf::token_type::OpArtModulus);
        return new ModulusExprNode(expr_unary(), expr_mult_p());
    }
    else
    {
        return nullptr;
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
            auto el = expr_list();
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