/*
 * ============================================================================
 *  C 语言圣经 · 第 2 章
 *  运算符与表达式
 * ============================================================================
 *
 *  本文件把类型放进表达式里，观察：
 *
 *    1. 算术、关系、逻辑、位运算
 *    2. 优先级与结合性
 *    3. 短路求值
 *    4. 自增自减与赋值
 *    5. 求值顺序和序列点
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic expressions.c -o expressions
 * ============================================================================
 */

#include <limits.h>
#include <stddef.h>
#include <stdio.h>

static void show_arithmetic(void);
static void show_compare_and_logic(void);
static void show_bitwise(void);
static void show_assignment(void);
static void show_precedence(void);
static void show_evaluation(void);
static int trace(const char *label, int value);

int main(void)
{
    show_arithmetic();
    show_compare_and_logic();
    show_bitwise();
    show_assignment();
    show_precedence();
    show_evaluation();
    return 0;
}

static void show_arithmetic(void)
{
    int a = 7;
    int b = 3;
    double d = 3.9;

    printf("== arithmetic ==\n");
    printf("a + b = %d\n", a + b);
    printf("a - b = %d\n", a - b);
    printf("a * b = %d\n", a * b);
    printf("a / b = %d\n", a / b);
    printf("a %% b = %d\n", a % b);
    printf("-7 / 3 = %d\n", -7 / 3);
    printf("-7 %% 3 = %d\n", -7 % 3);
    printf("(double)a / b = %.2f\n", (double)a / b);
    printf("(int)d = %d\n", (int)d);
    printf("(unsigned char)300 = %u\n", (unsigned)(unsigned char)300);
}

static void show_compare_and_logic(void)
{
    int a = 3;
    int b = 5;
    int side_effect = 0;

    printf("\n== compare and logic ==\n");
    printf("a < b  = %d\n", a < b);
    printf("a == b = %d\n", a == b);
    printf("a != b = %d\n", a != b);
    printf("!a = %d, !0 = %d\n", !a, !0);

    printf("2 && 4 = %d\n", 2 && 4);
    printf("2 & 4  = %d\n", 2 & 4);
    printf("2 || 4 = %d\n", 2 || 4);
    printf("2 | 4  = %d\n", 2 | 4);

    printf("short-circuit:\n");
    printf("0 && trace(...) = %d\n",
           trace("left 0", 0) && trace("right 1", 1));
    printf("1 || trace(...) = %d\n",
           trace("left 1", 1) || trace("right 1", 1));
    printf("1 && trace(...) = %d\n",
           trace("left 1", 1) && trace("right 2", 2));
    printf("0 || trace(...) = %d\n",
           trace("left 0", 0) || trace("right 2", 2));

    if ((0 && (side_effect = 1)) || (1 && (side_effect = 2))) {
        printf("side_effect = %d\n", side_effect);
    }
}

static void show_bitwise(void)
{
    unsigned int x = 0xB4u;   /* 二进制 1011 0100 */

    printf("\n== bitwise ==\n");
    printf("x       = 0x%X\n", x);
    printf("~x      = 0x%X\n", ~x);
    printf("x & 0x0F= 0x%X\n", x & 0x0Fu);
    printf("x | 0x0F= 0x%X\n", x | 0x0Fu);
    printf("x ^ 0xFF= 0x%X\n", x ^ 0xFFu);
    printf("x << 2  = 0x%X\n", x << 2);
    printf("x >> 1  = 0x%X\n", x >> 1);
    printf("x & (x-1) = 0x%X\n", x & (x - 1u));
    printf("UINT_MAX  = %u\n", UINT_MAX);
}

static void show_assignment(void)
{
    int n = 10;
    int i = 0;
    int pre;
    int post;
    int comma;
    int maximum;
    const char *parity;

    printf("\n== assignment ==\n");
    n += 5;
    printf("n += 5  : %d\n", n);
    n -= 3;
    printf("n -= 3  : %d\n", n);
    n *= 2;
    printf("n *= 2  : %d\n", n);
    n /= 4;
    printf("n /= 4  : %d\n", n);
    n %= 4;
    printf("n %%= 4  : %d\n", n);

    pre = ++i;
    post = i++;
    printf("pre = %d, post = %d, i = %d\n", pre, post, i);

    comma = (i = 10, i + 1);
    printf("comma: i = %d, result = %d\n", i, comma);

    maximum = (n > 3) ? n : 3;
    parity = (n % 2 == 0) ? "even" : "odd";
    printf("maximum = %d, parity = %s\n", maximum, parity);
}

static void show_precedence(void)
{
    int a = 2 + 3 * 4;
    int b = (2 + 3) * 4;
    int c = 10 - 4 - 3;
    int d = 10 - (4 - 3);
    int e = 1 << (2 + 1);
    int f = (1 < 2) == 1;

    printf("\n== precedence ==\n");
    printf("2 + 3 * 4   = %d\n", a);
    printf("(2 + 3) * 4 = %d\n", b);
    printf("10 - 4 - 3  = %d\n", c);
    printf("10 - (4 - 3)= %d\n", d);
    printf("1 << (2 + 1)= %d\n", e);
    printf("(1 < 2) == 1= %d\n", f);
}

static void show_evaluation(void)
{
    int counter = 0;
    size_t unevaluated = sizeof(counter++);
    int i = 0;
    int comma_result = (i++, i++, i);

    printf("\n== evaluation ==\n");
    printf("sizeof(counter++) = %zu, counter = %d\n",
           unevaluated, counter);
    printf("comma sequence: i = %d, result = %d\n", i, comma_result);

    /*
     * 下面这些写法不要模仿：
     *
     *     i = i++;
     *     i++ + i++;
     *     a[i] = i++;
     *
     * 它们都在两个序列点之间多次修改同一个对象，
     * 属于未定义行为。这里只展示安全的写法。
     */
}

static int trace(const char *label, int value)
{
    printf("  trace %s = %d\n", label, value);
    return value;
}
