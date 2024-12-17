#ifndef __PS_SYNTAX_ANALYSIS_H__
#define __PS_SYNTAX_ANALYSIS_H__

#include "lex_scanner.h"

namespace ps
{
namespace syntax
{

class SyntaxAnalysis
{
public:
    /**
     * @brief 初始化递归语法分析器
     * @param output 如果需要保存中间结果，则指向保存结果的路径名。默认NULL，不输出。
     */
    SyntaxAnalysis(const char *output = nullptr);
    void scan(FILE *input);
private:
    bool next_token(); // 跳到下一个token，如果到达结尾，返回false
    bool back_token(); // 回到上一个token
    bool token_end() {return token_index == tokens.size();} // 读取到结尾
    void error_handle();

    bool __program();
    bool __block();
    bool __stmts();
    bool __stmt();
    bool __E();
    bool __bool();
    bool __bexpr();
    bool __expr();
    bool __exprx();
    bool __term();
    bool __termx();
    bool __factor();

    int token_index = -1; // 读取到了token的第几个
    lex::LexScanner::vTokenResult tokens;
    lex::TokenResult *token = NULL;

    lex::LexScanner lexs;
    FILE *output;
};

}
}


#endif