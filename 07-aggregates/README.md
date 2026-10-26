# 第 7 章 · 结构体、联合体与枚举

> **卷一 · 语言**　|　前置知识：第 6 章　|　建议用时：50 分钟

> 聚合类型把分散的对象组织成一个整体；布局规则则提醒你，这个整体在内存里并不总是“成员之和”。

前面几章的对象都是单个值：一个整数、一个指针、一个字符。现实里的数据通常有结构：一个点有 `x` 和 `y`，一个学生有姓名和成绩，一个网络包有类型和负载。

这一章讲 C 的三种聚合类型：

- `struct`：把多个成员放在一起，每个成员有独立存储；
- `union`：多个成员共享同一块存储；
- `enum`：给一组整数常量起名字。

它们都是语言层面的构造，不依赖标准库。

## 7.1 本章语法地图

| 语法 | 本章出现的位置 |
|------|----------------|
| `struct` 定义 | `struct Point { int x; int y; };` |
| 成员访问 | `p.x`、`p->x` |
| 结构体初始化 | `struct Point p = {1, 2};` |
| 指定初始化 | `struct Point p = {.y = 4, .x = 3};` |
| 结构体赋值 | `q = p;` |
| 结构体数组 | `struct Point points[3];` |
| 自引用结构体 | `struct Node { struct Node *next; };` |
| `typedef` | `typedef struct Point Point;` |
| `union` | `union Number { int i; double d; };` |
| tagged union | `enum Kind` + `union` |
| `enum` | `enum Color { RED, GREEN, BLUE };` |
| 位域 | `unsigned int ready : 1;` |
| 布局查询 | `sizeof`、`offsetof`、`_Alignof` |
| 复合字面量 | `(struct Point){.x = 1, .y = 2}` |

## 7.2 为什么需要 `struct`

假设要表示一个二维点：

```c
int x = 1;
int y = 2;
```

两个变量是散的。函数要接收一个点，就得传两个参数；数组要存很多点，就得用两个平行数组。`struct` 把它们绑成一个对象：

```c
struct Point {
    int x;
    int y;
};

struct Point p = {1, 2};
```

现在 `p` 是一个对象，类型是 `struct Point`，里面有两个成员。成员用 `.` 访问：

```c
p.x = 10;
p.y = 20;
```

`struct` 定义只是创建了一个**类型**，不分配存储。真正分配存储的是定义变量：

```c
struct Point p;          // 分配一个 struct Point 对象
struct Point *ptr;       // 分配一个指针对象，指向 struct Point
```

## 7.3 结构体的声明、定义与初始化

### 标签与变量

```c
struct Point {    // struct Point 是标签（tag）
    int x;
    int y;
};

struct Point p;   // 用标签声明变量
```

`struct Point` 是一个类型名。`Point` 本身不是类型名，除非你用 `typedef` 起别名（见 7.7）。

你也可以在定义类型的同时定义变量：

```c
struct Point {
    int x;
    int y;
} p1, p2;
```

但通常把类型定义和变量定义分开，代码更清楚。

### 初始化

```c
struct Point p = {1, 2};                 // x = 1, y = 2
struct Point q = {.y = 4, .x = 3};       // 指定初始化
struct Point r = {.x = 5};               // y 自动补 0
```

规则和数组类似：

- 按成员声明顺序初始化；
- 初始化器可以少于成员数，剩余成员补 0；
- C99 起可以用 `.成员名 = 值` 指定初始化；
- 指定初始化可以乱序，但每个成员只应初始化一次。

嵌套结构体也可以嵌套初始化：

```c
struct Segment {
    struct Point start;
    struct Point end;
};

struct Segment s = {
    .start = {.x = 0, .y = 0},
    .end = {.x = 10, .y = 20}
};
```

## 7.4 赋值、比较与复制

### 结构体可以整体赋值

```c
struct Point p = {1, 2};
struct Point q;

q = p;        // 复制整个结构体的值
q.x = 99;     // 只修改 q
```

`q = p` 是**值复制**，不是指针别名。`q` 和 `p` 是两个独立对象。结构体里的数组成员也会被复制。

### 结构体不能用 `==` 比较

C 没有为结构体定义 `==`：

```c
if (p == q) { ... }   // 错误
```

需要逐成员比较：

```c
if (p.x == q.x && p.y == q.y) {
    ...
}
```

不要用 `memcmp` 比较结构体，除非你非常清楚填充字节的规则。结构体里的 padding 内容是不确定的，两个成员完全相同的结构体，`memcmp` 也可能不相等。

### 按值传递与返回

结构体可以作为参数和返回值，语义都是复制：

```c
void print_point(struct Point p);          // 复制一份
struct Point make_point(int x, int y);     // 返回一个值
```

大结构体按值传递会有复制成本。如果函数只需要读，传 `const struct Point *`；如果需要修改调用者的对象，传 `struct Point *`。

## 7.5 指向结构体的指针

```c
struct Point p = {1, 2};
struct Point *ptr = &p;

(*ptr).x    // 1
ptr->x      // 1，语法糖
```

`->` 是 `(*ptr).member` 的简写。它的左边必须是指向结构体的指针。

```c
ptr->x = 10;   // 修改 p.x
```

结构体指针在链表、树、动态数据结构和函数接口里非常常见。第 5 章的指针规则全部适用：空指针检查、悬空指针、`const` 指针。

```c
void print_point(const struct Point *p)
{
    printf("(%d, %d)\n", p->x, p->y);
}
```

`const struct Point *` 表示不能通过这个指针修改成员，但指针本身可以改指向。

## 7.6 布局：填充与对齐

结构体的 `sizeof` 不一定等于成员大小之和。编译器会在成员之间和结构体末尾插入**填充字节**（padding），让每个成员满足对齐要求。

```c
struct A {
    char c;
    int i;
};
```

在常见平台上：

```text
偏移 0: c
偏移 1-3: padding
偏移 4-7: i
sizeof(struct A) == 8
```

`char` 只占 1 字节，但 `int` 通常要求 4 字节对齐，所以 `c` 后面要空出 3 字节。

成员顺序会影响填充：

```c
struct B {
    char a;
    int b;
    char c;
};   // 常见大小：12

struct C {
    int b;
    char a;
    char c;
};   // 常见大小：8
```

同样的成员，换一个顺序，大小可能不同。这是布局优化的一个基本手段，但不要为了省几个字节牺牲可读性，除非你确实需要。

### `sizeof`、`offsetof` 与 `_Alignof`

- `sizeof(struct T)`：整个结构体占多少字节，包括尾部填充。
- `offsetof(struct T, member)`：成员相对于结构体起始地址的字节偏移，结果是 `size_t`。
- `_Alignof(struct T)`：结构体的对齐要求，结果是 `size_t`（C11）。

```c
printf("%zu\n", sizeof(struct Point));
printf("%zu\n", offsetof(struct Point, y));
printf("%zu\n", _Alignof(struct Point));
```

`offsetof` 定义在 `<stddef.h>`。它不能用于位域成员。

### 不要直接序列化结构体

把结构体的字节直接写入文件或网络，是不可移植的：

- padding 的内容不确定；
- 不同平台的对齐和大小可能不同；
- 字节序可能不同；
- 位域布局由实现定义。

需要序列化时，逐字段写入，或者使用明确的字节协议。

## 7.7 `typedef`：给类型起别名

`typedef` 不创建新类型，只创建别名：

```c
typedef struct Point Point;

Point p = {1, 2};
```

现在 `Point` 和 `struct Point` 是同一个类型。常见写法是合并：

```c
typedef struct Point {
    int x;
    int y;
} Point;
```

也可以给匿名结构体起别名：

```c
typedef struct {
    int x;
    int y;
} Pair;
```

这样就没有 `struct Pair` 这个标签，只有 `Pair` 这个 typedef 名。

标签和 typedef 名在不同的命名空间里，可以同名：

```c
struct Point { int x, y; };
typedef struct Point Point;
```

`typedef` 常用于简化复杂声明，例如函数指针：

```c
typedef int (*IntFn)(int);
IntFn fp = square;
```

它不会改变类型兼容性，只是让名字更好读。

## 7.8 自引用结构体与链表

结构体可以包含指向自身类型的指针，但不能包含自身类型的对象：

```c
struct Node {
    int value;
    struct Node *next;   // 合法：指针的大小是固定的
};
```

不能写：

```c
struct Node {
    int value;
    struct Node next;    // 错误：类型还不完整，且会无限递归
};
```

用这个定义可以构造链表：

```c
struct Node n3 = {30, NULL};
struct Node n2 = {20, &n3};
struct Node n1 = {10, &n2};
```

遍历：

```c
for (const struct Node *p = &n1; p != NULL; p = p->next) {
    printf("%d\n", p->value);
}
```

这里的节点是静态分配的，生命期到程序结束。动态分配节点需要 `malloc`/`free`，属于标准库和对象生命期的话题，第 8 章会提到。

## 7.9 `union`：共享存储

`union` 的成员共享同一块存储：

```c
union Number {
    int i;
    double d;
};
```

`sizeof(union Number)` 至少能容纳最大的成员，并满足最严格的对齐要求。常见平台上 `double` 是 8 字节，所以 union 也是 8 字节。

同一时刻只有一个成员是“活动”的：

```c
union Number n;
n.i = 42;        // 活动成员是 i
printf("%d\n", n.i);

n.d = 3.5;       // 现在活动成员是 d
printf("%g\n", n.d);
```

写入一个成员、读取另一个成员，在 C17 里通常是**实现定义**的行为（有一个“公共初始序列”的例外）。不要依赖它做类型双关。需要按字节重新解释对象时，用 `memcpy` 到另一个对象，语义更清楚。

`union` 的常见用途：

- 节省存储：同一时刻只存一种类型；
- 带标签的联合（tagged union）：用一个 `enum` 记录当前活动成员；
- 访问对象的字节表示（配合 `unsigned char`，并注意字节序）。

## 7.10 Tagged union：带标签的联合

单独一个 `union` 不知道自己当前存的是什么。常见模式是加一个标签：

```c
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
```

使用：

```c
struct Variant v = {
    .kind = KIND_INT,
    .value.i = 7
};
```

读取前先看 `kind`：

```c
switch (v.kind) {
case KIND_INT:
    printf("%d\n", v.value.i);
    break;
case KIND_DOUBLE:
    printf("%g\n", v.value.d);
    break;
}
```

这就是一个最小的“动态类型”值。标签和 union 必须同步维护；写错标签，读到的就是错误的活动成员。

## 7.11 `enum`：命名常量

`enum` 给一组整数常量起名字：

```c
enum Color {
    RED,        // 0
    GREEN = 5,  // 5
    BLUE        // 6
};
```

规则：

- 第一个枚举值默认是 0；后续值默认是前一个值加 1。
- 可以显式赋值；后续值从它继续递增。
- 枚举常量（enumerator）的类型是 `int`，值是整数常量表达式。
- 枚举类型本身会映射到某个整数类型；具体是 `char`、有符号整数还是无符号整数，由实现选择，只要它能表示所有枚举值。
- 因此 `sizeof(enum Color)` 由实现决定，不一定是 `sizeof(int)`（虽然常见是 4）。
- 枚举常量是普通标识符，作用域是它所在的作用域：`RED` 不是 `Color.RED`。

`enum` 很适合配合 `switch`：

```c
const char *color_name(enum Color c)
{
    switch (c) {
    case RED:   return "RED";
    case GREEN: return "GREEN";
    case BLUE:  return "BLUE";
    default:    return "UNKNOWN";
    }
}
```

### `enum` 与 `#define`

```c
enum Color { RED, GREEN, BLUE };
#define RED 0
```

`enum` 有类型、有作用域，调试器能看到名字；宏是文本替换，没有类型，也没有作用域。能用 `enum` 时，优先用 `enum`。

枚举值可以重复：

```c
enum E { A = 1, B = 1 };
```

但同一个 `switch` 里不能有两个相同的 `case` 值，否则编译错误。

## 7.12 位域

位域把整数成员压缩到指定的位数：

```c
struct Flags {
    unsigned int ready : 1;
    unsigned int mode  : 2;
};
```

- `ready` 占 1 位，取值 0 或 1；
- `mode` 占 2 位，取值 0 到 3；
- 成员的类型通常是 `unsigned int`、`signed int` 或 `_Bool`；
- 位域的布局、顺序、是否跨存储单元，由实现定义；
- 不能对位域取地址；
- `offsetof` 不能用于位域；
- `sizeof(struct Flags)` 包括底层存储单元，可能是 4 字节。

位域适合在内存紧张时压缩标志，**不适合**直接映射网络协议或文件格式，因为布局不可移植。需要明确的二进制布局时，用手工移位和掩码。

## 7.13 复合字面量

复合字面量是 C99 的特性：在表达式里直接构造一个匿名对象。

```c
struct Point p = (struct Point){.x = 1, .y = 2};
```

它也可以直接取地址：

```c
print_point(&(struct Point){3, 4});
```

复合字面量是左值，有存储期：

- 在函数内部，它的存储期与所在块相同；
- 在函数外部，它具有静态存储期。

不要返回指向局部复合字面量的指针：

```c
struct Point *bad(void)
{
    return &(struct Point){1, 2};   // 返回后对象消失，悬空
}
```

按值返回结构体是安全的，因为返回的是副本：

```c
struct Point make_point(int x, int y)
{
    return (struct Point){.x = x, .y = y};
}
```

## 7.14 其他聚合特性

### 柔性数组成员

C99 允许结构体的最后一个成员是“不完整数组”：

```c
struct Buffer {
    size_t len;
    unsigned char data[];
};
```

`sizeof(struct Buffer)` 不包含 `data` 的实际内容；`data` 只是占位，实际存储要在动态分配时一起申请。不能给 `data` 指定初始化器，也不能单独定义一个柔性数组成员。

### 匿名结构体/联合体

C11 允许匿名成员：

```c
struct Outer {
    int tag;
    union {
        int i;
        double d;
    };
};
```

匿名 union 的成员可以直接用 `outer.i` 访问，不需要中间名字。它适合 tagged union，但会降低可读性，使用前要权衡。

## 7.15 代码导览

[`aggregates.c`](aggregates.c) 分成几段：

1. `struct Point`：成员访问、初始化、赋值、指针；
2. `typedef` 和匿名结构体 `Pair`；
3. 布局：`sizeof`、`offsetof`、`_Alignof`；
4. 结构体数组；
5. 自引用 `struct Node` 和静态链表；
6. `enum Color` 与 `switch`；
7. `union Number` 和 tagged union；
8. 位域；
9. 复合字面量和按值返回结构体。

编译运行：

```sh
cc -std=c17 -Wall -Wextra -Wpedantic aggregates.c -o aggregates
./aggregates
```

在这台机器上重点观察：

- `sizeof(struct Point)` 是 8，`offsetof(struct Point, y)` 是 4；
- `sizeof(struct Node)` 是 16，因为 `int` 和指针之间有填充；
- `sizeof(union Number)` 是 8，至少能容纳 `double`；
- `sizeof(enum Color)` 是 4（实现定义）；
- 位域的值可以像普通整数一样读写，但布局不可移植。

## 7.16 常见陷阱

| 陷阱 | 说明 |
|------|------|
| 以为 `sizeof(struct)` 等于成员大小之和 | 有填充；成员顺序会影响大小。 |
| 用 `memcmp` 比较结构体 | padding 内容不确定，可能误判。 |
| 用 `==` 比较结构体 | C 不提供结构体相等运算符。 |
| 读取 union 的非活动成员 | C17 通常是实现定义；不要依赖类型双关。 |
| 以为 `enum` 一定是 `int` 大小 | 底层类型由实现选择；`sizeof` 不保证。 |
| 枚举常量当成员访问 | 枚举常量是普通标识符，不是 `Color.RED`。 |
| 对位域取地址 | 位域没有独立地址；`offsetof` 也不能用。 |
| 直接序列化结构体 | padding、对齐、字节序、位域布局都不可移植。 |
| 返回局部复合字面量的地址 | 对象在块结束时消失，指针悬空。 |
| 自引用结构体包含自身对象 | 类型不完整且会无限递归；只能包含指针。 |
| 忘记 `struct` 关键字 | 没有 typedef 时，`Point p;` 不是合法类型名。 |
| 把 `int **` 当成二维数组 | 布局不同；二维数组退化为 `int (*)[N]`。 |

## 7.17 语法卡片

| 术语 | 一句话定义 | 本章例子 |
|------|------------|----------|
| 结构体 | 多个成员组成的聚合类型 | `struct Point` |
| 成员 | 结构体里的一个对象 | `p.x` |
| 标签 | `struct`/`union`/`enum` 后的名字 | `struct Point` |
| 结构体赋值 | 按值复制整个结构体 | `q = p;` |
| 填充 | 为满足对齐插入的未使用字节 | `char` 后到 `int` 前 |
| 对齐 | 对象地址需要满足的边界 | `_Alignof` |
| `offsetof` | 成员相对结构体起始的偏移 | `offsetof(struct Point, y)` |
| `typedef` | 类型别名，不创建新类型 | `typedef struct Point Point;` |
| 联合体 | 成员共享存储的类型 | `union Number` |
| tagged union | 用标签记录当前活动成员的 union | `struct Variant` |
| 枚举 | 一组命名整数常量 | `enum Color` |
| 位域 | 指定宽度的整数成员 | `unsigned int ready : 1;` |
| 复合字面量 | 在表达式里构造匿名对象 | `(struct Point){1, 2}` |

## 7.18 练习

1. **矩形**：定义 `struct Rectangle { struct Point top_left; struct Point bottom_right; };`，写面积和周长函数。
2. **结构体数组**：定义 `struct Student { char name[32]; int score; };`，初始化 3 个学生，按分数从高到低排序并打印。
3. **指针与值**：写 `void move_point(struct Point p, int dx, int dy)` 和 `void move_point_ptr(struct Point *p, int dx, int dy)`，比较调用者的对象是否改变。
4. **布局**：定义三个成员相同但顺序不同的结构体，打印 `sizeof`、`offsetof` 和 `_Alignof`，画出 padding。
5. **typedef**：用 `typedef struct Point Point;`，再写一个同名标签和 typedef 名的例子，确认它们在不同命名空间。
6. **链表**：用 4 个静态 `struct Node` 构造链表，遍历并求和；再写一个函数返回链表长度。
7. **tagged union**：扩展 `struct Variant`，支持 `int`、`double` 和一个 `const char *`，用 `switch` 打印。
8. **枚举状态机**：定义 `enum State { START, RUNNING, STOPPED };`，用 `switch` 实现一个简单的状态转换函数。
9. **位域**：用 1 位表示 `ready`、2 位表示 `mode`、3 位表示 `priority`，写设置、清除和打印函数。
10. **复合字面量**：写 `struct Point make_point(int x, int y)` 返回复合字面量；再写一个返回局部复合字面量地址的错误版本，解释为什么危险。
11. **`memcmp`**：定义两个成员相同的结构体，先清零再用 `memcmp` 比较；然后只初始化成员，不初始化 padding，观察结果是否可靠。解释原因。
12. **union 类型双关**：用 `union { unsigned int u; unsigned char bytes[4]; }` 查看整数的字节表示；说明字节序和实现定义的影响。

## 7.19 下一章

**第 8 章 · 存储期、链接与对象模型**

下一章我们会把“对象在哪里、活多久、谁能看见它”讲清楚：自动存储、静态存储、线程存储、内部链接、外部链接、`static`/`extern`/`const` 的真实含义，以及多文件程序如何链接成一个整体。
