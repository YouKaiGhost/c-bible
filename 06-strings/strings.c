/*
 * ============================================================================
 *  C 语言圣经 · 第 6 章
 *  字符串与字符
 * ============================================================================
 *
 *  本文件观察：
 *
 *    1. char、字符常量与转义序列
 *    2. 字符串字面量的内部表示
 *    3. char 数组与 char 指针的区别
 *    4. 自己实现 strlen / copy / compare
 *    5. 缓冲区容量与截断
 *    6. UTF-8 下“字节数”和“字符数”的差别
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic strings.c -o strings
 * ============================================================================
 */

#include <stddef.h>
#include <stdio.h>

static size_t string_length(const char *s);
static void copy_string(char *dst, size_t cap, const char *src);
static int compare_strings(const char *a, const char *b);
static void print_bytes(const char *s);

int main(void)
{
    char greeting[] = "Hello";
    const char *literal = "Hello";
    char buffer[8];
    char exact[5] = "Hello";
    char bytes[] = {'H', 'i', '\0'};
    const char *names[] = {"Alice", "Bob", "Carol"};
    const char *utf8 = "你好";

    printf("== char ==\n");
    printf("'A' = %d, sizeof('A') = %zu, sizeof(int) = %zu\n",
           'A', sizeof('A'), sizeof(int));
    printf("hex '\\x41' = %c, octal '\\101' = %c\n", '\x41', '\101');
    printf("backslash = \\, quote = \", tab = |\t|\n");
    printf("NUL = %d\n", '\0');

    printf("\n== string literal ==\n");
    printf("sizeof(\"Hello\") = %zu\n", sizeof("Hello"));
    printf("sizeof(greeting) = %zu, length = %zu\n",
           sizeof greeting, string_length(greeting));
    printf("sizeof(literal) = %zu (pointer), length = %zu\n",
           sizeof literal, string_length(literal));
    printf("sizeof(bytes) = %zu, bytes = %s\n", sizeof bytes, bytes);
    printf("concatenation: %s\n", "Hello, " "World!");

    printf("\n== mutable copy vs literal ==\n");
    greeting[0] = 'J';
    printf("modified greeting: %s\n", greeting);
    printf("literal still: %s\n", literal);
    /*
     * 不要这样做：
     *
     *     char *p = "Hello";
     *     p[0] = 'J';       // 未定义行为：修改字符串字面量
     *
     * 字符串字面量不保证可修改。需要修改时，复制到 char 数组。
     */

    printf("\n== copying and truncation ==\n");
    copy_string(buffer, sizeof buffer, "Hi");
    printf("copy 'Hi': %s\n", buffer);

    copy_string(buffer, 4, "Hello");
    printf("copy with cap 4: %s\n", buffer);

    printf("exact[5] = \"Hello\" has sizeof = %zu; it has no terminating NUL\n",
           sizeof exact);

    printf("\n== comparison ==\n");
    printf("compare(apple, banana) = %d\n", compare_strings("apple", "banana"));
    printf("compare(apple, apple)  = %d\n", compare_strings("apple", "apple"));
    printf("compare(banana, apple) = %d\n", compare_strings("banana", "apple"));

    printf("\n== array of strings ==\n");
    printf("names count = %zu\n", sizeof names / sizeof names[0]);
    for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i) {
        printf("names[%zu] = %s\n", i, names[i]);
    }

    printf("\n== UTF-8 is bytes ==\n");
    printf("sizeof(\"你好\") = %zu, string_length = %zu\n",
           sizeof("你好"), string_length(utf8));
    printf("utf8 = %s\n", utf8);
    printf("bytes:");
    print_bytes(utf8);

    return 0;
}

static size_t string_length(const char *s)
{
    size_t n = 0;

    while (s[n] != '\0') {
        ++n;
    }
    return n;
}

static void copy_string(char *dst, size_t cap, const char *src)
{
    size_t i = 0;

    if (cap == 0) {
        return;
    }

    while (i + 1 < cap && src[i] != '\0') {
        dst[i] = src[i];
        ++i;
    }
    dst[i] = '\0';
}

static int compare_strings(const char *a, const char *b)
{
    const unsigned char *pa = (const unsigned char *)a;
    const unsigned char *pb = (const unsigned char *)b;

    while (*pa != '\0' && *pa == *pb) {
        ++pa;
        ++pb;
    }
    return (int)*pa - (int)*pb;
}

static void print_bytes(const char *s)
{
    for (size_t i = 0; s[i] != '\0'; ++i) {
        printf(" %02X", (unsigned int)(unsigned char)s[i]);
    }
    printf("\n");
}
