/*
 * ============================================================================
 *  C 语言圣经 · 第 4 章 附：main 的参数
 * ============================================================================
 *
 *  main 可以写成：
 *
 *      int main(void)
 *
 *  也可以写成：
 *
 *      int main(int argc, char *argv[])
 *
 *  argc 是参数个数（包含程序名本身），argv 是参数字符串数组。
 *  argv[argc] 保证是空指针。
 * ============================================================================
 */

#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("argc = %d\n", argc);
    printf("argv[argc] == 0: %d\n", argv[argc] == 0);

    for (int i = 1; i < argc; ++i) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    if (argc == 1) {
        printf("no command-line arguments\n");
    }

    return 0;
}
