# 第 9 章 · 预处理器

> **卷一 · 语言**　|　前置知识：第 8 章　|　建议用时：50 分钟

> 预处理器不理解类型、作用域和函数。它只认识记号、行和替换。

在编译器真正开始工作之前，C 程序还要经过一个独立的阶段：**预处理**。`#include`、`#define`、`#if` 这些以 `#` 开头的行，都是给预处理器看的。

预处理器不是 C 语言的一部分吗？它是 C 翻译过程的一部分，但它的规则和 C 的语法规则不同。它做的是**记号级的文本处理**：展开头文件、替换宏、选择性地保留或删除代码。它不知道 `int` 是什么，也不知道变量在哪里声明。

这一章讲预处理器的语法和常见陷阱。宏很强大，但也很容易把代码变成谜语。

## 9.1 本章语法地图

| 语法 | 本章出现的位置 |
|------|----------------|
| `#include` | 引入头文件 |
| `#define` / `#undef` | 定义/取消宏 |
| 对象宏 | `#define VERSION 3` |
| 函数宏 | `#define SQUARE(x) ((x) * (x))` |
| `#` | 字符串化 |
| `##` | 记号粘贴 |
| `...` / `__VA_ARGS__` | 可变参数宏 |
| `#if` / `#ifdef` / `#ifndef` | 条件编译 |
| `#elif` / `#else` / `#endif` | 条件分支 |
| `#error` | 主动制造编译错误 |
| `#line` | 改变行号和文件名 |
| `#pragma` | 实现定义的指令 |
| 预定义宏 | `__STDC_VERSION__`、`__FILE__`、`__LINE__` |

## 9.2 预处理器在做什么

C 的翻译过程大致分为：

```text
源文件
  │ 预处理：处理 #include、#define、条件编译、删除注释
  ▼
预处理后的翻译单元
  │ 编译：语法/语义分析，生成目标代码
  ▼
目标文件
  │ 链接
  ▼
可执行文件
```

预处理器的输入和输出都是**预处理记号**（preprocessing tokens）。它不理解类型，也不做类型检查。它只做几件事：

1. 把 `#include` 的文件内容插入当前文件；
2. 展开宏；
3. 根据条件编译保留或删除代码；
4. 处理 `#line`、`#error`、`#pragma` 等指令。

所以宏不是函数，`#define` 不是变量声明。它们只是文本替换规则。

## 9.3 `#include` 与头文件

```c
#include <stdio.h>    // 在实现提供的系统头文件目录里找
#include "config.h"   // 通常先找当前文件所在目录，再找系统目录
```

- 尖括号形式用于标准库和系统头文件。
- 双引号形式用于项目自己的头文件；具体搜索顺序由实现决定，但通常会先找当前文件目录。
- `#include` 不是 C 语句，不以分号结尾。

### include guard

头文件可能被多个文件包含，也可能被同一个文件间接包含多次。如果头文件里有类型定义、枚举常量或函数定义，重复包含就会产生重复定义。

经典解法是 **include guard**：

```c
#ifndef CONFIG_H
#define CONFIG_H

/* 头文件内容 */

#endif
```

第一次包含时，`CONFIG_H` 未定义，内容被保留；第二次包含时，`CONFIG_H` 已定义，整个文件被跳过。

本章的 [`config.h`](config.h) 就用了这个模式。`preprocessor.c` 故意包含它两次，验证 guard 生效。

`#pragma once` 是另一种常见写法：

```c
#pragma once
```

它更简洁，但 `#pragma` 的具体行为由实现定义，不是 ISO C 的标准保证。可移植代码通常还是用 include guard。

### 头文件里放什么

- 函数声明（原型）；
- 外部对象声明（`extern`）；
- 类型定义（`struct`、`union`、`enum`、`typedef`）；
- 宏和常量；
- `static inline` 函数（每个翻译单元一份）；
- include guard。

不要把普通对象的定义、非 inline 函数的定义直接放进头文件，否则多个翻译单元包含它时会产生重复定义。

## 9.4 宏

### 对象宏

```c
#define VERSION 3
#define APP_NAME "C Bible"
```

使用 `VERSION` 时，预处理器把它替换成 `3`。没有类型，不遵守 C 的块作用域，也没有地址。

```c
printf("%d\n", VERSION);   // 展开成 printf("%d\n", 3);
```

对象宏常用于配置常量、特性开关、简短的表达式。它和 `const` 变量不同：

- 宏没有类型，编译器看到的已经是替换后的值；
- 宏没有作用域，`#undef` 才能取消；
- 宏可能被意外重定义；
- 调试器看不到宏名，只能看到展开后的值。

能用 `enum`、`const` 或 `static const` 表达的东西，优先用它们；宏留给预处理阶段真正需要的地方。

### 函数宏

```c
#define SQUARE(x) ((x) * (x))
```

函数宏看起来像函数，但它只是文本替换。调用：

```c
SQUARE(1 + 2)
```

展开成：

```c
((1 + 2) * (1 + 2))
```

得到 9。如果写成没有括号的版本：

```c
#define SQUARE_BAD(x) x * x
```

`SQUARE_BAD(1 + 2)` 展开成：

```c
1 + 2 * 1 + 2
```

按优先级计算得到 5。这就是为什么函数宏的参数和整个替换体都要加括号。

### 宏的多次求值

即使加了括号，宏仍可能多次求值参数：

```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))
```

`MAX(i++, j++)` 会展开成：

```c
((i++) > (j++) ? (i++) : (j++))
```

条件里自增了一次，选中的分支又自增一次。参数被多次求值，结果很难预测。**不要给宏传带副作用的表达式。**

这也是为什么 C++ 更倾向于 `inline` 函数和模板；C 里如果需要类型安全和单次求值，应该写函数。

### 宏语句与 `do { } while (0)`

如果宏要展开成多条语句，直接写 `{ ... }` 会在 `if/else` 里出问题：

```c
#define BAD_SWAP(a, b) { int t = (a); (a) = (b); (b) = t; }

if (cond)
    BAD_SWAP(x, y);   // 展开后 if 只控制第一句，else 会悬空
else
    ...
```

经典写法：

```c
#define SWAP(type, a, b) \
    do { type tmp = (a); (a) = (b); (b) = tmp; } while (0)
```

调用时写 `SWAP(int, x, y);`，展开成一条 `do-while` 语句，在 `if/else` 里行为正确。

### `#undef`

```c
#define TEMP 1
#undef TEMP
```

`#undef` 取消宏定义。之后的 `TEMP` 不再是宏；如果还有同名宏，需要重新定义。

宏没有块作用域。`#define` 从定义处生效到文件结束或 `#undef`，不会因为在某个 `{}` 里就失效。

## 9.5 宏的陷阱

| 陷阱 | 说明 |
|------|------|
| 参数没有括号 | `SQUARE_BAD(1 + 2)` 变成 `1 + 2 * 1 + 2`。 |
| 替换体没有括号 | 宏结果参与更高优先级运算时被拆开。 |
| 参数被多次求值 | `MAX(i++, j++)` 可能自增多次。 |
| 宏名和参数名冲突 | 宏是文本替换，不遵守 C 的作用域。 |
| 在宏里写分号 | 调用处再加分号，可能变成两条语句。 |
| 用宏定义类型 | 调试器看不到，错误信息难读；用 `typedef`。 |
| 用宏定义常量 | 没有类型；优先用 `enum`/`const`。 |
| 宏展开后语法错误 | 错误信息指向展开后的代码，阅读困难。 |
| 宏名冲突 | 宏名不遵守 C 的块作用域；用项目前缀避免冲突。 |
| 保留标识符 | 不要定义 `__` 开头或 `_大写字母` 开头的名字。 |

## 9.6 `#` 与 `##`

### 字符串化 `#`

在函数宏里，`#` 把参数变成字符串字面量：

```c
#define STRINGIFY_RAW(x) #x

STRINGIFY_RAW(hello)   // "hello"
```

但 `#` 的参数不会先展开。如果 `VERSION` 是宏：

```c
#define VERSION 3
#define STRINGIFY_RAW(x) #x
#define STRINGIFY(x) STRINGIFY_RAW(x)

STRINGIFY_RAW(VERSION)   // "VERSION"
STRINGIFY(VERSION)       // "3"
```

`STRINGIFY` 多了一层：先把 `VERSION` 展开成 `3`，再传给 `STRINGIFY_RAW` 做字符串化。这是宏编程里非常常见的“两层宏”技巧。

### 记号粘贴 `##`

`##` 把两个记号粘成一个：

```c
#define CONCAT_RAW(a, b) a##b
#define CONCAT(a, b) CONCAT_RAW(a, b)

int CONCAT(counter, 1) = 10;   // 展开成 int counter1 = 10;
```

同样，`##` 的操作数不会先展开；需要先展开时，用两层宏。

`##` 不能出现在替换体的开头或结尾，粘贴结果也必须是合法的预处理记号。

## 9.7 可变参数宏

C99 支持可变参数宏：

```c
#define LOG(fmt, ...) printf("[LOG] " fmt "\n", __VA_ARGS__)
```

调用：

```c
LOG("x = %d, y = %d", x, y);
```

展开成：

```c
printf("[LOG] " "x = %d, y = %d" "\n", x, y);
```

相邻字符串字面量会拼接，所以最终格式串是 `"[LOG] x = %d, y = %d\n"`。

注意：

- `__VA_ARGS__` 必须至少对应一个参数（C17 的约束）。
- 如果可变参数为空，不同编译器/标准版本的处理不一样；C23 的 `__VA_OPT__` 提供了标准写法。
- GCC 的 `##__VA_ARGS__` 是扩展，用来处理空参数，但不是 C17 标准。
- 可变参数宏仍然没有类型检查。

## 9.8 条件编译

```c
#if ENABLE_EXTRA
    printf("extra enabled\n");
#elif defined(FALLBACK)
    printf("fallback\n");
#else
    printf("extra disabled\n");
#endif
```

`#if` 后面是**整数常量表达式**，可以使用：

- 整数常量、字符常量；
- 宏；
- `defined(NAME)` 或 `defined NAME`；
- 算术、关系、逻辑、位运算。

不能用：

- `sizeof`；
- 类型转换；
- 变量；
- 枚举常量（预处理器看不到枚举，只看到宏）；
- 浮点表达式。

未定义的标识符在 `#if` 里会被替换成 0：

```c
#if UNKNOWN_MACRO
    // 这里的 UNKNOWN_MACRO 被当成 0
#endif
```

`#ifdef NAME` 等价于 `#if defined(NAME)`；`#ifndef` 是它的否定。include guard 就是基于 `#ifndef`。

`#if 0` 是常用的“注释掉一大段代码”的方法，因为它能正确处理嵌套注释和预处理指令。但被跳过的代码仍然要满足预处理器的结构规则，不能随便写乱。

### `#error`

```c
#if !defined(APP_NAME)
#error "APP_NAME must be defined"
#endif
```

`#error` 会让预处理阶段直接报错，适合检查编译环境或配置。`#warning` 在 C23 成为标准；在 C17 里它是 GCC 等编译器的扩展。

## 9.9 预定义宏与 `#line`

预处理器预定义了一些宏：

| 宏 | 含义 |
|----|------|
| `__STDC__` | 标准 C 实现通常为 1 |
| `__STDC_VERSION__` | 标准版本，C17 是 `201710L` |
| `__STDC_HOSTED__` | 宿主实现为 1，独立实现为 0 |
| `__FILE__` | 当前源文件名 |
| `__LINE__` | 当前行号 |
| `__DATE__` | 编译日期 |
| `__TIME__` | 编译时间 |
| `__func__` | 当前函数名（预定义标识符，不是宏） |

`__DATE__` 和 `__TIME__` 会随编译时间变化。用它们会让构建结果不可复现；需要可复现构建时，通常通过构建系统注入版本信息。

### `#line`

`#line` 改变后续代码的行号和文件名：

```c
#line 500 "virtual.c"
printf("%d\n", __LINE__);   // 500
```

它常用于代码生成器：生成器把原始源文件的行号写回 `#line`，这样编译器报错时能指向原始文件。

## 9.10 其他指令

- `#pragma`：实现定义的指令。常见的有 `#pragma once`、`#pragma pack`、`#pragma GCC diagnostic`。它们不可移植，使用时要清楚依赖的是哪个编译器。
- `_Pragma`：C99 的运算符形式，可以在宏里生成 `#pragma`。
- `#line`：改变行号和文件名。
- `#error`：主动报错。
- `#warning`：C23；C17 里可能是扩展。

## 9.11 代码导览

本章有两个文件：

- [`config.h`](config.h)：include guard、对象宏、枚举常量；
- [`preprocessor.c`](preprocessor.c)：宏、字符串化、粘贴、可变参数、条件编译、预定义宏、`#line`。

编译运行：

```sh
cc -std=c17 -Wall -Wextra -Wpedantic preprocessor.c -o preprocessor
./preprocessor
```

重点观察：

- `config.h` 被包含两次，但没有重复定义错误；
- `SQUARE_BAD(1 + 2)` 得到 5，`SQUARE(1 + 2)` 得到 9；
- `STRINGIFY(VERSION)` 得到 `"3"`；
- `CONCAT(counter, 1)` 生成变量 `counter1`；
- `TEMP` 被 `#undef` 后，`#ifdef TEMP` 为假；
- `#if 0` 里的 `#error` 不会触发；
- `#line 500 "virtual.c"` 之后，`__LINE__` 是 500，`__FILE__` 是 `"virtual.c"`；
- `__STDC_VERSION__` 是 `201710L`（C17）。

`__FILE__` 的具体值取决于你从哪个目录、用什么路径编译；这本身就是“实现/构建相关”的一个例子。

## 9.12 常见陷阱

| 陷阱 | 说明 |
|------|------|
| 宏参数没有括号 | 运算优先级会改变表达式含义。 |
| 宏参数有副作用 | 参数可能被多次求值。 |
| 宏替换体没有括号 | 宏结果参与外层运算时被拆开。 |
| 宏里写分号 | 调用处加分号会变成两条语句。 |
| 用 `#define` 定义常量 | 没有类型；优先用 `enum`/`const`。 |
| 用 `#define` 定义类型 | 调试困难；用 `typedef`。 |
| 头文件没有 include guard | 重复包含导致重复定义。 |
| 在头文件定义普通对象 | 多个翻译单元重复定义。 |
| `#if` 里用 `sizeof` 或变量 | 预处理器不认识它们。 |
| 认为 `#ifdef` 能检查变量 | 它只检查宏是否定义。 |
| `#` 参数没有先展开 | 需要两层宏。 |
| 可变参数宏空参数 | C17 约束和编译器扩展不同；用 C23 `__VA_OPT__` 或避免空参数。 |
| 使用保留标识符做宏名 | `__` 开头或 `_大写` 开头的名字保留给实现。 |

## 9.13 语法卡片

| 术语 | 一句话定义 | 本章例子 |
|------|------------|----------|
| 预处理 | 编译前的记号级处理 | `#include`、`#define` |
| 预处理记号 | 预处理器操作的基本单位 | 标识符、数字、字符串 |
| 对象宏 | 不带参数的宏 | `#define VERSION 3` |
| 函数宏 | 带参数的宏 | `#define SQUARE(x) ...` |
| 字符串化 | 用 `#` 把参数变成字符串 | `#x` |
| 记号粘贴 | 用 `##` 连接两个记号 | `a##b` |
| 可变参数宏 | 用 `...` 接收可变参数 | `#define LOG(fmt, ...)` |
| 条件编译 | 根据宏选择保留哪段代码 | `#if`、`#ifdef` |
| include guard | 防止头文件重复包含 | `#ifndef CONFIG_H` |
| 预定义宏 | 实现预先定义的宏 | `__LINE__`、`__FILE__` |
| `#line` | 改变行号和文件名 | `#line 500 "virtual.c"` |

## 9.14 练习

1. **修宏**：把 `SQUARE_BAD` 改成正确加括号的版本，测试 `SQUARE(1 + 2)`、`SQUARE((1 + 2))`、`SQUARE(x++)`，解释最后一次为什么危险。
2. **`MAX` 的副作用**：写 `MAX(i++, j++)`，展开宏并在纸上计算，再运行观察；解释为什么不应该这样用。
3. **`MIN`/`MAX`**：用函数宏实现 `MIN` 和 `MAX`，再写 `static inline` 函数版本，比较类型安全和可读性。
4. **`SWAP`**：用 `do { } while (0)` 写 `SWAP(type, a, b)`，测试 `int`、`double` 和指针。
5. **字符串化**：写 `STRINGIFY` 和 `STRINGIFY_RAW` 两层宏，分别打印 `STRINGIFY(VERSION)` 和 `STRINGIFY_RAW(VERSION)`。
6. **记号粘贴**：用 `CONCAT` 生成 `counter1`、`counter2` 两个变量，打印它们。
7. **可变参数**：写 `LOG(fmt, ...)`，测试带一个、两个、三个可变参数的调用；再查 C23 的 `__VA_OPT__`。
8. **include guard**：删掉 `config.h` 的 guard，连续包含两次，观察重复定义错误；恢复 guard。
9. **条件编译**：用 `#if`/`#elif`/`#else` 根据 `LEVEL` 宏打印不同信息，测试 `LEVEL` 为 0、1、2 的情况。
10. **`#error`**：写一个 `#if` 检查 `__STDC_VERSION__`，不满足时用 `#error` 报错；再用 `-std=c17` 编译。
11. **`#line`**：在 `#line 100 "generated.c"` 后打印 `__LINE__` 和 `__FILE__`，再故意制造一个语法错误，观察诊断指向哪里。
12. **头文件**：把 `static inline int square(int)` 放进头文件，在两个 `.c` 文件里调用，确认不会重复定义；再把 `static` 去掉，观察链接结果。

## 9.15 下一章

**第 10 章 · 未定义行为与可移植性**

下一章我们会把前面所有章节里埋下的“地雷”集中起来：有符号溢出、越界、空指针、未初始化读取、严格别名、移位、格式串不匹配，以及如何用编译器警告和 sanitizer 把它们找出来。
