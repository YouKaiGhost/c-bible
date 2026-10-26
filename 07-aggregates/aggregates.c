/*
 * ============================================================================
 *  C 语言圣经 · 第 7 章
 *  结构体、联合体与枚举
 * ============================================================================
 *
 *  本文件观察：
 *
 *    1. struct 的定义、初始化、赋值与成员访问
 *    2. 填充、对齐、sizeof、offsetof、_Alignof
 *    3. 自引用结构体与静态链表
 *    4. typedef
 *    5. union 的共享存储与 tagged union
 *    6. enum 的命名常量
 *    7. 位域
 *    8. 复合字面量
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic aggregates.c -o aggregates
 * ============================================================================
 */

#include <stddef.h>
#include <stdio.h>

struct Point {
    int x;
    int y;
};
typedef struct Point Point;

typedef struct {
    int x;
    int y;
} Pair;

struct Node {
    int value;
    struct Node *next;
};

enum Color {
    RED,
    GREEN = 5,
    BLUE
};

enum Kind {
    KIND_INT,
    KIND_DOUBLE
};

struct Variant {
    enum Kind kind;
    union {
        int i;
        double d;
    } value;
};

union Number {
    int i;
    double d;
};

struct Flags {
    unsigned int ready : 1;
    unsigned int mode : 2;
};

static void print_point(const struct Point *p);
static struct Point make_point(int x, int y);
static void print_list(const struct Node *head);
static const char *color_name(enum Color color);
static void print_variant(const struct Variant *v);
static void print_flags(struct Flags flags);

int main(void)
{
    struct Point p = {1, 2};
    struct Point q = {.y = 4, .x = 3};
    struct Point copy = p;
    Point alias = {5, 6};
    Pair pair = {9, 10};
    struct Point points[3] = {
        {1, 2},
        {3, 4},
        {5, 6}
    };
    struct Node n3 = {30, NULL};
    struct Node n2 = {20, &n3};
    struct Node n1 = {10, &n2};
    enum Color color = GREEN;
    union Number number;
    struct Variant v1 = {.kind = KIND_INT, .value.i = 7};
    struct Variant v2 = {.kind = KIND_DOUBLE, .value.d = 2.5};
    struct Flags flags = {.ready = 1, .mode = 2};
    struct Point literal = (struct Point){.x = 7, .y = 8};
    const struct Point origin = {0, 0};

    printf("== struct basics ==\n");
    print_point(&p);
    print_point(&q);
    printf("copy before: ");
    print_point(&copy);
    copy.x = 99;
    printf("copy after:  ");
    print_point(&copy);
    printf("original:    ");
    print_point(&p);

    printf("\n== typedef ==\n");
    printf("alias: ");
    print_point(&alias);
    printf("pair: %d,%d\n", pair.x, pair.y);

    printf("\n== layout ==\n");
    printf("sizeof(struct Point) = %zu\n", sizeof(struct Point));
    printf("offsetof(struct Point, x) = %zu\n", offsetof(struct Point, x));
    printf("offsetof(struct Point, y) = %zu\n", offsetof(struct Point, y));
    printf("_Alignof(struct Point) = %zu\n", _Alignof(struct Point));
    printf("sizeof(struct Node) = %zu\n", sizeof(struct Node));
    printf("sizeof(struct Flags) = %zu\n", sizeof(struct Flags));
    printf("sizeof(union Number) = %zu\n", sizeof(union Number));
    printf("sizeof(enum Color) = %zu\n", sizeof(enum Color));

    printf("\n== array of structs ==\n");
    for (size_t i = 0; i < sizeof points / sizeof points[0]; ++i) {
        printf("points[%zu]: ", i);
        print_point(&points[i]);
    }

    printf("\n== linked list ==\n");
    print_list(&n1);

    printf("\n== enum ==\n");
    printf("RED = %d, GREEN = %d, BLUE = %d\n",
           (int)RED, (int)GREEN, (int)BLUE);
    printf("color = %d, name = %s\n", (int)color, color_name(color));

    printf("\n== union ==\n");
    number.i = 42;
    printf("number.i = %d\n", number.i);
    number.d = 3.5;
    printf("number.d = %g\n", number.d);
    /* 不要在写入 d 后读取 i；当前活动成员是 d。 */

    printf("\n== tagged union ==\n");
    print_variant(&v1);
    print_variant(&v2);

    printf("\n== bit-fields ==\n");
    print_flags(flags);

    printf("\n== compound literal ==\n");
    print_point(&literal);
    print_point(&origin);

    printf("\n== function returning struct ==\n");
    struct Point made = make_point(11, 12);
    print_point(&made);

    return 0;
}

static void print_point(const struct Point *p)
{
    printf("(%d, %d)\n", p->x, p->y);
}

static struct Point make_point(int x, int y)
{
    return (struct Point){.x = x, .y = y};
}

static void print_list(const struct Node *head)
{
    const struct Node *node = head;

    while (node != NULL) {
        printf("%d", node->value);
        node = node->next;
        if (node != NULL) {
            printf(" -> ");
        }
    }
    printf("\n");
}

static const char *color_name(enum Color color)
{
    switch (color) {
    case RED:
        return "RED";
    case GREEN:
        return "GREEN";
    case BLUE:
        return "BLUE";
    default:
        return "UNKNOWN";
    }
}

static void print_variant(const struct Variant *v)
{
    switch (v->kind) {
    case KIND_INT:
        printf("int: %d\n", v->value.i);
        break;
    case KIND_DOUBLE:
        printf("double: %g\n", v->value.d);
        break;
    default:
        printf("unknown variant\n");
        break;
    }
}

static void print_flags(struct Flags flags)
{
    printf("ready = %u, mode = %u\n",
           (unsigned)flags.ready, (unsigned)flags.mode);
}
