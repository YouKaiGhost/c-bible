/*
 * ============================================================================
 *  C 语言圣经 · 第 9 章
 *  预处理器
 * ============================================================================
 *
 *  本文件观察：
 *
 *    1. #include 与 include guard
 *    2. 对象宏与函数宏
 *    3. 宏的括号、多次求值陷阱
 *    4. # 字符串化与 ## 记号粘贴
 *    5. 可变参数宏
 *    6. 条件编译、#undef、#error
 *    7. 预定义宏与 #line
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic preprocessor.c -o preprocessor
 * ============================================================================
 */

#include <stdio.h>

#include "config.h"
#include "config.h"   /* 第二次包含会被 include guard 挡住 */

#define VERSION 3
#define SQUARE_BAD(x) x * x
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define STRINGIFY_RAW(x) #x
#define STRINGIFY(x) STRINGIFY_RAW(x)
#define CONCAT_RAW(a, b) a##b
#define CONCAT(a, b) CONCAT_RAW(a, b)

#define LOG(fmt, ...) printf("[LOG] " fmt "\n", __VA_ARGS__)
#define SWAP(type, a, b) do { type tmp = (a); (a) = (b); (b) = tmp; } while (0)

#define TEMP 1
#undef TEMP

int CONCAT(counter, 1) = 10;

int main(void)
{
    int x = 1;
    int y = 2;
    double left = 1.5;
    double right = 2.5;
    int a = 3;
    int b = 4;

    printf("== include and config ==\n");
    printf("APP_NAME = %s\n", APP_NAME);
    printf("CONFIG_MAGIC = %d\n", CONFIG_MAGIC);

    printf("\n== object-like macros ==\n");
    printf("VERSION = %d\n", VERSION);
    printf("STRINGIFY(VERSION) = %s\n", STRINGIFY(VERSION));
    printf("counter1 = %d\n", counter1);

    printf("\n== function-like macros ==\n");
    printf("SQUARE_BAD(1 + 2) = %d\n", SQUARE_BAD(1 + 2));
    printf("SQUARE(1 + 2)     = %d\n", SQUARE(1 + 2));
    printf("MAX(3, 4)         = %d\n", MAX(3, 4));

    LOG("x = %d, y = %d", x, y);
    SWAP(int, x, y);
    printf("after SWAP(int): x = %d, y = %d\n", x, y);
    SWAP(double, left, right);
    printf("after SWAP(double): left = %.1f, right = %.1f\n",
           left, right);

    printf("\n== conditional compilation ==\n");
#if ENABLE_EXTRA
    printf("ENABLE_EXTRA is on\n");
#else
    printf("ENABLE_EXTRA is off\n");
#endif

#ifdef TEMP
    printf("TEMP is defined\n");
#else
    printf("TEMP is undefined\n");
#endif

#if defined(APP_NAME)
    printf("APP_NAME is defined\n");
#endif

#if 0
#error "this line is not compiled"
    printf("this line is not compiled either\n");
#endif

    printf("\n== predefined macros ==\n");
    printf("__STDC__ = %d\n", __STDC__);
    printf("__STDC_HOSTED__ = %d\n", __STDC_HOSTED__);
    printf("__STDC_VERSION__ = %ldL\n", (long)__STDC_VERSION__);
    printf("__func__ = %s\n", __func__);
    printf("__LINE__ = %d\n", __LINE__);
    printf("__FILE__ = %s\n", __FILE__);

#line 500 "virtual.c"
    printf("after #line: __LINE__ = %d, __FILE__ = %s\n", __LINE__, __FILE__);

    /*
     * __DATE__ 和 __TIME__ 也是预定义宏，但它们的值随编译时间变化。
     * 为了避免输出不确定，这里不打印它们。
     */

    printf("\n== macro pitfalls ==\n");
    /*
     * MAX(a++, b++) 会展开成：
     *
     *     ((a++) > (b++) ? (a++) : (b++))
     *
     * 条件里的自增和分支里的自增会执行两次，而且顺序很容易出错。
     * 宏参数可能被多次求值，带副作用的实参要特别小心。
     */
    printf("MAX(a, b) = %d\n", MAX(a, b));

    return 0;
}
