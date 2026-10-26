/*
 * ============================================================================
 *  C 语言圣经 · 第 1 章
 *  对象、类型与声明
 * ============================================================================
 *
 *  本文件回答四个问题：
 *
 *    1. 一个“对象”是什么？
 *    2. 类型给对象加上了什么约束？
 *    3. 声明与定义有什么区别？
 *    4. sizeof 到底测量了什么？
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic types.c -o types
 * ============================================================================
 */

#include <float.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static void show_sizes(void);
static void show_ranges(void);
static void show_literals(void);
static void show_conversions(void);
static void show_declarations(void);

int main(void)
{
    show_sizes();
    show_ranges();
    show_literals();
    show_conversions();
    show_declarations();
    return 0;
}

static void show_sizes(void)
{
    printf("== sizeof ==\n");
    printf("char       : %zu\n", sizeof(char));
    printf("short      : %zu\n", sizeof(short));
    printf("int        : %zu\n", sizeof(int));
    printf("long       : %zu\n", sizeof(long));
    printf("long long  : %zu\n", sizeof(long long));
    printf("float      : %zu\n", sizeof(float));
    printf("double     : %zu\n", sizeof(double));
    printf("long double: %zu\n", sizeof(long double));
    printf("void *     : %zu\n", sizeof(void *));
    printf("size_t     : %zu\n", sizeof(size_t));
    printf("ptrdiff_t  : %zu\n", sizeof(ptrdiff_t));
    printf("CHAR_BIT   : %d\n", CHAR_BIT);
}

static void show_ranges(void)
{
    printf("\n== ranges ==\n");
    printf("signed char       : %d .. %d\n", SCHAR_MIN, SCHAR_MAX);
    printf("unsigned char     : 0 .. %u\n", (unsigned)UCHAR_MAX);
    printf("char              : %d .. %d (%s)\n",
           CHAR_MIN, CHAR_MAX, CHAR_MIN < 0 ? "signed" : "unsigned");
    printf("short             : %d .. %d\n", SHRT_MIN, SHRT_MAX);
    printf("unsigned short    : 0 .. %u\n", (unsigned)USHRT_MAX);
    printf("int               : %d .. %d\n", INT_MIN, INT_MAX);
    printf("unsigned int      : 0 .. %u\n", UINT_MAX);
    printf("long              : %ld .. %ld\n", LONG_MIN, LONG_MAX);
    printf("unsigned long     : 0 .. %lu\n", ULONG_MAX);
    printf("long long         : %lld .. %lld\n", LLONG_MIN, LLONG_MAX);
    printf("unsigned long long: 0 .. %llu\n", ULLONG_MAX);
    printf("size_t            : 0 .. %zu\n", (size_t)SIZE_MAX);
    printf("intmax_t          : %jd .. %jd\n", INTMAX_MIN, INTMAX_MAX);
    printf("uintmax_t         : 0 .. %ju\n", UINTMAX_MAX);
    printf("FLT_DIG           : %d\n", FLT_DIG);
    printf("DBL_DIG           : %d\n", DBL_DIG);
    printf("FLT_EPSILON       : %e\n", (double)FLT_EPSILON);
    printf("DBL_EPSILON       : %e\n", DBL_EPSILON);
    printf("LDBL_EPSILON      : %Le\n", LDBL_EPSILON);
}

static void show_literals(void)
{
    printf("\n== literals ==\n");
    printf("42                 : %d\n", 42);
    printf("052 (octal)        : %d\n", 052);
    printf("0x2A (hex)         : %d\n", 0x2A);
    printf("42u                : %u\n", 42u);
    printf("42L                : %ld\n", 42L);
    printf("42ULL              : %llu\n", 42ULL);
    printf("'A'                : %d\n", 'A');
    printf("sizeof('A')        : %zu\n", sizeof('A'));
    printf("1e3                : %.1f\n", 1e3);
    printf("0.5f               : %.1f\n", 0.5f);
}

static void show_conversions(void)
{
    unsigned int u = 0;
    int negative = -1;
    unsigned int converted = (unsigned int)negative;

    printf("\n== conversions ==\n");
    printf("0u - 1             : %u\n", u - 1);
    printf("(unsigned)-1       : %u\n", converted);
    printf("converted < 1u     : %d\n", converted < 1u);
    printf("(double)7 / 2      : %.1f\n", (double)7 / 2);
    printf("7 / 2              : %d\n", 7 / 2);
    printf("7 %% 2              : %d\n", 7 % 2);
    printf("-7 / 3             : %d\n", -7 / 3);
    printf("-7 %% 3             : %d\n", -7 % 3);
}

static void show_declarations(void)
{
    int x = 10;
    int y = x;
    const int answer = 42;
    const int *p = &answer;
    int a[4] = {1, 2, 3, 4};
    int counter = 0;
    size_t unevaluated = sizeof(counter++);

    printf("\n== declarations ==\n");
    x = 20;
    printf("x = %d, y = %d\n", x, y);
    printf("answer = %d, *p = %d\n", answer, *p);
    printf("sizeof a = %zu, elements = %zu\n",
           sizeof a, sizeof a / sizeof a[0]);
    printf("sizeof(counter++) = %zu, counter = %d\n",
           unevaluated, counter);

    _Static_assert(sizeof(char) == 1, "sizeof(char) is always 1");
    _Static_assert(sizeof(int) >= 2, "int holds at least 16 bits");
    printf("static assertions passed\n");
}
