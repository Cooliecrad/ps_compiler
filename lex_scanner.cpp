#include "lex_analysis.h"
#include "lex_scanner.h"

#include "configure.h"

using namespace ps::lex;

#ifdef __SAVE_TEMP
#endif

LexScanner::LexScanner(vTokenResult& token_output, const char *output)
    : vResult {token_output}
{
    // 有设置路径就那输出
    if (output)
    {
        std::string output_name {output};
        output_name.append(".i");
        this->output = fopen(output_name.c_str(), "w");
    }
}

LexScanner::~LexScanner()
{
    // 有设置路径就那输出
    fclose(output);
}

/**
 * @brief 将token送入词法分析器。正确返回0，错误返回1
 */
int LexScanner::__send_lexa(int start, int len)
{
    TokenResult ret = lexa.lex_analyse(bf+start, len);
    if (ret.type == TokenType::error)
    {
        // 为了拷贝字符，临时截断
        char *end = bf+start+ret.scan;
        char tmp = *end;
        *end = '\0'; // 为了打印函数拷贝用
        fprintf(stderr, "[ERROR] invalid token '%s' at line %d, col %d\n",
               bf+start, line, (int)ret.scan);
        *end = tmp;

        return 1;
    }
    else
    {
        ret.col = start;
        ret.line = line;
        vResult.emplace_back(ret);
    }
    return 0;
}

void LexScanner::line_scan()
{
    // 扫描
    __line_scan();

    // 只有需要输出的时候才会输出到文件中
    if (output != NULL)
    {
        // 空行就跳过吧
        if (print_index < vResult.size())
        { // 打印行号
            fprintf(output, "%d ", line);
            // 打印二元式
            for (; print_index< vResult.size(); print_index++)
                vResult[print_index].fprint(output);
            fprintf(output, "\n");
        }
    }

    line++;
}

/**
 * @brief 扫描单行，返回结果
 */
void LexScanner::__line_scan()
{
    int start = -1; // 当前token开始的位置
    int star_flag = 0; // 标记: 上一个字符是*
    int slash_flag = 0; // 标记: 上一个字符是/
    int line_comment = 0; // 标记: 本行进入注释
    int index = -1; // 当前字符
    // 读取到间隔符再将符号载入词法分析器
    do
    {
        index++;
        // 多行注释模式
        if (multi_comment)
        {
            switch (bf[index])
            {
            case '*':
                star_flag = 1;
                break;
            case '/':
                if (star_flag) multi_comment = 0; // 读到 */
            default:
                star_flag = 0;
                break;
            }
        } else
        {
            switch (bf[index])
            {
            case '*':
                if (slash_flag) multi_comment = 1; // 读到 /* 
                start = index;
                star_flag = 1;
                slash_flag = 0;
                break;
            case '/':
                if (slash_flag) line_comment = 1; // 读到 //
                start = index;
                star_flag = 0;
                slash_flag = 1;
                break;
            case '\0':
            case '\n':
            case '\t':
            case ';':
            case ' ': // 读到分隔符，可以送入分析器进行分析了
                if (start != -1)
                    __send_lexa(start, index-start);
                star_flag = 0;
                slash_flag = 0;
                start = -1;
                break;
            default:
                if (start == -1) start = index; // 若之前没有读取字符，就从这里开始
                star_flag = 0;
                slash_flag = 0;
                break;
            }
        }
    } while (bf[index]);
}

int LexScanner::scan(FILE *input)
{
    int output_cnt = 0;
    print_index = 0;
    char *ret = fgets(bf, MAX_LINE_WIDTH, input);
    while (ret != nullptr)
    {
        line_scan();
        output_cnt += vResult.size();
        ret = fgets(bf, MAX_LINE_WIDTH, input);
    }
    return output_cnt;
}