#include "lex_analysis.h"
#include <stdio.h>
#include <string.h>

using namespace ps::lex;

const int identi_testcase_num = 16;
const char identi_testcase[][32] = {
    // 正确集合
    "Helloworld", "_123", "__123", "a123", "Helloworld", "Helloworld",
    // 保留字
    "main", "void", "for", "continue", "enum",
    // 错误集合
    "不支持中文标识符", "$aaa", "%123", "1abc", "ab.c", "a`", "a-"
};

const int symbol_testcase_num = 25;
const char symbol_testcase[][32] = {
    // 正确集合
    "+", "-", "*", "/", "%", "&", "<", ">", "=", "{", "}", "(", ")", "!", "==",
    "&&", "<=", "~", "|",
    // 错误集合
    "&!", "`", "!@", "=!", "=>", "->"
};

const int number_testcase_num = 42;
const char number_testcase[][32] = {
    // 正确集合
    "1",        "2",        "3",        "4",        "5",        "6",
    "7",        "8",        "9",        "0",        "1.",       "1.1",
    "1.1e1",    "1.1e-1",   "0x1",      "0x1A",     "0x1f",     "0x1e",
    "00",       "07",       "1u",       "1UL",      "1ULL",     "9U",
    "0x16U",    ".1",       ".1e1",
    // 错误集合
    "A",        "B",        "C",        "D",        "E",        "F",
    "G",        "H",        "0x",       "a.1e1",    "1.e",      "0c",
    "08",       "1uu",      "1lu",
};

const int char_testcase_num = 11;
const char char_testcase[][32] = {
    // 正确集合
    "'A'", "'B'", "'C'", "'1'", "';'", "'\\\\'", "'\"'", "'\\''",
    // 错误集合
    "'AAAA'", "'''", "'\\'"
};

const int str_testcase_num = 15;
const char str_testcase[][64] = {
    // 正确集合
    "\"A\"", "\"1\"", "\"Helloworld\"", "\"你好世界\"", "\"UTF-8也可以用\"",
    "\"Ptil0psis & Cooliecrad\"", "\"T0_BE_OR_NOT_T0_BE\"", "\"TH1$_1$_A_QUESTI0N\"",
    "\"123\\456\"", "\"\t\"", "\"!@#$%^&*(())\"", "\"'\""
    // 错误集合
    "\"", "''", "123", "\""
};

const int cmt_testcase_num = 8;
const char cmt_testcase[][64] = {
    // 正确集合
    "// 123\n", "// 你好世界\n", "/* Helloworld */", "/**\n * @brief 注释\n*/",
    // 错误集合
    "/**1/", "/\n/", "// 12345", "/1/"
};

// 单独测试每一个dfa
void separate_test()
{
    LexAnalysis lexa {};
    printf("\n开始测试数字集:\n");
    for (int x=0; x<number_testcase_num; x++)
    {
        const char *ptr = number_testcase[x];
        TokenResult result = lexa.number_dfa(ptr, strlen(ptr));
        printf("%d lexa >> (%s, %s)\n", x, ptr, lexa.token_type_to_id(result.type));
    }

    printf("\n开始测试字符集\n");
    for (int x=0; x<char_testcase_num; x++)
    {
        const char *ptr = char_testcase[x];
        TokenResult result = lexa.literal_char_dfa(ptr, strlen(ptr));
        printf("%d lexa >> (%s, %s)\n", x,  ptr, lexa.token_type_to_id(result.type));
    }

    printf("\n开始测试字符串集\n");
    for (int x=0; x<str_testcase_num; x++)
    {
        const char *ptr = str_testcase[x];
        TokenResult result = lexa.literal_string_dfa(ptr, strlen(ptr));
        printf("%d lexa >> (%s, %s)\n", x,  ptr, lexa.token_type_to_id(result.type));
    }

    printf("\n开始测试注释集（换行符导致打印会比较奇怪）\n");
    for (int x=0; x<cmt_testcase_num; x++)
    {
        const char *ptr = cmt_testcase[x];
        TokenResult result = lexa.comment_dfa(ptr, strlen(ptr));
        printf("%d lexa >> (%s, %s)\n", x,  ptr, lexa.token_type_to_id(result.type));
    }
}

// 测试主dfa
void main_test()
{
    LexAnalysis lexa {};
    printf("\n开始测试标识符和保留字集:\n");
    for (int x=0; x<identi_testcase_num; x++)
    {
        const char *ptr = identi_testcase[x];
        TokenResult result = lexa.lex_analyse(ptr, strlen(ptr));
        printf("%d lexa >> (%s, %s)\n", x, ptr, lexa.token_type_to_id(result.type));
    }

    printf("\n开始测试特殊符号集:\n");
    for (int x=0; x<symbol_testcase_num; x++)
    {
        const char *ptr = symbol_testcase[x];
        TokenResult result = lexa.lex_analyse(ptr, strlen(ptr));
        printf("%d lexa >> (%s, %s)\n", x, ptr, lexa.token_type_to_id(result.type));
    }

    printf("\n开始测试数字集:\n");
    for (int x=0; x<number_testcase_num; x++)
    {
        const char *ptr = number_testcase[x];
        TokenResult result = lexa.lex_analyse(ptr, strlen(ptr));
        printf("%d lexa >> (%s, %s)\n", x, ptr, lexa.token_type_to_id(result.type));
    }

    printf("\n开始测试字符集\n");
    for (int x=0; x<char_testcase_num; x++)
    {
        const char *ptr = char_testcase[x];
        TokenResult result = lexa.lex_analyse(ptr, strlen(ptr));
        printf("%d lexa >> (%s, %s)\n", x,  ptr, lexa.token_type_to_id(result.type));
    }

    printf("\n开始测试字符串集\n");
    for (int x=0; x<str_testcase_num; x++)
    {
        const char *ptr = str_testcase[x];
        TokenResult result = lexa.lex_analyse(ptr, strlen(ptr));
        printf("%d lexa >> (%s, %s)\n", x,  ptr, lexa.token_type_to_id(result.type));
    }

    printf("\n开始测试注释集（换行符导致打印会比较奇怪）\n");
    for (int x=0; x<cmt_testcase_num; x++)
    {
        const char *ptr = cmt_testcase[x];
        TokenResult result = lexa.lex_analyse(ptr, strlen(ptr));
        printf("%d lexa >> (%s, %s)\n", x,  ptr, lexa.token_type_to_id(result.type));
    }
}

int main(void)
{
    // separate_test();
    main_test();
}