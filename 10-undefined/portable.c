/*
 * ============================================================================
 *  C 语言圣经 · 第 10 章
 *  未定义行为与可移植性
 * ============================================================================
 *
 *  本文件只演示“安全”的写法：
 *
 *    1. 平台相关的尺寸和范围
 *    2. 无符号回绕 vs 有符号溢出检查
 *    3. _Generic 与 _Static_assert
 *    4. 字节表示与字节序
 *    5. memmove 处理重叠区间
 *
 *  危险的写法只出现在 README 的代码片段里，不要直接运行。
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic portable.c -o portable
 * ============================================================================
 */

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TYPE_NAME(x) _Generic((x), \
    int: "int",                     \
    long: "long",                   \
    double: "double",               \
    default: "other")

static int safe_add(int a, int b, int *out);
static void show_sizes(void);
static void show_arithmetic(void);
static void show_bytes(void);
static void show_overlap(void);

int main(void)
{
    show_sizes();
    show_arithmetic();
    show_bytes();
    show_overlap();
    return 0;
}

static void show_sizes(void)
{
    printf("== sizes and ranges ==\n");
    printf("CHAR_BIT = %d\n", CHAR_BIT);
    printf("sizeof(char) = %zu, sizeof(int) = %zu, sizeof(long) = %zu\n",
           sizeof(char), sizeof(int), sizeof(long));
    printf("sizeof(long long) = %zu, sizeof(void *) = %zu\n",
           sizeof(long long), sizeof(void *));
    printf("sizeof(size_t) = %zu, sizeof(ptrdiff_t) = %zu\n",
           sizeof(size_t), sizeof(ptrdiff_t));
    printf("sizeof(float) = %zu, sizeof(double) = %zu\n",
           sizeof(float), sizeof(double));
    printf("INT_MIN = %d, INT_MAX = %d\n", INT_MIN, INT_MAX);
    printf("LONG_MIN = %ld, LONG_MAX = %ld\n", LONG_MIN, LONG_MAX);
    printf("LLONG_MIN = %lld, LLONG_MAX = %lld\n", LLONG_MIN, LLONG_MAX);
    printf("SIZE_MAX = %zu\n", (size_t)SIZE_MAX);
    printf("INTMAX_MAX = %jd\n", INTMAX_MAX);
    printf("UINTMAX_MAX = %ju\n", UINTMAX_MAX);
    printf("CHAR_MIN = %d, CHAR_MAX = %d\n", CHAR_MIN, CHAR_MAX);
    printf("_Alignof(int) = %zu, _Alignof(double) = %zu\n",
           _Alignof(int), _Alignof(double));

    _Static_assert(sizeof(char) == 1, "sizeof(char) must be 1");
    _Static_assert(CHAR_BIT >= 8, "CHAR_BIT must be at least 8");
}

static void show_arithmetic(void)
{
    unsigned int u = 0;
    int result = 0;

    printf("\n== arithmetic ==\n");
    printf("0u - 1 = %u (unsigned wrap, well defined)\n", u - 1);

    if (safe_add(INT_MAX, 1, &result)) {
        printf("INT_MAX + 1 = %d\n", result);
    } else {
        printf("INT_MAX + 1 would overflow; refused\n");
    }

    if (safe_add(INT_MIN, -1, &result)) {
        printf("INT_MIN - 1 = %d\n", result);
    } else {
        printf("INT_MIN - 1 would overflow; refused\n");
    }

    if (safe_add(20, 22, &result)) {
        printf("20 + 22 = %d\n", result);
    }

    printf("TYPE_NAME(1) = %s\n", TYPE_NAME(1));
    printf("TYPE_NAME(1L) = %s\n", TYPE_NAME(1L));
    printf("TYPE_NAME(1.0) = %s\n", TYPE_NAME(1.0));
    printf("TYPE_NAME(1.0f) = %s\n", TYPE_NAME(1.0f));
}

static void show_bytes(void)
{
    unsigned int value = 0x1234u;
    unsigned char bytes[sizeof value];

    memcpy(bytes, &value, sizeof value);

    printf("\n== object representation ==\n");
    printf("value = 0x%X, sizeof = %zu, bytes:",
           value, sizeof value);
    for (size_t i = 0; i < sizeof bytes; ++i) {
        printf(" %02X", (unsigned)bytes[i]);
    }
    printf("\n");
    printf("The byte order shown above is implementation-defined.\n");
}

static void show_overlap(void)
{
    char buffer[16] = "abcdefghij";

    printf("\n== overlapping copy ==\n");
    printf("before: %s\n", buffer);
    memmove(buffer + 2, buffer, 5);
    printf("after memmove: %s\n", buffer);
    /*
     * memcpy 不保证正确处理重叠区域；重叠时用 memmove。
     * 这里用 memmove 演示，所以行为是定义良好的。
     */
}

static int safe_add(int a, int b, int *out)
{
    if (b > 0 && a > INT_MAX - b) {
        return 0;
    }
    if (b < 0 && a < INT_MIN - b) {
        return 0;
    }
    *out = a + b;
    return 1;
}
