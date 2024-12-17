#include "syntax_analysis.h"

#include <cstdio>

using namespace ps::lex;
using namespace ps::syntax;

namespace
{
    const char *input_dir = {"../test_src/syna_testcase.c"};
    const char *output_dir = {"../test_build/syna_testcase"};
}

int main(void)
{
    FILE *input;
    input = fopen(input_dir, "r");
    SyntaxAnalysis syna {output_dir};
    syna.scan(input);
}