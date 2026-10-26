/*
 * ============================================================================
 *  C 语言圣经 · 第 8 章
 *  存储期、链接与对象模型
 * ============================================================================
 *
 *  本文件与 counter.c 一起编译，观察：
 *
 *    1. 自动存储、静态存储、静态局部变量
 *    2. 内部链接与外部链接
 *    3. static / extern 的不同含义
 *    4. const 在文件作用域的链接属性
 *    5. tentative definition
 *    6. static inline
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic storage.c counter.c -o storage
 * ============================================================================
 */

#include <stdio.h>

#include "counter.h"

int file_scope = 100;            /* 外部链接 */
static int file_static = 200;    /* 内部链接 */
const int external_const = 42;   /* 在 C 里默认是外部链接 */
static const int internal_const = 7;
int tentative;                   /* 暂定定义，零初始化 */

static inline int square(int x)
{
    return x * x;
}

static int next_local(void)
{
    static int n = 0;
    return ++n;
}

int main(void)
{
    int a;
    int b;
    int c;

    printf("== external linkage ==\n");
    printf("file_scope = %d, external_const = %d\n",
           file_scope, external_const);
    printf("read_file_scope() = %d\n", read_file_scope());
    printf("read_external_const() = %d\n", read_external_const());

    printf("shared_counter = %d\n", shared_counter);
    bump();
    bump();
    printf("after two bumps: shared_counter = %d\n", shared_counter);
    printf("read_counter() = %d, internal_bumps = %d\n",
           read_counter(), internal_bump_count());

    printf("\n== internal linkage ==\n");
    printf("file_static = %d, internal_const = %d\n",
           file_static, internal_const);
    printf("square(7) = %d\n", square(7));

    printf("\n== static local ==\n");
    a = next_local();
    b = next_local();
    c = next_local();
    printf("next_local: %d, %d, %d\n", a, b, c);

    printf("\n== tentative definition ==\n");
    printf("tentative = %d\n", tentative);

    return 0;
}
