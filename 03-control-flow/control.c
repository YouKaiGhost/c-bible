/*
 * ============================================================================
 *  C 语言圣经 · 第 3 章
 *  控制流
 * ============================================================================
 *
 *  本文件观察：
 *
 *    1. if / else if / else
 *    2. while / do-while / for
 *    3. break / continue
 *    4. switch / case / default / fallthrough
 *    5. goto 的两种正当用途
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic control.c -o control
 * ============================================================================
 */

#include <stdio.h>

static void classify(int n);
static void show_if(void);
static void show_loops(void);
static void show_break_continue(void);
static void show_switch(void);
static void show_goto(void);
static const char *day_name(int day);

int main(void)
{
    show_if();
    show_loops();
    show_break_continue();
    show_switch();
    show_goto();
    return 0;
}

static void classify(int n)
{
    if (n > 0) {
        printf("%d is positive\n", n);
    } else if (n < 0) {
        printf("%d is negative\n", n);
    } else {
        printf("%d is zero\n", n);
    }
}

static void show_if(void)
{
    int n = 7;

    printf("== if ==\n");
    classify(-3);
    classify(0);
    classify(7);

    /*
     * 悬空 else：else 总是与最近的、尚未配对的 if 结合。
     *
     * 如果写成：
     *
     *     if (n > 0)
     *         if (n % 2 == 0)
     *             printf("even\n");
     *         else
     *             printf("odd\n");
     *
     * else 属于内层 if，而不是外层。编译器通常会用
     * -Wdangling-else 警告。下面加上外层花括号，让归属一目了然。
     */
    if (n > 0) {
        if (n % 2 == 0) {
            printf("%d is positive even\n", n);
        } else {
            printf("%d is positive odd\n", n);
        }
    }
}

static void show_loops(void)
{
    int sum = 0;
    int i;

    printf("\n== loops ==\n");

    for (int n = 1; n <= 5; ++n) {
        sum += n;
    }
    printf("for: 1 + ... + 5 = %d\n", sum);

    i = 5;
    printf("while countdown:");
    while (i > 0) {
        printf(" %d", i);
        --i;
    }
    printf("\n");

    i = 0;
    do {
        printf("do-while body: i = %d\n", i);
        ++i;
    } while (i < 3);

    /*
     * do-while 至少执行一次；while 在条件一开始就为假时执行零次。
     */
    i = 10;
    while (i < 3) {
        printf("this while body never runs\n");
        ++i;
    }
    do {
        printf("do-while runs once even when the condition is false\n");
    } while (0);

    printf("3x3 table:\n");
    for (int row = 1; row <= 3; ++row) {
        for (int col = 1; col <= 3; ++col) {
            printf("%3d", row * col);
        }
        printf("\n");
    }
}

static void show_break_continue(void)
{
    printf("\n== break and continue ==\n");

    printf("break at 4:");
    for (int i = 0; i < 10; ++i) {
        if (i == 4) {
            break;
        }
        printf(" %d", i);
    }
    printf("\n");

    printf("odd numbers:");
    for (int i = 0; i < 6; ++i) {
        if (i % 2 == 0) {
            continue;
        }
        printf(" %d", i);
    }
    printf("\n");

    printf("nested continue:\n");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (j == 1) {
                continue;
            }
            printf("(%d,%d) ", i, j);
        }
        printf("\n");
    }
}

static void show_switch(void)
{
    printf("\n== switch ==\n");

    for (int i = 0; i <= 5; ++i) {
        printf("switch(%d): ", i);
        switch (i) {
        case 0:
            printf("zero\n");
            break;
        case 1:
        case 2:
            printf("one or two\n");
            break;
        case 3:
            printf("three, ");
            /* fall through */
        case 4:
            printf("three or four\n");
            break;
        default:
            printf("other\n");
            break;
        }
    }

    printf("day_name(1) = %s\n", day_name(1));
    printf("day_name(9) = %s\n", day_name(9));
}

static const char *day_name(int day)
{
    switch (day) {
    case 1:
        return "Monday";
    case 2:
        return "Tuesday";
    case 3:
        return "Wednesday";
    case 4:
        return "Thursday";
    case 5:
        return "Friday";
    case 6:
        return "Saturday";
    case 7:
        return "Sunday";
    default:
        return "invalid";
    }
}

static void show_goto(void)
{
    printf("\n== goto ==\n");

    /*
     * 用途一：从多层循环中一次性退出。
     */
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (i == 1 && j == 1) {
                printf("leaving both loops at (%d,%d)\n", i, j);
                goto done;
            }
            printf("(%d,%d) ", i, j);
        }
    }

done:
    printf("\n");

    /*
     * 用途二：集中清理。这里只演示控制流，不做真实资源管理。
     */
    int ok = 0;
    if (!ok) {
        goto cleanup;
    }
    printf("this line is skipped\n");

cleanup:
    printf("cleanup reached\n");
}
