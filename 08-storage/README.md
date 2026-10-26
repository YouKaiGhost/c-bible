# 第 8 章 · 存储期、链接与对象模型

> **卷一 · 语言**　|　前置知识：第 7 章　|　建议用时：55 分钟

> 名字有两个问题：它在哪里可见，以及它指的是不是同一个对象。

到目前为止，大多数程序只有一个源文件。真实项目通常由多个翻译单元组成：每个 `.c` 文件单独编译，最后链接成一个可执行文件。这时，C 必须回答几个问题：

- 一个对象活多久？
- 一个名字在哪些地方可见？
- 不同文件里的同名声明，指的是同一个对象吗？
- `static`、`extern`、`const` 到底改变了什么？

这一章把**作用域**、**存储期**、**链接**和**生命期**拆开讲清楚。它们是 C 对象模型的核心。

## 8.1 本章语法地图

| 语法 | 本章出现的位置 |
|------|----------------|
| 自动存储 | 块作用域的普通变量 |
| 静态存储 | 文件作用域对象、`static` 局部变量 |
| 线程存储 | `_Thread_local`（C11） |
| 内部链接 | 文件作用域的 `static` |
| 外部链接 | 文件作用域的非 `static` 对象/函数 |
| `extern` | 声明但不定义，引用别的翻译单元 |
| `static` | 内部链接，或静态存储期 |
| `const` 链接 | C 里文件作用域 `const` 默认外部链接 |
| `static inline` | 只在本翻译单元内联的函数 |
| `volatile` | 每次访问都不可省略 |
| `restrict` | 指针别名承诺（C99） |
| `_Noreturn` | 函数不返回（C11） |
| tentative definition | 文件作用域的 `int x;` |

## 8.2 四个维度

先把四个容易混淆的词分开：

| 维度 | 回答的问题 | 例子 |
|------|------------|------|
| 作用域（scope） | 名字在源码的哪里可见？ | 块、文件、函数原型 |
| 生命期（lifetime） | 对象在运行时什么时候存在？ | 块进入/退出、程序开始/结束 |
| 存储期（storage duration） | 对象的存储属于哪一类？ | 自动、静态、线程、动态分配 |
| 链接（linkage） | 不同声明是否指同一个对象/函数？ | 无链接、内部链接、外部链接 |

它们互相关联，但不能互相替代。

- 一个 `static` 局部变量：作用域是块，生命期是整个程序，存储期是静态，链接是无。
- 一个文件作用域 `static int x;`：作用域是文件，生命期是整个程序，存储期是静态，链接是内部。
- 一个普通局部变量：作用域是块，生命期是块，存储期是自动，链接是无。
- 一个 `extern int x;` 声明：如果写在块里，作用域是块，但链接是外部；它引用的是别处定义的对象。

## 8.3 存储期

### 自动存储期

块作用域内、没有 `static` 或 `_Thread_local` 的对象，通常具有自动存储期：

```c
void f(void)
{
    int x = 1;      // 自动存储期
    {
        int y = 2;  // 自动存储期
    }               // y 的生命期结束
}                   // x 的生命期结束
```

特点：

- 进入块时创建，离开块时销毁；
- 未初始化时值不确定；
- 通常放在调用栈上，但不要求；
- 函数返回后，指向局部变量的指针会悬空。

### 静态存储期

文件作用域对象，以及块作用域里带 `static` 的对象，具有静态存储期：

```c
int global;              // 静态存储期，零初始化

void f(void)
{
    static int count = 0;   // 静态存储期，只初始化一次
    ++count;
}
```

特点：

- 生命期是整个程序；
- 未显式初始化时零初始化；
- `static` 局部变量的名字只在块内可见，但存储在程序运行期间一直存在；
- 不是可重入的，也不是线程安全的。

### 线程存储期

C11 的 `_Thread_local` 给每个线程一份独立对象：

```c
_Thread_local int tls_counter = 0;
```

它适合线程私有的计数器、错误码等。没有线程库时也能声明，但真正的并发访问需要线程支持。`_Thread_local` 可以和 `static`/`extern` 组合。

### 动态分配存储期

`malloc`/`free` 创建和销毁的对象具有动态分配存储期：

```c
int *p = malloc(10 * sizeof *p);
/* ... */
free(p);
```

它不属于本章的语言核心；这里只记住：动态对象的生命期由程序员控制，`free` 之后指针悬空。

## 8.4 链接

**链接**决定不同翻译单元里的声明是否指同一个对象或函数。

### 无链接

块作用域的局部变量、函数参数没有链接：

```c
void f(int x)
{
    int y = 1;
}
```

每个 `x`、`y` 都是独立的局部对象，别的函数无法按名字访问。

### 内部链接

文件作用域的 `static` 对象和函数具有内部链接：

```c
static int counter = 0;
static void helper(void) { }
```

它们只在**当前翻译单元**可见。另一个 `.c` 文件即使写 `extern int counter;`，也链接不到这个对象（会得到未定义引用）。每个翻译单元可以有自己的同名 `static` 对象，它们互不相干。

内部链接适合：

- 模块私有的辅助函数；
- 模块私有的全局状态；
- 避免不同文件里的同名符号冲突。

### 外部链接

文件作用域的非 `static` 对象和函数具有外部链接：

```c
int shared = 0;
void bump(void) { ++shared; }
```

它们可以被其他翻译单元通过 `extern` 声明访问：

```c
extern int shared;
void bump(void);
```

整个程序里，一个外部对象/函数只能有一个定义（暂定定义除外）。这就是为什么头文件里通常只放声明，定义放在 `.c` 文件里。

### 链接与作用域的区别

```c
void f(void)
{
    extern int shared;   // 作用域是块，链接是外部
    ++shared;
}
```

这个 `extern` 声明在块内可见，但它引用的是文件作用域的外部对象。作用域和链接是两个维度。

## 8.5 `static` 与 `extern`

### `static` 的两种主要含义

1. **文件作用域**：内部链接。

```c
static int module_state = 0;
static void helper(void) { }
```

2. **块作用域**：静态存储期。

```c
void f(void)
{
    static int calls = 0;
    ++calls;
}
```

同一个关键字，位置不同，作用不同。记法：

- 在函数外：`static` 管“谁能看见”；
- 在函数内：`static` 管“活多久”。

C99 还有第三种用法：数组参数里的 `static` 表示最小元素个数：

```c
void f(int a[static 10]);
```

它告诉编译器“传入的数组至少有 10 个元素”。这是优化提示，不是运行时检查，初学阶段可以忽略。

### `extern`

`extern` 表示“这个对象/函数在别处定义”：

```c
extern int shared;       // 声明，不分配存储
extern void bump(void);  // 函数声明
```

注意：

- `extern int x;` 是声明，不是定义。
- `extern int x = 1;` 在文件作用域是**定义**（带初始化器）。
- 函数声明默认就是 `extern`，写不写都一样。
- 在块作用域写 `extern`，引用的仍是外部链接的对象。

### `const` 的链接属性

这是 C 和 C++ 的一个经典差异：

```c
const int global_const = 42;        // C：外部链接
static const int internal_const = 7; // 内部链接
```

在 C 里，文件作用域的 `const` 默认具有**外部链接**；在 C++ 里默认是内部链接。所以不要在头文件里直接写 `const int x = 1;`，否则每个包含它的翻译单元都可能产生一个定义，链接时冲突。常见做法是：

```c
/* header */
extern const int global_const;

/* one .c file */
const int global_const = 42;
```

或者用 `static const` 让每个翻译单元各有自己的副本。

## 8.6 其他存储类与限定符

### `register`

`register` 建议编译器把对象放在寄存器里：

```c
register int i = 0;
```

现代编译器通常忽略这个建议。它的实际约束是：**不能对 `register` 对象取地址**。

### `auto`

C17 的 `auto` 是存储类说明符，表示自动存储期：

```c
auto int x = 1;   // 等价于 int x = 1;
```

它几乎没人写。C23 改变了 `auto` 的含义，用于类型推断；本书以 C17 为基线，看到旧代码里的 `auto` 时知道它是“自动存储”即可。

### `inline`

C99 的 `inline` 很微妙。`static inline` 是最安全的用法：

```c
static inline int square(int x)
{
    return x * x;
}
```

`static inline` 具有内部链接，每个翻译单元都有自己的定义，不会在链接时冲突。

不带 `static` 的 `inline` 函数具有外部链接，但**不一定**产生外部定义；标准要求某个翻译单元提供外部定义，否则链接可能失败。不同编译器的行为细节容易踩坑。初学阶段记住：

- 头文件里的内联函数用 `static inline`；
- 需要跨翻译单元共享的实现，放在 `.c` 文件里，或者使用明确的 `extern inline` 规则。

### `volatile`

`volatile` 告诉编译器：这个对象可能被程序之外的力量改变，每次访问都必须真的发生，不能缓存或优化掉。

```c
volatile int flag = 0;
```

它适合：

- 内存映射 I/O 寄存器；
- 信号处理函数修改的变量；
- 某些嵌入式场景。

它**不是**线程同步工具。`volatile` 不提供原子性，也不提供内存顺序。多线程通信用原子类型或互斥量。

### `restrict`

`restrict` 是 C99 的指针限定符，表示“在这个指针的生命期内，对象只通过这个指针（或从它派生的指针）访问”：

```c
void add(int *restrict dst, const int *restrict a, const int *restrict b);
```

它是给编译器的别名承诺。违反承诺是未定义行为。标准库的 `memcpy` 就用了类似约定：源和目标不能重叠；重叠时要用 `memmove`。第 10 章会展开。

### `_Noreturn`

C11 的 `_Noreturn` 表示函数不会返回：

```c
_Noreturn void fatal(const char *message);
```

调用这样的函数后，控制流不会回到调用点。编译器可以据此优化，也不会警告“缺少返回值”。`exit`、`abort`、`longjmp` 属于这一类。C23 用 `[[noreturn]]` 属性表达同样的事。

### `_Thread_local` 与 `_Atomic`

- `_Thread_local`：线程存储期，每个线程一份对象。
- `_Atomic`：原子类型和原子操作，需要 `<stdatomic.h>`；它解决的是并发访问，不是普通变量能替代的。

## 8.7 对象模型与暂定定义

C 的对象模型可以概括成：

- 对象是一块存储区域；
- 类型决定大小、对齐和解释方式；
- 生命期决定它什么时候存在；
- 链接决定不同声明是否指同一个对象；
- 作用域决定名字在源码哪里可见。

### 暂定定义

文件作用域下，没有初始化器、没有 `extern` 的对象声明，是**暂定定义**（tentative definition）：

```c
int x;        // 暂定定义
int x;        // 同一翻译单元里可以重复
int x = 1;    // 真正的定义
```

如果整个翻译单元里没有真正的定义，编译器会在末尾把它当作零初始化的定义。所以：

```c
int x;
int x;
```

在同一个 `.c` 文件里合法，最终只有一个 `x`。

但下面不合法：

```c
int x = 1;
int x = 2;    // 重复定义
```

跨翻译单元时，一个外部对象也只能有一个真正的定义。多个 `.c` 文件都写 `int shared = 0;` 会在链接时冲突。

### 头文件的正确姿势

头文件里放声明：

```c
/* counter.h */
#ifndef COUNTER_H
#define COUNTER_H

extern int shared_counter;
void bump(void);

#endif
```

定义放在一个 `.c` 文件里：

```c
/* counter.c */
#include "counter.h"

int shared_counter = 0;

void bump(void)
{
    ++shared_counter;
}
```

不要把带初始化的对象定义直接放进头文件，否则每个包含它的翻译单元都会得到一个定义，链接时重复。`static`/`static inline` 是例外：它们本来就允许每个翻译单元一份。

## 8.8 多文件程序

本章的程序由三个文件组成：

```text
08-storage/
├── counter.h   声明
├── counter.c   shared_counter、bump、static 辅助函数
└── storage.c   main、file_scope、static 对象
```

编译命令：

```sh
cc -std=c17 -Wall -Wextra -Wpedantic storage.c counter.c -o storage
```

`cc` 先分别编译两个 `.c` 文件，再把它们和启动代码、标准库链接成可执行文件。

### 外部链接的例子

- `storage.c` 定义 `int file_scope = 100;`
- `counter.c` 写 `extern int file_scope;`，函数 `read_file_scope()` 返回它；
- `main` 调用 `read_file_scope()`，得到 100。

这说明两个翻译单元访问的是同一个对象。

### 内部链接的例子

- `counter.c` 定义 `static int internal_bumps = 0;`
- 这个对象只在 `counter.c` 内可见；
- `storage.c` 不能按名字访问它，只能通过 `internal_bump_count()` 这个公开函数读取。

### `const` 链接的例子

- `storage.c` 定义 `const int external_const = 42;`，C 里是外部链接；
- `counter.c` 用 `extern const int external_const;` 读取它；
- `static const int internal_const = 7;` 只在 `storage.c` 内可见。

### 静态局部变量的例子

`next_local` 里的 `static int n = 0;` 只初始化一次。连续调用得到 1、2、3。

## 8.9 代码导览

[`storage.c`](storage.c) 与 [`counter.c`](counter.c) 一起编译。

`storage.c`：

1. 定义 `file_scope`、`file_static`、`external_const`、`internal_const`、`tentative`；
2. `static inline square`；
3. `next_local` 的静态局部变量；
4. 调用 `counter.c` 提供的函数，观察跨翻译单元访问。

`counter.c`：

1. 定义 `shared_counter`；
2. `static int internal_bumps` 和 `static void note_bump`；
3. `bump`、`read_counter`、`internal_bump_count`；
4. `read_file_scope`、`read_external_const` 通过 `extern` 读取另一个翻译单元的对象。

运行：

```sh
cc -std=c17 -Wall -Wextra -Wpedantic storage.c counter.c -o storage
./storage
```

输出会显示：

- `file_scope` 和 `external_const` 能被另一个翻译单元读取；
- `shared_counter` 在两次 `bump` 后变成 2；
- `internal_bumps` 也被同步增加，但只能通过函数访问；
- `next_local` 连续返回 1、2、3；
- `tentative` 被零初始化为 0。

## 8.10 常见陷阱

| 陷阱 | 说明 |
|------|------|
| 以为 `static` 只有一种含义 | 文件作用域是内部链接；块作用域是静态存储期。 |
| 把 `extern` 当定义 | `extern int x;` 不分配存储；定义在别处。 |
| 在头文件定义普通对象 | 多个翻译单元包含时重复定义；头文件只放声明。 |
| 以为 `const` 默认内部链接 | 在 C 里文件作用域 `const` 默认外部链接；C++ 不同。 |
| 认为静态局部变量每次调用重置 | 只初始化一次，跨调用保留值。 |
| 认为静态局部变量线程安全 | 所有线程共享同一个对象；需要同步。 |
| 用 `volatile` 做线程同步 | 它不提供原子性，也不提供内存顺序。 |
| 普通 `inline` 导致链接错误 | 用 `static inline`，或提供外部定义。 |
| 多个 `.c` 定义同一个外部对象 | 链接时重复定义；一个定义，多个声明。 |
| 返回局部变量的地址 | 自动对象在函数返回后消失，指针悬空。 |
| 以为 `auto` 是类型推断 | C17 的 `auto` 是存储类；C23 才改变含义。 |
| 对 `register` 变量取地址 | 约束违反；编译器会诊断。 |

## 8.11 语法卡片

| 术语 | 一句话定义 | 本章例子 |
|------|------------|----------|
| 翻译单元 | 源文件经过预处理后的结果 | `storage.c` |
| 程序 | 多个翻译单元链接后的整体 | `storage` |
| 作用域 | 名字在源码中可见的区域 | 块、文件 |
| 生命期 | 对象在运行时存在的区间 | 块、程序 |
| 存储期 | 对象存储的类别 | 自动、静态、线程、动态 |
| 链接 | 不同声明是否指同一对象/函数 | 内部、外部、无 |
| 内部链接 | 只在当前翻译单元可见 | `static int x;` |
| 外部链接 | 可被其他翻译单元引用 | `int x;`、`extern` |
| 暂定定义 | 文件作用域无初始化的对象声明 | `int x;` |
| `static` 局部 | 块作用域、静态存储期 | `static int n;` |
| `static inline` | 内部链接的内联函数 | `static inline int f(...)` |
| `volatile` | 每次访问都必须发生 | `volatile int flag;` |
| `restrict` | 指针别名承诺 | `int *restrict p` |
| `_Noreturn` | 函数不返回 | `_Noreturn void fatal(...)` |

## 8.12 练习

1. **多文件计数器**：给 `counter.c` 增加 `void reset_counter(void)`，在头文件声明，在 `storage.c` 调用。
2. **内部链接**：把 `internal_bumps` 的 `static` 去掉，在 `storage.c` 里写 `extern int internal_bumps;` 并访问它。观察是否还能链接，以及为什么这破坏了封装。
3. **`const` 链接**：把 `external_const` 改成 `static const`，在 `counter.c` 里保留 `extern` 声明，观察链接错误；再改回来。
4. **暂定定义**：在同一个 `.c` 文件里写两次 `int tentative;`，确认可以编译；再写两次 `int tentative = 1;`，观察重复定义错误。
5. **静态局部**：写两个函数，都调用同一个 `next_local`，观察它们共享同一个计数器；再讨论线程安全。
6. **头文件**：故意在头文件里定义 `int shared = 0;`，让两个 `.c` 都包含它，观察链接错误；改成 `extern` 声明后修复。
7. **`static inline`**：在头文件里写 `static inline int square(int x)`，在两个 `.c` 文件里调用，确认不会重复定义。
8. **普通 `inline`**：把 `static inline` 改成 `inline`，观察是否出现链接错误；查一查 C99 的 `inline` 规则。
9. **`volatile`**：写一个 `volatile int` 变量，做几次读写并打印；再用 `objdump` 或优化选项观察它是否被保留。解释它为什么不是原子操作。
10. **`_Thread_local`**：声明一个 `_Thread_local int`，在主线程读写；如果环境支持线程，创建两个线程各读写一次，观察每个线程有独立副本。
11. **作用域与生命期**：列一个表，写出局部变量、静态局部变量、文件作用域 `static`、文件作用域非 `static` 四项的作用域、生命期、链接。
12. **返回地址**：写一个返回局部变量地址的函数，调用后打印指针；解释为什么这是未定义行为。

## 8.13 下一章

**第 9 章 · 预处理器**

下一章我们会进入编译之前的世界：`#include`、`#define`、条件编译、宏展开、字符串化、记号粘贴，以及那些“看起来像函数、其实是文本替换”的宏陷阱。
