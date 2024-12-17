#include "lex_analysis.h"
#include "dfa.h"
#include <cstring>
#include <cstdio>

using namespace ps::lex;

namespace
{
    const ReservedTab reserved_words {
        {"if", TokenType::kw_if}, {"else", TokenType::kw_else},
        {"while", TokenType::kw_while}, {"do", TokenType::kw_do},
        {"for", TokenType::kw_for}, {"main", TokenType::kw_main},
        {"char", TokenType::kw_char}, {"int", TokenType::kw_int},
        {"long", TokenType::kw_long}, {"float", TokenType::kw_float},
        {"double", TokenType::kw_double}, {"return", TokenType::kw_return},
        {"void", TokenType::kw_void}, {"const", TokenType::kw_const},
        {"continue", TokenType::kw_continue}, {"break", TokenType::kw_break},
        {"unsigned", TokenType::kw_unsigned}, {"enum", TokenType::kw_enum},
        {"switch", TokenType::kw_swtich}, {"case", TokenType::kw_case},
        {"auto", TokenType::kw_auto}, {"static", TokenType::kw_static},
        {"register", TokenType::kw_register}, {"extern", TokenType::kw_extern}
    };
}

#define __is_digit(ch) (ch >= '0' && ch <= '9')
#define __is_char(ch) ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_')
#define __is_symbol(ch) (ch == '-' || ch == '+')
#define __is_nzero(ch) (ch >= '1' && ch <= '9')
#define __is_digit8(ch) (ch >= '0' && ch <= '7')
#define __is_digit16(ch) ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <=' F') || (ch >= 'a' && ch <= 'f'))
#define __is_unsigned(ch) (ch == 'u' || ch == 'U')
#define __is_long(ch) (ch == 'l' || ch == 'L')
#define __is_dot(ch) (ch == '.')
#define __is_zero(ch) (ch == '0')
#define __is_eight_nine(ch) (ch == '8' || ch == '9')
#define __is_sci_notation(ch) (ch == 'e' || ch == 'E')
#define __is_hex(ch) (ch == 'x' || ch == 'X')
#define __is_oct(ch) (ch == 'o' || ch == 'O')
#define __is_quote(ch) (ch == '\'')
#define __is_dequote(ch) (ch == '"')
#define __is_slash(ch) (ch == '/')
#define __is_star(ch) (ch == '*')
#define __no_star(ch) (ch != '*')
#define __no_star_slash(ch) (ch != '/' && ch != '*')
#define __no_ENDL(ch) (ch != '\n')
#define __is_mark_equal(ch) (ch == '=')
#define __is_exclaim(ch) (ch == '!')
#define __is_mark_less(ch) (ch == '<')
#define __is_mark_greater(ch) (ch == '>')
#define __is_mark_bitand(ch) (ch == '&')
#define __is_mark_bitor(ch) (ch == '|')
#define __is_ENDL(ch) (ch == '\n')
#define __is_bslash(ch) (ch == '\\')
#define __char_no_quote_bslash(ch) (ch != '\'' && ch != '\\')
#define __char_no_dequote_bslash(ch) (ch != '"' && ch != '\\')
#define __any_char(ch) (1)
#define __is_number_first(ch) (__is_digit(ch) || __is_dot(ch) || __is_symbol(ch))


void TokenResult::fprint(FILE *dst) const
{
    // 数字要根据类型
    if (type & NUMBER_MASK)
    {
        if (type & FLOAT_MASK)
            fprintf(dst, "(%lf, %s) ", value_lf, LexAnalysis::token_type_to_id(type));
        else
            if (type & UNSIGNED_MASK)
                fprintf(dst, "(%llu, %s) ", value_ull, LexAnalysis::token_type_to_id(type));
            else
                fprintf(dst, "(%lld, %s) ", value_ll, LexAnalysis::token_type_to_id(type));
    } else if (type & ID_MASK || type == TokenType::literal_string)
        fprintf(dst, "(%s, %s) ", str->c_str(), LexAnalysis::token_type_to_id(type));
    else if (type == TokenType::literal_char)
        fprintf(dst, "(%c, %s) ", liter_ch, LexAnalysis::token_type_to_id(type));
    // 错误 & 保留字 & 符号 & 注释 不需要打印
    else fprintf(dst, "(-, %s) ", LexAnalysis::token_type_to_id(type));
}



LexAnalysis::LexAnalysis()
    : reserved_table {reserved_words}
{
}

// 生成标识符的DFA
DFA(__identifier_dfa, 1, TokenType::error,\
    DFA_FINAL_STATE_TABLE(\
        DFA_FINAL_STATE(2, TokenType::identifier)\
    ),\
    DFA_STATE(1, DFA_TRANSFER(__is_char, 2)),\
    DFA_STATE(2, DFA_TRANSFER(__is_char, 2),\
                 DFA_TRANSFER(__is_digit, 2))\
)

TokenResult LexAnalysis::identifier_dfa(const char *token, int len)
{
    auto ret = __identifier_dfa(token, len);
    if (ret.type == TokenType::identifier)
    {
        std::string str {token, ret.scan};
        // 检测是不是保留字
        auto iter = reserved_table.find(str);
        // 是保留字
        if (iter != reserved_table.end())
        {
            ret.str = &iter->first;
            ret.type = iter->second;
        } else
        { // 不是保留字
            auto iter = id_table.find(str);
            if (iter != id_table.end())
                ret.str = &(*(iter));
            else
            {
                auto insert_ret = id_table.insert(str);
                if (insert_ret.second)
                    ret.str = &(*insert_ret.first);
                else
                    ret.str = nullptr;
            }
        }
    }
    return ret;
}

// 生成识别数字的DFA
DFA(__number_dfa, 0, TokenType::error,\
    DFA_FINAL_STATE_TABLE(\
        DFA_FINAL_STATE(2, TokenType::number_INT),    DFA_FINAL_STATE(3, number_U),\
        DFA_FINAL_STATE(4, TokenType::number_UL),     DFA_FINAL_STATE(5, TokenType::number_ULL),\
        DFA_FINAL_STATE(6, TokenType::number_L),      DFA_FINAL_STATE(7, TokenType::number_LL),\
        DFA_FINAL_STATE(8, TokenType::number_zero),\
        DFA_FINAL_STATE(9, TokenType::number_OCT),    DFA_FINAL_STATE(10, TokenType::number_UO),\
        DFA_FINAL_STATE(11, TokenType::number_ULO),   DFA_FINAL_STATE(12, TokenType::number_ULLO),\
        DFA_FINAL_STATE(13, TokenType::number_LO),    DFA_FINAL_STATE(14, TokenType::number_LLO),\
        DFA_FINAL_STATE(16, TokenType::number_DOUBLE),DFA_FINAL_STATE(19, TokenType::number_E_NOTATION),\
        DFA_FINAL_STATE(21, TokenType::number_HEX),   DFA_FINAL_STATE(22, TokenType::number_UH),\
        DFA_FINAL_STATE(23, TokenType::number_ULH),   DFA_FINAL_STATE(24, TokenType::number_ULLH),\
        DFA_FINAL_STATE(25, TokenType::number_LH),    DFA_FINAL_STATE(26, TokenType::number_LLH)\
    ),\
    DFA_STATE(0,    DFA_TRANSFER(__is_nzero, 2),\
                    DFA_TRANSFER(__is_symbol, 1),\
                    DFA_TRANSFER(__is_zero, 8),\
                    DFA_TRANSFER(__is_dot, 16)),\
    DFA_STATE(1,    DFA_TRANSFER(__is_nzero, 2),\
                    DFA_TRANSFER(__is_zero, 8)),\
    DFA_STATE(2,    DFA_TRANSFER(__is_digit, 2),\
                    DFA_TRANSFER(__is_unsigned, 3),\
                    DFA_TRANSFER(__is_long, 6),\
                    DFA_TRANSFER(__is_dot, 16)),\
    DFA_STATE(3,    DFA_TRANSFER(__is_long, 4)),\
    DFA_STATE(4,    DFA_TRANSFER(__is_long, 5)),\
    DFA_STATE(6,    DFA_TRANSFER(__is_long, 7)),\
    DFA_STATE(8,    DFA_TRANSFER(__is_digit8, 9),\
                    DFA_TRANSFER(__is_eight_nine, 15),\
                    DFA_TRANSFER(__is_hex, 20)),\
    DFA_STATE(9,    DFA_TRANSFER(__is_digit8, 9),\
                    DFA_TRANSFER(__is_unsigned, 10),\
                    DFA_TRANSFER(__is_long, 13),\
                    DFA_TRANSFER(__is_eight_nine, 15),\
                    DFA_TRANSFER(__is_dot, 16)),\
    DFA_STATE(10,   DFA_TRANSFER(__is_long, 11)),\
    DFA_STATE(11,   DFA_TRANSFER(__is_long, 12)),\
    DFA_STATE(13,   DFA_TRANSFER(__is_long, 14)),\
    DFA_STATE(15,   DFA_TRANSFER(__is_dot, 16)),\
    DFA_STATE(16,   DFA_TRANSFER(__is_digit, 16),\
                    DFA_TRANSFER(__is_sci_notation, 17)),\
    DFA_STATE(17,   DFA_TRANSFER(__is_symbol, 18),\
                    DFA_TRANSFER(__is_digit, 19)),\
    DFA_STATE(18,   DFA_TRANSFER(__is_digit, 19)),\
    DFA_STATE(19,   DFA_TRANSFER(__is_digit, 19)),\
    DFA_STATE(20,   DFA_TRANSFER(__is_digit, 21)),\
    DFA_STATE(21,   DFA_TRANSFER(__is_digit16, 21),\
                    DFA_TRANSFER(__is_unsigned, 22),\
                    DFA_TRANSFER(__is_long, 25)),\
    DFA_STATE(22,   DFA_TRANSFER(__is_long, 23)),\
    DFA_STATE(23,   DFA_TRANSFER(__is_long, 24)),\
    DFA_STATE(25,   DFA_TRANSFER(__is_long, 26))\
)

TokenResult LexAnalysis::number_dfa(const char *token, int len)
{
    auto ret = __number_dfa(token, len);
    // 调用c的函数来转换，这里耍了个赖：不用词法分析也可以读取
    switch (ret.type)
    {
    case TokenType::error:
        break;
    case TokenType::number_DOUBLE:
    case TokenType::number_E_NOTATION: ret.value_lf = strtod(token, NULL); break;
    case TokenType::number_INT:
    case TokenType::number_L:
    case TokenType::number_LL: ret.value_ll = strtoll(token, NULL, 10); break;
    case TokenType::number_U:
    case TokenType::number_UL:
    case TokenType::number_ULL: ret.value_ull = strtoull(token, NULL, 10); break;
    case TokenType::number_OCT:
    case TokenType::number_LO:
    case TokenType::number_LLO: ret.value_ll = strtoll(token, NULL, 8); break;
    case TokenType::number_UO:
    case TokenType::number_ULO:
    case TokenType::number_ULLO: ret.value_ull = strtoull(token, NULL, 8); break;
    case TokenType::number_HEX:
    case TokenType::number_LH:
    case TokenType::number_LLH: ret.value_ll = strtoll(token, NULL, 16); break;
    case TokenType::number_UH:
    case TokenType::number_ULH:
    case TokenType::number_ULLH: ret.value_ull = strtoull(token, NULL, 16); break;
    case TokenType::number_zero: ret.value_ll = 0; break;
    }
    return ret;
}

// 生成识别字符的dfa
DFA(__literal_char_dfa, 0, TokenType::error,\
    DFA_FINAL_STATE_TABLE(\
        DFA_FINAL_STATE(3, literal_char)\
    ),\
    DFA_STATE(0, DFA_TRANSFER(__is_quote, 1)),\
    DFA_STATE(1, DFA_TRANSFER(__char_no_quote_bslash, 2),\
                 DFA_TRANSFER(__is_bslash, 4)),\
    DFA_STATE(2, DFA_TRANSFER(__is_quote, 3)),\
    DFA_STATE(4, DFA_TRANSFER(__any_char, 2))\
)

TokenResult LexAnalysis::literal_char_dfa(const char *token, int len)
{
    auto ret = __literal_char_dfa(token, len);
    if (ret.type != TokenType::error) // 事实上只有第一个字符才是
        ret.liter_ch = token[1];
    ret.liter_ch = '\0';
    return ret;
}

// 生成识别字符串的dfa
DFA(__literal_string_dfa, 0, TokenType::error,\
    DFA_FINAL_STATE_TABLE(\
        DFA_FINAL_STATE(2, literal_string)\
    ),\
    DFA_STATE(0, DFA_TRANSFER(__is_dequote, 1)),\
    DFA_STATE(1, DFA_TRANSFER(__char_no_dequote_bslash, 1),\
                 DFA_TRANSFER(__is_dequote, 2),\
                 DFA_TRANSFER(__is_bslash, 3)),\
    DFA_STATE(3, DFA_TRANSFER(__any_char, 1))\
)

TokenResult LexAnalysis::literal_string_dfa(const char *token, int len)
{
    auto ret = __literal_string_dfa(token, len);
    if (ret.type != TokenType::error)
    {
        auto iter = literal_string_tab.insert({token, ret.scan});
        ret.str = &(*iter.first);
    } else ret.str = nullptr;
    return ret;
}

// 生成识别注释的dfa
DFA(LexAnalysis::comment_dfa, 0, TokenType::error,\
    DFA_FINAL_STATE_TABLE(\
        DFA_FINAL_STATE(3, comment),\
        DFA_FINAL_STATE(6, comment)\
    ),\
    DFA_STATE(0, DFA_TRANSFER(__is_slash, 1)),\
    DFA_STATE(1, DFA_TRANSFER(__is_slash, 2),\
                 DFA_TRANSFER(__is_star, 4)),\
    DFA_STATE(2, DFA_TRANSFER(__no_ENDL, 2),\
                 DFA_TRANSFER(__is_ENDL, 3)),\
    DFA_STATE(4, DFA_TRANSFER(__no_star, 4),\
                 DFA_TRANSFER(__is_star, 5)),\
    DFA_STATE(5, DFA_TRANSFER(__no_star_slash, 4),\
                 DFA_TRANSFER(__is_star, 5),\
                 DFA_TRANSFER(__is_slash, 6))\
)

/**
 * @brief 各种各样的二元符dfa
 */

DFA(LexAnalysis::equal_dfa, 0, TokenType::error,\
    DFA_FINAL_STATE_TABLE(DFA_FINAL_STATE(2, symbol_equal)),\
    DFA_STATE(0, DFA_TRANSFER(__is_mark_equal, 1)),\
    DFA_STATE(1, DFA_TRANSFER(__is_mark_equal, 2))\
)

DFA(LexAnalysis::nequal_dfa, 0, TokenType::error,\
    DFA_FINAL_STATE_TABLE(DFA_FINAL_STATE(2, symbol_nequal)),\
    DFA_STATE(0, DFA_TRANSFER(__is_exclaim, 1)),\
    DFA_STATE(1, DFA_TRANSFER(__is_mark_equal, 2))\
)

DFA(LexAnalysis::le_dfa, 0, TokenType::error,\
    DFA_FINAL_STATE_TABLE(DFA_FINAL_STATE(2, symbol_le)),\
    DFA_STATE(0, DFA_TRANSFER(__is_mark_less, 1)),\
    DFA_STATE(1, DFA_TRANSFER(__is_mark_equal, 2))\
)

DFA(LexAnalysis::ge_dfa, 0, TokenType::error,\
    DFA_FINAL_STATE_TABLE(DFA_FINAL_STATE(2, symbol_ge)),\
    DFA_STATE(0, DFA_TRANSFER(__is_mark_greater, 1)),\
    DFA_STATE(1, DFA_TRANSFER(__is_mark_equal, 2))\
)

DFA(LexAnalysis::logic_and_dfa, 0, TokenType::error,\
    DFA_FINAL_STATE_TABLE(DFA_FINAL_STATE(2, symbol_logic_and)),\
    DFA_STATE(0, DFA_TRANSFER(__is_mark_bitand, 1)),\
    DFA_STATE(1, DFA_TRANSFER(__is_mark_bitand, 2))\
)

DFA(LexAnalysis::logic_or_dfa, 0, TokenType::error,\
    DFA_FINAL_STATE_TABLE(DFA_FINAL_STATE(2, symbol_logic_or)),\
    DFA_STATE(0, DFA_TRANSFER(__is_mark_bitor, 1)),\
    DFA_STATE(1, DFA_TRANSFER(__is_mark_bitor, 2))\
)

#define simple_case(_ch, _type) case _ch: ret.type = _type; break;
#define dfa_char_case(_ch, func) case _ch: ret = func(token, len); break;
#define dfa_condi_case(_condi, func)\
if (_condi(*token)) ret = func(token, len);
TokenResult LexAnalysis::lex_analyse(const char *token, int len)
{
    // 为了简化代码，先识别第一个字符来确定走哪个dfa
    // 如果识别出来了，就不需要走分支了
    TokenResult ret;
    ret.scan = 1; // 方便直接返回
    ret.type = TokenType::error; // 默认无效
    if (len == 1)
    {
        switch (*token)
        {
            // 匹配特殊字符
            simple_case('+', TokenType::symbol_plus);
            simple_case('-', TokenType::symbol_minus);
            simple_case('*', TokenType::symbol_mul);
            simple_case('/', TokenType::symbol_div);
            simple_case('%', TokenType::symbol_mod);
            simple_case('&', TokenType::symbol_bit_and);
            simple_case('|', TokenType::symbol_bit_or);
            simple_case('~', TokenType::symbol_bit_not);
            simple_case('^', TokenType::symbol_bit_exor);
            simple_case('=', TokenType::symbol_assign);
            simple_case('<', TokenType::symbol_less);
            simple_case('>', TokenType::symbol_greater);
            simple_case('{', TokenType::symbol_lbrace);
            simple_case('}', TokenType::symbol_rbrace);
            simple_case('(', TokenType::symbol_lbracket);
            simple_case(')', TokenType::symbol_rbracket);
            simple_case('!', TokenType::symbol_logic_not);
        }
    }
    if (ret.type == TokenType::error) // 多于一个字符，先匹配第一个
    {
        // 处理标识符的情况，简单分支
        switch (*token)
        {
            dfa_char_case('=', equal_dfa);
            dfa_char_case('!', nequal_dfa);
            dfa_char_case('<', le_dfa);
            dfa_char_case('>', ge_dfa);
            dfa_char_case('&', logic_and_dfa);
            dfa_char_case('|', logic_or_dfa);
            dfa_char_case('\'', literal_char_dfa);
            dfa_char_case('"', literal_string_dfa);
            dfa_char_case('/', comment_dfa);
        }
        // 处理不能靠一个字符识别的情况
        dfa_condi_case(__is_char, identifier_dfa)
        dfa_condi_case(__is_number_first, number_dfa)
    }
    return ret;
}
#undef simple_case
#undef dfa_char_case

const char* LexAnalysis::token_type_to_id(TokenType tp)
{
    switch (tp)
    {
        case TokenType::error: return "ERR";
        case TokenType::identifier: return "ID";
        case TokenType::number_INT: return "INT";
        case TokenType::number_U: return "U";
        case TokenType::number_UL: return "UL";
        case TokenType::number_ULL: return "ULL";
        case TokenType::number_L: return "L";
        case TokenType::number_LL: return "LL";
        case TokenType::number_OCT: return "O";
        case TokenType::number_UO: return "UO";
        case TokenType::number_ULO: return "ULO";
        case TokenType::number_ULLO: return "ULLO";
        case TokenType::number_LO: return "LO";
        case TokenType::number_LLO: return "LLO";
        case TokenType::number_DOUBLE: return "F";
        case TokenType::number_E_NOTATION: return "F";
        case TokenType::number_HEX: return "H";
        case TokenType::number_UH: return "UH";
        case TokenType::number_ULH: return "ULH";
        case TokenType::number_ULLH: return "ULLH";
        case TokenType::number_LH: return "LH";
        case TokenType::number_LLH: return "LLH";
        case TokenType::number_zero: return "ZERO";
        case TokenType::literal_char: return "CHR";
        case TokenType::literal_string: return "STR";
        case TokenType::comment: return "CMT";
        case TokenType::kw_if: return "IF";
        case TokenType::kw_else: return "ELSE";
        case TokenType::kw_while: return "WILE";
        case TokenType::kw_do: return "DO";
        case TokenType::kw_for: return "FOR";
        case TokenType::kw_main: return "MAIN";
        case TokenType::kw_char: return "CHAR";
        case TokenType::kw_int: return "T_D";
        case TokenType::kw_long: return "T_L";
        case TokenType::kw_double: return "T_LF";
        case TokenType::kw_return: return "RETN";
        case TokenType::kw_void: return "VOID";
        case TokenType::kw_const: return "CONS";
        case TokenType::kw_continue: return "CTUN";
        case TokenType::kw_break: return "BRK";
        case TokenType::kw_unsigned: return "T_U";
        case TokenType::kw_enum: return "ENUM";
        case TokenType::kw_swtich: return "SWCH";
        case TokenType::kw_case: return "CASE";
        case TokenType::kw_auto: return "AUTO";
        case TokenType::kw_static: return "STAT";
        case TokenType::kw_register: return "REG";
        case TokenType::kw_extern: return "EXTN";
        case TokenType::symbol_plus: return "PLUS";
        case TokenType::symbol_minus: return "MIN";
        case TokenType::symbol_mul: return "MUL";
        case TokenType::symbol_div: return "DIV";
        case TokenType::symbol_mod: return "MOD";
        case TokenType::symbol_bit_and: return "BAND";
        case TokenType::symbol_bit_or: return "BOR";
        case TokenType::symbol_bit_not: return "BNOT";
        case TokenType::symbol_bit_exor: return "EXOR";
        case TokenType::symbol_assign: return "AIGN";
        case TokenType::symbol_less: return "LS";
        case TokenType::symbol_greater: return "GT";
        case TokenType::symbol_lbrace: return "LB";
        case TokenType::symbol_rbrace: return "RB";
        case TokenType::symbol_colon: return "COLN";
        case TokenType::symbol_lbracket: return "LBR";
        case TokenType::symbol_rbracket: return "RBR";
        case TokenType::symbol_logic_not: return "LNOT";
        case TokenType::symbol_equal: return "EQL";
        case TokenType::symbol_nequal: return "NEQL";
        case TokenType::symbol_logic_and: return "LAND";
        case TokenType::symbol_logic_or: return "LOR";
        case TokenType::symbol_le: return "LE";
        case TokenType::symbol_ge: return "GE";
    };
    return "NONE";
}