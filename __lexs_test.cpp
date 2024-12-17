#include "lex_scanner.h"

#include <cstdio>

using namespace ps::lex;

namespace
{
    const char *input_dir = {"../test_src/lexa_testcase.c"};
    const char *output_dir = {"../test_build/lexa_testcase"};
}

int main(void)
{
    FILE *input;
    input = fopen(input_dir, "r");
    LexScanner::vTokenResult results;
    LexScanner lexs {results, output_dir};
    lexs.scan(input);
}