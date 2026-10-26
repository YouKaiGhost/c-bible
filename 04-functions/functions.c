/*
 * ============================================================================
 *  C 语言圣经 · 第 4 章
 *  函数与作用域
 * ============================================================================
 *
 *  本文件观察：
 *
 *    1. 函数声明、定义与调用
 *    2. 按值传递
 *    3. 返回值与递归
 *    4. 块作用域、文件作用域、静态局部变量
 *    5. 数组参数为什么会“退化”成指针
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic functions.c -o functions
 * ============================================================================
 */

#include <stddef.h>
#include <stdio.h>

int file_scope = 100;
static int file_static = 200;

static int square(int x);
static void try_change(int x);
static unsigned long long factorial(unsigned int n);
static unsigned long long fib(unsigned int n);
static int gcd(int a, int b);
static int next_id(void);
static void scope_demo(void);
static int sum_array(const int a[], size_t n);
static void print_array(const int *a, size_t n);

int main(void)
{
    int value = 7;
    int id1;
    int id2;
    int id3;
    const int values[] = {1, 2, 3, 4, 5};

    printf("== function calls ==\n");
    printf("square(7) = %d\n", square(7));
    printf("gcd(48, 18) = %d\n", gcd(48, 18));

    printf("\n== pass by value ==\n");
    printf("outside before: value = %d\n", value);
    try_change(value);
    printf("outside after: value = %d\n", value);

    printf("\n== recursion ==\n");
    for (unsigned int n = 0; n <= 10; ++n) {
        printf("factorial(%u) = %llu\n", n, factorial(n));
    }

    printf("fib:");
    for (unsigned int n = 0; n <= 10; ++n) {
        printf(" %llu", fib(n));
    }
    printf("\n");

    printf("\n== scope ==\n");
    scope_demo();

    id1 = next_id();
    id2 = next_id();
    id3 = next_id();
    printf("next_id: %d, %d, %d\n", id1, id2, id3);

    printf("\n== array parameters ==\n");
    printf("outside sizeof(values) = %zu\n", sizeof values);
    printf("sum_array = %d\n", sum_array(values, sizeof values / sizeof values[0]));
    print_array(values, sizeof values / sizeof values[0]);

    return 0;
}

static int square(int x)
{
    return x * x;
}

static void try_change(int x)
{
    printf("inside before: x = %d\n", x);
    x = 99;
    printf("inside after: x = %d\n", x);
}

static unsigned long long factorial(unsigned int n)
{
    if (n == 0) {
        return 1;
    }
    return (unsigned long long)n * factorial(n - 1);
}

static unsigned long long fib(unsigned int n)
{
    if (n < 2) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

static int gcd(int a, int b)
{
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

static int next_id(void)
{
    static int id = 0;
    return ++id;
}

static void scope_demo(void)
{
    int x = 1;

    printf("file_scope = %d, file_static = %d\n", file_scope, file_static);
    printf("outer x = %d\n", x);

    {
        int x = 2;
        printf("inner x = %d\n", x);
    }

    printf("outer x after inner block = %d\n", x);
}

static int sum_array(const int a[], size_t n)
{
    int sum = 0;

    for (size_t i = 0; i < n; ++i) {
        sum += a[i];
    }
    return sum;
}

static void print_array(const int *a, size_t n)
{
    /*
     * 参数写成 const int *a 比 const int a[] 更诚实：
     * 数组作为函数参数时，已经退化为指向首元素的指针。
     */
    printf("inside sizeof(a) = %zu\n", sizeof a);
    for (size_t i = 0; i < n; ++i) {
        printf("%d ", a[i]);
    }
    printf("\n");
}
