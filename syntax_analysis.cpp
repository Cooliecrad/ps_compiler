#include "syntax_analysis.h"

using namespace ps::lex;
using namespace ps::syntax;

SyntaxAnalysis::SyntaxAnalysis(const char *output)
    : lexs {tokens, output}
{
    // 有设置路径就那输出
    if (output)
    {
        std::string output_name {output};
        output_name.append(".ii");
        this->output = fopen(output_name.c_str(), "w");
    }
}

void SyntaxAnalysis::scan(FILE *input)
{
    lexs.scan(input);
    if (!__program()) error_handle();
}

bool SyntaxAnalysis::next_token()
{
    if (token_index < tokens.size() || token_index == -1)
    {
        token_index++;
        token = &tokens[token_index];
        return true;
    }
    fprintf(output, "[ERROR] Syntax analysis failed, end of file.\n");
    return false;
}

bool SyntaxAnalysis::back_token()
{
    if (token_index > 0)
    {
        token_index--;
        token = &tokens[token_index];
        return true;
    }
    fprintf(output, "[ERROR] Syntax analysis failed, invalid back_token().\n");
    return false;
}

void SyntaxAnalysis::error_handle()
{
    fprintf(output, "[ERROR] invaild token '");
    token->fprint(output);
    fprintf(output, "' at line %d col %d\n", token->line, token->col);
}

#define __next_lbr if (!next_token() || token->type != TokenType::symbol_lbrace) return false;
#define __next_rbr if (!next_token() || token->type != TokenType::symbol_rbrace) return false;
#define __next_lbk if (!next_token() || token->type != TokenType::symbol_lbracket) return false;
#define __next_rbk if (!next_token() || token->type != TokenType::symbol_rbracket) return false;
#define __next_stmts if (!__stmts()) return false;
#define __next_stmt if (!__stmt()) return false;
#define __next_expr if (!__expr()) return false;
#define __next_bool if(!__bool()) return false;
#define __next_term if (!__term()) return false;
#define __next_factor if (!__factor()) return false;
#define __next_assign if (!next_token()  || token->type != TokenType::symbol_assign) return false;
bool SyntaxAnalysis::__program()
{
    fprintf(output, "syna >> program -> block\n");
    if (!__block()) return false;
    fprintf(output, "[INFO] Finished Syntax analysis.\n");
    return true;
}
bool SyntaxAnalysis::__block()
{
    fprintf(output, "syna >> block -> {stmts}\n");
    __next_lbr
    __next_stmts
    __next_rbr
    return true;
}
bool SyntaxAnalysis::__stmts()
{
    if (token_end())
    {
        fprintf(output, "syna >> stmts -> ɛ\n");
        return true;
    }

    next_token();
    switch (token->type)
    {
    case TokenType::identifier:
    case TokenType::kw_if:
    case TokenType::kw_while:
    case TokenType::kw_do:
    case TokenType::kw_break:
    case TokenType::symbol_lbrace:
        fprintf(output, "syna >> stmts -> stmt stmts\n");
        back_token();
        return __stmt() && __stmts();
    }
    back_token();
    fprintf(output, "syna >> stmts -> ɛ\n");
    return true;
}
bool SyntaxAnalysis::__stmt()
{
    if (!next_token()) return false;
    switch (token->type)
    {
    case TokenType::identifier:
        fprintf(output, "syna >> stmt -> id = expr\n");
        __next_assign
        return __expr();

    case TokenType::kw_if:
        fprintf(output, "syna >> stmt -> if (bool) stmt E\n");
        __next_lbk
        __next_bool
        __next_rbk
        __next_stmt
        return __E();

    case TokenType::kw_while:
        fprintf(output, "syna >> stmt -> while (bool) stmt\n");
        __next_lbk
        __next_bool
        __next_rbk
        return __stmt();

    case TokenType::kw_do:
        fprintf(output, "syna >> stmt -> do stmt while (bool)\n");
        __next_stmt
        if (!next_token() || token->type != TokenType::kw_while) return false;
        __next_lbk
        __next_bool
        __next_rbk
        return true;

    case TokenType::kw_break:
        fprintf(output, "syna >> stmt -> break\n");
        return true;

    case TokenType::symbol_lbrace:
        fprintf(output, "syna >> stmt -> {stmts}\n");
        __next_stmts
        __next_rbr
        return true;
    }
    return false;
}
bool SyntaxAnalysis::__E()
{
    if (token_end())
    {
        fprintf(output, "syna >> E -> ɛ\n");
        return true;
    }
    fprintf(output, "syna >> E -> else stmt\n");
    if (!next_token() || token->type != TokenType::kw_else) return false;
    return true;
}
bool SyntaxAnalysis::__bool()
{
    fprintf(output, "syna >> bool -> expr bexpr\n");
    __next_expr
    return __bexpr();
}
bool SyntaxAnalysis::__bexpr()
{
    if (token_end())
    {
        fprintf(output, "syna >> bexpr -> ɛ\n");
        return true;
    }
    if (next_token())
    {
        switch (token->type)
        {
        case TokenType::symbol_less:
            fprintf(output, "syna >> bexpr -> < expr\n");
            return __expr();
        case TokenType::symbol_greater:
            fprintf(output, "syna >> bexpr -> > expr\n");
            return __expr();
        case TokenType::symbol_le:
            fprintf(output, "syna >> bexpr -> <= expr\n");
            return __expr();
        case TokenType::symbol_ge:
            fprintf(output, "syna >> bexpr -> >= expr\n");
            return __expr();
        }
    }
    return false;
}
bool SyntaxAnalysis::__expr()
{
    fprintf(output, "syna >> expr -> term exprx\n");
    __next_term
    return __exprx();
}
bool SyntaxAnalysis::__exprx()
{
    if (token_end())
    {
        fprintf(output, "syna >> exprx -> ɛ\n");
        return true;
    }
    next_token();
    switch (token->type)
    {
    case TokenType::symbol_plus:
        fprintf(output, "syna >> exprx -> + term exprx\n");
        __next_term
        return __exprx();
    case TokenType::symbol_minus:
        fprintf(output, "syna >> exprx -> - term exprx\n");
        __next_term
        return __exprx();
    }

    back_token();
    fprintf(output, "syna >> exprx -> ɛ\n");
    return true;
}
bool SyntaxAnalysis::__term()
{
    fprintf(output, "syna >> term -> factor termx\n");
    __next_factor
    return __termx();
}
bool SyntaxAnalysis::__termx()
{
    if (token_end())
    {
        fprintf(output, "syna >> termx -> ɛ\n");
        return true;
    }
    next_token();
    switch (token->type)
    {
    case TokenType::symbol_mul:
        fprintf(output, "syna >> termx -> * factor termx\n");
        __next_term
        return __termx();
    case TokenType::symbol_div:
        fprintf(output, "syna >> termx -> / factor termx\n");
        __next_term
        return __termx();
    }

    back_token();
    fprintf(output, "syna >> termx -> ɛ\n");
    return true;
}
bool SyntaxAnalysis::__factor()
{
    if (!next_token()) return false;
    if (token->type == TokenType::symbol_lbracket)
    {
        fprintf(output, "syna >> factor -> (expr)\n");
        __next_expr
        __next_rbk
        return true;
    }
    if (token->type == TokenType::identifier)
    {
        fprintf(output, "syna >> factor -> id\n");
        return true;
    }
    if (token->type & NUMBER_MASK)
    {
        fprintf(output, "syna >> factor -> num\n");
        return true;
    }
    return false;
}