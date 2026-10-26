/*
 * ============================================================================
 *  C 语言圣经 · 第 5 章（上）
 *  数组
 * ============================================================================
 *
 *  本文件观察：
 *
 *    1. 数组的定义、初始化、指定初始化
 *    2. sizeof 与元素个数
 *    3. 下标、边界与越界
 *    4. 数组不能整体赋值
 *    5. 二维数组的行优先布局
 *    6. 数组作为函数参数时的退化
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic arrays.c -o arrays
 * ============================================================================
 */

#include <stddef.h>
#include <stdio.h>

static int sum_array(const int a[], size_t n);
static void print_array(const int *a, size_t n);
static void print_matrix(const int m[2][3]);
static void safe_get(const int a[], size_t n, size_t index);

int main(void)
{
    int a[5] = {10, 20, 30, 40, 50};
    int partial[5] = {1, 2, 3};
    int designated[6] = {[1] = 20, [4] = 50};
    int copy[5];
    const int primes[] = {2, 3, 5, 7, 11};
    const int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };
    size_t n = sizeof a / sizeof a[0];

    printf("== array basics ==\n");
    printf("sizeof a = %zu, elements = %zu\n", sizeof a, n);
    printf("a[0] = %d, a[4] = %d\n", a[0], a[4]);
    printf("a == &a[0]: %d\n", a == &a[0]);

    printf("partial:");
    for (size_t i = 0; i < sizeof partial / sizeof partial[0]; ++i) {
        printf(" %d", partial[i]);
    }
    printf("\n");

    printf("designated:");
    for (size_t i = 0; i < sizeof designated / sizeof designated[0]; ++i) {
        printf(" %d", designated[i]);
    }
    printf("\n");

    printf("primes:");
    for (size_t i = 0; i < sizeof primes / sizeof primes[0]; ++i) {
        printf(" %d", primes[i]);
    }
    printf("\n");

    /*
     * 数组不能整体赋值。下面这行无法编译：
     *
     *     copy = a;
     *
     * 必须逐元素复制，或者用 memcpy（标准库函数）。
     */
    for (size_t i = 0; i < n; ++i) {
        copy[i] = a[i];
    }
    printf("copy:");
    print_array(copy, n);

    printf("sum_array = %d\n", sum_array(a, n));

    printf("\n== array parameters ==\n");
    printf("outside sizeof(a) = %zu\n", sizeof a);
    print_array(a, n);

    printf("\n== bounds ==\n");
    safe_get(a, n, 0);
    safe_get(a, n, 4);
    safe_get(a, n, 5);

    printf("\n== 2D array ==\n");
    printf("sizeof matrix = %zu\n", sizeof matrix);
    printf("sizeof matrix[0] = %zu\n", sizeof matrix[0]);
    printf("sizeof matrix[0][0] = %zu\n", sizeof matrix[0][0]);
    print_matrix(matrix);

    return 0;
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
    printf("inside sizeof(a) = %zu: ", sizeof a);
    for (size_t i = 0; i < n; ++i) {
        printf(" %d", a[i]);
    }
    printf("\n");
}

static void print_matrix(const int m[2][3])
{
    for (size_t row = 0; row < 2; ++row) {
        for (size_t col = 0; col < 3; ++col) {
            printf("%3d", m[row][col]);
        }
        printf("\n");
    }
}

static void safe_get(const int a[], size_t n, size_t index)
{
    if (index >= n) {
        printf("index %zu is out of range [0, %zu)\n", index, n);
        return;
    }
    printf("a[%zu] = %d\n", index, a[index]);
}
