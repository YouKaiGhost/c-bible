# 第 3 章 · 控制流

> **卷一 · 语言**　|　前置知识：第 2 章　|　建议用时：35 分钟

> 程序的力量不在于它会计算，而在于它会选择、重复和退出。

到第 2 章为止，程序都是一条直线：声明、计算、打印、返回。这一章引入分叉和循环，让程序开始根据条件走不同的路，并把同一段代码执行很多次。

C 的控制流语句不多，但每一个都有自己的边界条件。真正的难点不是语法，而是**把“什么时候继续、什么时候退出”写清楚**。

## 3.1 本章语法地图

| 语法 | 本章出现的位置 |
|------|----------------|
| `if` / `else if` / `else` | 根据条件选择分支 |
| `while` | 先判断，后循环 |
| `do-while` | 先执行一次，再判断 |
| `for` | 初始化、条件、迭代三件事写在一行 |
| `break` | 退出最近的循环或 `switch` |
| `continue` | 跳过本轮剩余部分，进入下一轮 |
| `switch` / `case` / `default` | 多路分支 |
| `goto` / 标签 | 跳出多层循环、集中清理 |
| 空语句 `;` | 有意让循环体为空 |

## 3.2 `if`：让程序做选择

最简单的形式：

```c
if (condition) {
    /* condition 为真时执行 */
}
```

加上 `else`：

```c
if (n > 0) {
    printf("positive\n");
} else if (n < 0) {
    printf("negative\n");
} else {
    printf("zero\n");
}
```

几个关键点：

- 条件可以是任何标量表达式：整数、浮点、指针。0 为假，非 0 为真。
- `else if` 不是一个关键字，它是 `else` 后面跟另一个 `if`。
- 花括号在语法上可以省略，但建议总是写。省略花括号是很多 bug 的来源。
- `if` 后面的表达式不需要和 `_Bool` 比较。`if (flag)` 比 `if (flag == 1)` 更通用，因为任何非零值都为真。

### 悬空 `else`

```c
if (n > 0)
    if (n % 2 == 0)
        printf("positive even\n");
    else
        printf("positive odd\n");
```

这个 `else` 属于**内层** `if`，不是外层。规则是：`else` 与最近的、尚未配对的 `if` 结合。

编译器通常会用 `-Wdangling-else` 警告这种写法。最便宜的修复方法是加花括号：

```c
if (n > 0) {
    if (n % 2 == 0) {
        printf("positive even\n");
    } else {
        printf("positive odd\n");
    }
}
```

### `if (x = 1)` 与 `if (x == 1)`

```c
if (x = 1) { ... }    // 赋值，条件恒为真
if (x == 1) { ... }   // 比较
```

赋值表达式也有值，所以它可以出现在条件里。编译器通常会用 `-Wparentheses` 提醒你。不要用“把常量写在左边”来替代认真检查；现代编译器已经能发现大多数这类错误。

## 3.3 `while`：先判断，再循环

```c
int i = 1;
int sum = 0;

while (i <= 5) {
    sum += i;
    ++i;
}
```

执行顺序：

1. 求条件 `i <= 5`；
2. 为真则执行循环体，再回到 1；
3. 为假则退出。

如果条件一开始就为假，循环体一次也不执行。循环变量必须在循环体里朝“退出条件”移动，否则就是无限循环。

### `while (1)` 与 `for (;;)`

两者都是常见的无限循环写法，退出要靠 `break`、`return` 或 `goto`：

```c
while (1) {
    if (should_stop()) {
        break;
    }
    do_work();
}
```

`while (1)` 在 C 里完全合法，因为 1 是非零常量；有些编译器会对常量条件给出提示，但这不是语言错误。

## 3.4 `do-while`：至少执行一次

```c
int i = 0;

do {
    printf("i = %d\n", i);
    ++i;
} while (i < 3);
```

`do-while` 先执行循环体，再判断条件。所以它至少执行一次，即使条件一开始就是假：

```c
do {
    printf("runs once\n");
} while (0);
```

注意末尾的分号：`do { ... } while (condition);` 的分号是语法的一部分。漏掉它，编译器会在下一条语句处报错。

`do-while` 适合“先做一次，再决定要不要继续”的场景，例如读取输入、重试操作。它不适合“条件可能一开始就不成立”的场景，那种情况用 `while`。

## 3.5 `for`：把三件事写在一行

```c
for (int i = 0; i < 5; ++i) {
    printf("%d\n", i);
}
```

三个部分：

1. **初始化**：`int i = 0`，只在进入循环时执行一次；
2. **条件**：`i < 5`，每轮开始前求值；
3. **迭代**：`++i`，每轮循环体结束后执行。

它大致等价于：

```c
{
    int i = 0;
    while (i < 5) {
        printf("%d\n", i);
        ++i;
    }
}
```

但有一个重要差别：`continue` 在 `for` 里会跳到**迭代表达式** `++i`，在 `while` 里会跳到**条件判断**。如果循环变量的更新写在循环体末尾，`continue` 可能跳过它，造成无限循环。

### 作用域

C99 起，`for` 初始化部分声明的变量，作用域限于这个循环：

```c
for (int i = 0; i < 3; ++i) {
    /* i 在这里可见 */
}
/* 这里不能再用 i */
```

这让循环变量不容易泄漏到外层，也避免了嵌套循环重名的问题。

### 省略部分

`for` 的三个部分都可以为空：

```c
for (;;) {
    /* 无限循环 */
}

for (int i = 0; ; ++i) {
    if (i == 10) {
        break;
    }
}
```

但省略条件时要确保有别的退出路径。

### 嵌套循环

```c
for (int row = 1; row <= 3; ++row) {
    for (int col = 1; col <= 3; ++col) {
        printf("%3d", row * col);
    }
    printf("\n");
}
```

外层每走一步，内层完整走一轮。嵌套层数越多，越要小心退出条件和 `break` 的作用范围。

## 3.6 `break` 与 `continue`

### `break`

`break` 退出**最近的**循环或 `switch`：

```c
for (int i = 0; i < 10; ++i) {
    if (i == 4) {
        break;       // 只退出这个 for
    }
    printf("%d ", i);
}
```

在嵌套循环里，`break` 只退出内层：

```c
for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
        if (j == 1) {
            break;   // 只退出内层 for，外层继续
        }
        printf("(%d,%d) ", i, j);
    }
}
```

想一次退出多层，可以用 `goto`，或者用一个 `done` 标志配合外层条件。

### `continue`

`continue` 跳过本轮循环体剩下的部分，直接进入下一轮：

```c
for (int i = 0; i < 6; ++i) {
    if (i % 2 == 0) {
        continue;
    }
    printf("%d ", i);   // 只打印奇数
}
```

- 在 `while` 中，`continue` 跳到条件判断。
- 在 `do-while` 中，`continue` 跳到条件判断。
- 在 `for` 中，`continue` 跳到迭代表达式，然后才判断条件。

如果 `for` 的迭代表达式负责更新循环变量，`continue` 不会跳过它；如果更新写在循环体里，`continue` 可能把它跳过。

## 3.7 `switch`：多路分支

当条件是“从一组整数里选一个”时，`switch` 比一长串 `else if` 更清楚：

```c
switch (n) {
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
```

规则：

- 控制表达式必须是**整数类型**（包括 `char`、枚举）；不能是浮点或指针。
- `case` 标号必须是**整数常量表达式**，不能是变量。`const int n = 1; case n:` 在 C 里不合法。
- 同一个 `switch` 里，转换后重复的 `case` 值是编译错误。
- `default` 最多一个，位置任意；没有匹配且没有 `default` 时，什么也不做。
- `break` 退出 `switch`；没有 `break` 就继续执行下一个 `case` 的代码，这叫 **fallthrough**。
- 有意 fallthrough 时写注释，让读者和编译器都知道你不是忘了 `break`。

### fallthrough 不是 bug，但常常是

```c
case 3:
    printf("three, ");
    /* fall through */
case 4:
    printf("three or four\n");
    break;
```

当 `n == 3` 时，会先打印 `three, `，再进入 `case 4`，打印 `three or four`。这是有意设计的。

如果每个 `case` 都以 `break` 或 `return` 结束，通常更安全。`switch` 里的 `return` 会直接离开整个函数，不只是离开 `switch`。

### `case` 标签后面的声明

C17 要求标签后面跟一条语句，而声明不是语句。所以下面这种写法在 C17 里不合法：

```c
switch (n) {
case 1:
    int x = 10;    // C17: 标签后不能直接跟声明
    printf("%d\n", x);
    break;
}
```

加一对花括号，把声明包进复合语句：

```c
switch (n) {
case 1: {
    int x = 10;
    printf("%d\n", x);
    break;
}
}
```

C23 放宽了这条限制，但以 C17 为基线时，记住这个坑。

### `switch` 里的 `break` 与循环里的 `break`

`break` 只退出**最近**的 `switch` 或循环。如果 `switch` 在循环里，`break` 退出的是 `switch`，不是循环：

```c
while (running) {
    switch (command) {
    case QUIT:
        break;       // 只退出 switch，循环还在
    }
}
```

要退出循环，得在 `switch` 外面另写 `break`，或者用 `return`/`goto`。

## 3.8 `goto`：被误解的工具

`goto` 的语法很简单：

```c
goto label;

label:
    /* ... */
```

标签的作用域是**整个函数**，不是块。标签名在函数内必须唯一。`goto` 只能在同一函数内跳转。

`goto` 最常见的两种正当用途：

### 1. 从多层嵌套中退出

C 没有带标签的 `break`。要从三层循环里一次退出，`goto` 是最直接的写法：

```c
for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
        if (i == 1 && j == 1) {
            goto done;
        }
    }
}

done:
    /* 继续执行 */
```

替代方案是用一个 `done` 标志，让每层循环都检查它；代码会更长，也更容易漏掉一层。

### 2. 集中清理

C 没有异常和析构函数。需要释放多个资源时，常见模式是：

```c
int ok = 0;
FILE *f = fopen(...);
if (f == NULL) {
    goto cleanup;
}

/* ... */

cleanup:
    if (f != NULL) {
        fclose(f);
    }
    return ok;
```

`goto cleanup` 让所有失败路径汇合到同一个清理点，避免重复代码。

### `goto` 的危险

- 向后 `goto` 可以模拟循环，但通常会让控制流难以阅读；用 `while`/`for`。
- `goto` 可以跳过变量初始化，让对象处于不确定状态；不要这么做。
- `goto` 不能从函数外跳进来，也不能跳进 VLA 的作用域。
- 标签不是变量，没有类型，也不占运行时存储。

`goto` 不是“坏语言特性”，而是一把刀。用它做集中退出和清理是合适的；用它写面条代码不是。

## 3.9 空语句与空循环体

分号本身可以构成一条**空语句**：

```c
if (x > 0)
    ;              // 有意什么都不做
```

空循环体：

```c
while (getchar() != '\n')
    ;              // 消耗到行尾
```

这种写法合法，但很容易被误读。更清楚的做法是写 `{ }` 或加注释：

```c
while (getchar() != '\n') {
    /* 消耗到行尾 */
}
```

不要把 `for (...);` 后面的分号当成“循环体”，除非你确实想要空循环。一个多余的分号可能让循环体变成下一条语句。

## 3.10 代码导览

[`control.c`](control.c) 分成五段：

1. `show_if`：`if`/`else if`/`else`、悬空 `else`、花括号；
2. `show_loops`：`for`、`while`、`do-while`、嵌套循环、至少执行一次；
3. `show_break_continue`：`break`、`continue`、嵌套循环里的作用范围；
4. `show_switch`：分组 `case`、fallthrough、`break`、`default`、`return`；
5. `show_goto`：跳出多层循环、集中清理。

编译运行：

```sh
cc -std=c17 -Wall -Wextra -Wpedantic control.c -o control
./control
```

观察输出里的 `switch(3)`：它先打印 `three, `，再打印 `three or four`，这就是 fallthrough。

## 3.11 常见陷阱

| 陷阱 | 说明 |
|------|------|
| `if (x = 1)` | 赋值，不是比较；条件恒为真。 |
| 悬空 `else` | `else` 绑定最近的 `if`；用花括号消除歧义。 |
| `for (...);` | 多余的分号让循环体变成空语句。 |
| `while` 条件永远为真 | 循环变量没有更新，或 `continue` 跳过了更新。 |
| `break` 以为能退出多层循环 | 它只退出最近的一层循环或 `switch`。 |
| `switch` 忘记 `break` | 会 fallthrough 到下一个 `case`。 |
| `case` 用变量 | `case` 标号必须是整数常量表达式。 |
| `case` 后直接声明变量 | C17 要求标签后是语句；用花括号包成复合语句。 |
| `do-while` 漏分号 | `while (...)` 后面必须有分号。 |
| `goto` 跳进 VLA 作用域 | 违反 C 的约束；不要跨作用域跳转。 |
| 用 `goto` 写循环 | 可读性差；用 `while`/`for`。 |

## 3.12 语法卡片

| 术语 | 一句话定义 | 本章例子 |
|------|------------|----------|
| 分支 | 根据条件选择执行路径 | `if`、`switch` |
| 循环 | 重复执行一段代码 | `while`、`do-while`、`for` |
| 循环体 | 被重复执行的语句或块 | `{ ... }` |
| 迭代 | 每轮循环结束后的更新动作 | `++i` |
| `break` | 退出最近的循环或 `switch` | `break;` |
| `continue` | 跳过本轮剩余部分 | `continue;` |
| fallthrough | 没有 `break` 时继续执行下一个 `case` | `case 3:` 落入 `case 4:` |
| 标签 | 函数内 `goto` 的目标 | `done:` |
| 空语句 | 只有一个分号的语句 | `;` |

## 3.13 练习

1. **FizzBuzz**：打印 1 到 20。3 的倍数打印 `Fizz`，5 的倍数打印 `Buzz`，15 的倍数打印 `FizzBuzz`，其余打印数字。
2. **求和**：用 `for`、`while`、`do-while` 三种写法分别求 1 到 100 的和，比较它们的结构。
3. **跳过**：打印 1 到 20，但跳过所有 3 的倍数；用 `continue` 实现。
4. **嵌套退出**：用两层循环找第一个满足 `i * j > 20` 的组合。先用 `break` 加标志，再用 `goto`，比较两种写法。
5. **`switch`**：写一个函数，输入 1 到 7 输出星期几，输入其他值输出 `invalid`。再写一个版本，利用 fallthrough 把工作日归为一组。
6. **悬空 `else`**：把本章的悬空 `else` 例子改写成两种不同含义的版本，分别加花括号，观察输出。
7. **`for` 作用域**：在 `for (int i = 0; ...)` 结束后打印 `i`，看编译器如何报错。解释为什么这是好事。
8. **`case` 声明**：在 `case 1:` 后直接写 `int x = 10;`，用 C17 编译，读诊断；再用花括号修复。
9. **空循环**：写一个 `while (getchar() != '\n') ;` 的程序，故意在循环后多写一个分号，观察控制流如何改变。不要在有输入依赖的场景里运行失控版本。

## 3.14 下一章

**第 4 章 · 函数与作用域**

下一章我们会把代码切成可复用的块：函数声明、定义、参数传递、返回值、递归、作用域和生命期。你会第一次看到“按值传递”到底意味着什么，以及为什么一个函数改不了调用者的局部变量。
