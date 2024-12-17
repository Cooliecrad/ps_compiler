#ifndef __PS_LEX_SCANNER_H__
#define __PS_LEX_SCANNER_H__

#include "configure.h"
#include "lex_analysis.h"
#include <vector>

namespace ps
{
namespace lex
{

class LexScanner
{
public:
    /**
     * @brief 保存了词法分析结果的数组
     */
    using vTokenResult = std::vector<TokenResult>;

    /**
     * @brief 初始化词法扫描器
     * @param token_output 输出结果到指定的vector
     * @param output 如果需要保存中间结果，则指向保存结果的路径名。默认NULL，不输出。
     */
    LexScanner(vTokenResult& token_output, const char *output = nullptr);
    ~LexScanner();
    int scan(FILE *input);
    void line_scan();
private:
    // 将所有结果打印到stdout
    void __line_scan();
    int __send_lexa(int start, int len);

    LexAnalysis lexa;

    vTokenResult& vResult;

    FILE *output = NULL;
    char bf[MAX_LINE_WIDTH];
    int print_index = 0;

    int line = 0;
    bool multi_comment = false;
};

}
}

#endif