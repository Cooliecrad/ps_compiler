#ifndef __PS_DFA_H__
#define __PS_DFA_H__
/**
 * @brief 通过宏头文件来实现状态机
 */

/**
 * @brief 用来表示一个转移动作
 * @param _condi 转移条件
 * @param _state 目标状态
 */
#define DFA_TRANSFER(_condi, _state) if _condi(__ch) __state = _state;

/**
 * @brief 用来表示一个状态
 * @param _state 当前状态的编号，建议使用枚举
 * @param __VA_ARGS__  转移动作
 */
#define DFA_STATE(_state, ...)\
    case _state:\
        __DFA_STMT_EXPAND(__VA_ARGS__)\
        break;

/**
 * @brief 表示一个终止状态需要对应的符号类型
 */
#define DFA_FINAL_STATE(_state, _type) case _state: __final = _type; break;
#define DFA_FINAL_STATE_TABLE(...) __DFA_STMT_EXPAND(__VA_ARGS__)

/**
 * @brief 产生一个状态机函数。函数的参数是要扫描的字符串起点，函数的返回值是标识符的
 * 类型（终止类型）和长度。如果类型为-1，认为扫描出错，此时长度为已经扫描的长度。
 * @param dfa_name 状态机函数名称
 * @param start 初态
 * @param err_handle 错误处理函数。类型 void(*)(const char *token, int scan, int state)
 */

#define DFA(dfa_name, start, error_type, final_table, ...)\
TokenResult dfa_name(const char *token, int len)\
{\
    TokenResult ret;\
    ret.scan = 0;\
    ret.type = error_type;\
    decltype(ret.type) __final = error_type;\
    int __state = start;\
    char __ch = token[0];\
    while (ret.scan < len)\
    {\
        int __transfer = __state;\
        __state = -1;\
        switch (__transfer)\
        {\
            __DFA_STMT_EXPAND(__VA_ARGS__)\
        }\
        ret.scan++;\
        __ch = token[ret.scan];\
        if (__state == -1)\
        {\
            return ret;\
        }\
    }\
    switch (__state)\
    {\
        final_table\
    }\
    ret.type = __final;\
    return ret;\
}

// 用来展开转移动作
#define __DFA_STMT_EXPAND(...) __DFA_STMT_EXPAND_CONCAT(__DFA_COUNT(__VA_ARGS__), __VA_ARGS__)
#define __DFA_STMT_EXPAND_CONCAT(_cnt, ...) __DFA_CONCAT(__DFA_STMT_EXPAND,_cnt)(__VA_ARGS__)
#define __DFA_CONCAT(a, b) a ## b
// 详细的展开动作，需要使用GCC或者启用标准预处理器参数的MSVC
#define __DFA_STMT_EXPAND32(_stmt, ...) _stmt __DFA_STMT_EXPAND31(__VA_ARGS__)
#define __DFA_STMT_EXPAND31(_stmt, ...) _stmt __DFA_STMT_EXPAND30(__VA_ARGS__)
#define __DFA_STMT_EXPAND30(_stmt, ...) _stmt __DFA_STMT_EXPAND29(__VA_ARGS__)
#define __DFA_STMT_EXPAND29(_stmt, ...) _stmt __DFA_STMT_EXPAND28(__VA_ARGS__)
#define __DFA_STMT_EXPAND28(_stmt, ...) _stmt __DFA_STMT_EXPAND27(__VA_ARGS__)
#define __DFA_STMT_EXPAND27(_stmt, ...) _stmt __DFA_STMT_EXPAND26(__VA_ARGS__)
#define __DFA_STMT_EXPAND26(_stmt, ...) _stmt __DFA_STMT_EXPAND25(__VA_ARGS__)
#define __DFA_STMT_EXPAND25(_stmt, ...) _stmt __DFA_STMT_EXPAND24(__VA_ARGS__)
#define __DFA_STMT_EXPAND24(_stmt, ...) _stmt __DFA_STMT_EXPAND23(__VA_ARGS__)
#define __DFA_STMT_EXPAND23(_stmt, ...) _stmt __DFA_STMT_EXPAND22(__VA_ARGS__)
#define __DFA_STMT_EXPAND22(_stmt, ...) _stmt __DFA_STMT_EXPAND21(__VA_ARGS__)
#define __DFA_STMT_EXPAND21(_stmt, ...) _stmt __DFA_STMT_EXPAND20(__VA_ARGS__)
#define __DFA_STMT_EXPAND20(_stmt, ...) _stmt __DFA_STMT_EXPAND19(__VA_ARGS__)
#define __DFA_STMT_EXPAND19(_stmt, ...) _stmt __DFA_STMT_EXPAND18(__VA_ARGS__)
#define __DFA_STMT_EXPAND18(_stmt, ...) _stmt __DFA_STMT_EXPAND17(__VA_ARGS__)
#define __DFA_STMT_EXPAND17(_stmt, ...) _stmt __DFA_STMT_EXPAND16(__VA_ARGS__)
#define __DFA_STMT_EXPAND16(_stmt, ...) _stmt __DFA_STMT_EXPAND15(__VA_ARGS__)
#define __DFA_STMT_EXPAND15(_stmt, ...) _stmt __DFA_STMT_EXPAND14(__VA_ARGS__)
#define __DFA_STMT_EXPAND14(_stmt, ...) _stmt __DFA_STMT_EXPAND13(__VA_ARGS__)
#define __DFA_STMT_EXPAND13(_stmt, ...) _stmt __DFA_STMT_EXPAND12(__VA_ARGS__)
#define __DFA_STMT_EXPAND12(_stmt, ...) _stmt __DFA_STMT_EXPAND10(__VA_ARGS__)
#define __DFA_STMT_EXPAND10(_stmt, ...) _stmt __DFA_STMT_EXPAND9(__VA_ARGS__)
#define __DFA_STMT_EXPAND9(_stmt, ...) _stmt __DFA_STMT_EXPAND8(__VA_ARGS__)
#define __DFA_STMT_EXPAND8(_stmt, ...) _stmt __DFA_STMT_EXPAND7(__VA_ARGS__)
#define __DFA_STMT_EXPAND7(_stmt, ...) _stmt __DFA_STMT_EXPAND6(__VA_ARGS__)
#define __DFA_STMT_EXPAND6(_stmt, ...) _stmt __DFA_STMT_EXPAND5(__VA_ARGS__)
#define __DFA_STMT_EXPAND5(_stmt, ...) _stmt __DFA_STMT_EXPAND4(__VA_ARGS__)
#define __DFA_STMT_EXPAND4(_stmt, ...) _stmt __DFA_STMT_EXPAND3(__VA_ARGS__)
#define __DFA_STMT_EXPAND3(_stmt, ...) _stmt __DFA_STMT_EXPAND2(__VA_ARGS__)
#define __DFA_STMT_EXPAND2(_stmt, ...) _stmt __DFA_STMT_EXPAND1(__VA_ARGS__)
#define __DFA_STMT_EXPAND1(_stmt, ...) _stmt
#define __DFA_STMT_EXPAND0(_stmt, ...)

// 计算参数用
#define __DFA_COUNT(...) __DFA_COUNTER(-1, ## __VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define __DFA_COUNTER(__, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, ...) _31


#endif
