#ifndef __PS_LEX_ANALYSIS_H__
#define __PS_LEX_ANALYSIS_H__

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include "configure.h"

namespace ps
{
namespace lex
{

const uint16_t ERROR_MASK       = 0x0000;
const uint16_t ID_MASK          = 0x1000;
const uint16_t NUMBER_MASK      = 0x2000;
const uint16_t UNSIGNED_MASK    = 0x0800;
const uint16_t FLOAT_MASK       = 0x0400;
const uint16_t CHR_STR_MASK     = 0x4000;
const uint16_t KW_MASK          = 0x8000;

enum TokenType : int32_t
{
    // token类型：第15-12位
    // 0000-错误 
    error = ERROR_MASK, // 错误的标识符，仅用于测试

    // 0001-标识符 
    identifier = ID_MASK, // identifier

    // 0010-数字
    // 数字类型：第10位：整型/浮点
    //          第11位: 有符号/无符号
    number_INT = NUMBER_MASK, // int
    number_L, // long
    number_LL, // long long
    number_OCT, // octal
    number_LO, // long octal
    number_LLO, // long long octal
    number_HEX, // hex
    number_LH, // long hex
    number_LLH, // long long hex
    number_zero, // literal ZERO

    number_U = NUMBER_MASK | UNSIGNED_MASK, // unsigned int
    number_UL, // unsigned long
    number_ULL, // unsigned long long
    number_UO, // unsigned octal
    number_ULO, // unsigned long octal
    number_ULLO, // unsigned long long octal
    number_UH, // unsigned hex
    number_ULH, // unsigned long hex
    number_ULLH, // unsigned long long hex

    number_DOUBLE = NUMBER_MASK | FLOAT_MASK, // double
    number_E_NOTATION, // double with scientific notation

    // 0100-字面字符（串）
    literal_char = CHR_STR_MASK, // literal character
    literal_string, // literal string

    // 1000-保留字/符号/注释
    // 保留字
    kw_if = KW_MASK,
    kw_else,
    kw_while,
    kw_do,
    kw_for,
    kw_main,
    kw_char,
    kw_int,
    kw_long,
    kw_float,
    kw_double,
    kw_return,
    kw_void,
    kw_const,
    kw_continue,
    kw_break,
    kw_unsigned,
    kw_enum,
    kw_swtich,
    kw_case,
    kw_auto,
    kw_static,
    kw_register,
    kw_extern,
    // 特殊符号
    symbol_plus, // +
    symbol_minus, // -
    symbol_mul, // *
    symbol_div, // /
    symbol_mod, // %
    symbol_bit_and, // &
    symbol_bit_or, // |
    symbol_bit_not, // ~
    symbol_bit_exor, // ^
    symbol_assign, // =
    symbol_less, // <
    symbol_greater, // >
    symbol_lbrace, // {
    symbol_rbrace, // }
    symbol_colon, // ;
    symbol_lbracket, // (
    symbol_rbracket, // )
    symbol_logic_not, // !
    symbol_equal, // ==
    symbol_nequal, // !=
    symbol_logic_and, // &&
    symbol_logic_or, // ||
    symbol_le, // <=
    symbol_ge, // >=
    comment, // comment
};

using ReservedTab = std::unordered_map<std::string, TokenType>;
using IDTab = std::unordered_set<std::string>;
struct TokenResult
{
    TokenType type = TokenType::error;
    size_t scan = 0;
    int line;
    int col;
    union
    {
        char liter_ch;                  // 字面字符
        const std::string *str;         // 标识符的指针
        double value_lf;                // 浮点数统一表示
        long long value_ll;             // 有符号数统一表示
        unsigned long long value_ull;   // 无符号数统一表示
    };

    void fprint(FILE *dst) const;
};

class LexAnalysis
{
public:
    LexAnalysis();
    /**
     * @brief 词法分析器
     */
    TokenResult lex_analyse(const char *token, int len);
    static const char* token_type_to_id(TokenType tp);
    TokenResult identifier_dfa(const char *token, int len);
    TokenResult number_dfa(const char *token, int len);
    TokenResult literal_char_dfa(const char *token, int len);
    TokenResult literal_string_dfa(const char *token, int len);
    TokenResult comment_dfa(const char *token, int len);
private:
    TokenResult equal_dfa(const char *token, int len);
    TokenResult nequal_dfa(const char *token, int len);
    TokenResult le_dfa(const char *token, int len);
    TokenResult ge_dfa(const char *token, int len);
    TokenResult logic_and_dfa(const char *token, int len);
    TokenResult logic_or_dfa(const char *token, int len);

    char copy_bf[MAX_LINE_WIDTH];
    IDTab literal_string_tab;
    IDTab id_table;
    const ReservedTab& reserved_table;
};

}
}

#endif