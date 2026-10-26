/*
 * ============================================================================
 *  C 语言圣经 · 第 5 章（下）
 *  指针
 * ============================================================================
 *
 *  本文件观察：
 *
 *    1. 取地址与解引用
 *    2. 指针与数组的关系
 *    3. 指针算术、差值、one-past-the-end
 *    4. NULL 与空指针检查
 *    5. const 指针的三种写法
 *    6. 指针的指针
 *    7. 指针数组与数组指针
 *    8. 函数指针
 *    9. void *
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic pointers.c -o pointers
 * ============================================================================
 */

#include <stddef.h>
#include <stdio.h>

static void swap(int *a, int *b);
static int square(int x);
static int negate(int x);
static int apply(int (*fn)(int), int x);
static void print_array(const int *a, size_t n);

int main(void)
{
    int x = 42;
    int y = 7;
    int *p = &x;
    int **pp = &p;
    int *q = NULL;
    int a[5] = {10, 20, 30, 40, 50};
    int *begin = a;
    int *end = a + 5;
    int *mid = a + 2;
    const int ci = 10;
    const int *pci = &ci;
    int *const cpi = &x;
    const int *const cpci = &ci;
    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };
    int (*row)[3] = matrix;
    int *ptrs[3] = {&a[0], &a[1], &a[2]};
    int (*fn)(int) = square;
    void *vp = &x;
    int *ip = vp;

    printf("== address and dereference ==\n");
    printf("x = %d, *p = %d\n", x, *p);
    *p = 43;
    printf("after *p = 43: x = %d\n", x);
    printf("sizeof x = %zu, sizeof p = %zu\n", sizeof x, sizeof p);

    printf("\n== pointers and arrays ==\n");
    printf("a == &a[0]: %d\n", a == &a[0]);
    printf("sizeof a = %zu, sizeof begin = %zu\n", sizeof a, sizeof begin);
    printf("begin - a = %td, *(begin + 2) = %d, begin[3] = %d\n",
           begin - a, *(begin + 2), begin[3]);
    printf("end - begin = %td, mid - a = %td, end - mid = %td\n",
           end - begin, mid - a, end - mid);

    printf("pointer walk:");
    for (int *it = begin; it != end; ++it) {
        printf(" %d", *it);
    }
    printf("\n");

    printf("\n== null pointer ==\n");
    if (q == NULL) {
        printf("q is NULL; do not dereference it\n");
    }

    printf("\n== swap through pointers ==\n");
    printf("before: x = %d, y = %d\n", x, y);
    swap(&x, &y);
    printf("after:  x = %d, y = %d\n", x, y);

    printf("\n== pointer to pointer ==\n");
    printf("**pp = %d\n", **pp);

    printf("\n== const pointers ==\n");
    printf("ci = %d, *pci = %d\n", ci, *pci);
    printf("*cpi = %d, *cpci = %d\n", *cpi, *cpci);
    /*
     * 下面这些赋值都是编译错误：
     *
     *     *pci = 11;   // pci 指向 const int
     *     cpi = &y;    // cpi 本身是 const 指针
     *     *cpci = 11;  // 两者都 const
     */

    printf("\n== pointer to array vs array of pointers ==\n");
    printf("row[1][2] = %d, sizeof *row = %zu\n",
           row[1][2], sizeof *row);
    printf("ptrs:");
    for (size_t i = 0; i < 3; ++i) {
        printf(" %d", *ptrs[i]);
    }
    printf("\n");

    printf("\n== function pointers ==\n");
    printf("fn(5) = %d\n", fn(5));
    printf("apply(square, 6) = %d\n", apply(square, 6));
    printf("apply(negate, 6) = %d\n", apply(negate, 6));

    printf("\n== void pointer ==\n");
    printf("*ip = %d\n", *ip);
    printf("sizeof(void *) = %zu\n", sizeof(void *));

    printf("\n== array printing ==\n");
    print_array(a, sizeof a / sizeof a[0]);

    return 0;
}

static void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static int square(int x)
{
    return x * x;
}

static int negate(int x)
{
    return -x;
}

static int apply(int (*fn)(int), int x)
{
    return fn(x);
}

static void print_array(const int *a, size_t n)
{
    printf("inside sizeof(a) = %zu: ", sizeof a);
    for (size_t i = 0; i < n; ++i) {
        printf(" %d", a[i]);
    }
    printf("\n");
}
