# 第 11 章 · 综合练习：一个小型表达式求值器

> **卷一 · 语言**　|　前置知识：第 0–10 章　|　建议用时：60 分钟

> 这一章不引入新语法。它只把前面的东西拼起来，看看它们能否一起工作。

前面十章分别讲了类型、表达式、控制流、函数、数组、指针、字符串、聚合类型、存储期和预处理器。现在写一个真正能运行的小程序：一个支持 `+ - * /`、括号、一元正负号和十进制小数的表达式求值器。

它的核心是一个**递归下降解析器**（recursive descent parser）。这个算法直接对应文法，结构清晰，是学习“用 C 写解析器”的好起点。

## 11.1 程序目标

输入：

```text
1 + 2 * 3
(1 + 2) * 3
10 / 4
-(2 + 3) * 4
```

输出：

```text
1 + 2 * 3 = 7
(1 + 2) * 3 = 9
10 / 4 = 2.5
-(2 + 3) * 4 = -20
```

支持：

- `+`、`-`、`*`、`/`；
- 括号；
- 一元 `+` 和 `-`；
- 十进制小数（如 `3.14`、`.5`）；
- 空白字符。

不支持：

- 变量、赋值、函数调用；
- 指数记法（`1e3`）；
- 隐式乘法（`2(3)`）；
- `%`、幂运算、比较和逻辑运算；
- 注释。

## 11.2 文法

用 EBNF 写出来：

```text
expression = term { ("+" | "-") term } ;
term       = factor { ("*" | "/") factor } ;
factor     = number
           | "(" expression ")"
           | ("+" | "-") factor ;
number     = digit { digit } [ "." { digit } ]
           | "." digit { digit } ;
```

读法：

- `{ ... }` 表示重复零次或多次；
- `[ ... ]` 表示可选；
- `expression` 是最低优先级；
- `term` 处理乘除；
- `factor` 处理数字、括号和一元正负号。

这个文法已经决定了优先级：

```text
低  expression: + -
    term:       * /
高  factor:     一元 + -、括号、数字
```

括号可以把低优先级表达式包起来，变成高优先级的原子。

## 11.3 递归下降

递归下降的思路是：**为每条文法规则写一个函数**。

```c
double parse_expression(struct Parser *p);
double parse_term(struct Parser *p);
double parse_factor(struct Parser *p);
double parse_number(struct Parser *p);
```

- `parse_expression` 调 `parse_term`，然后循环处理 `+`/`-`；
- `parse_term` 调 `parse_factor`，然后循环处理 `*`/`/`；
- `parse_factor` 遇到 `(` 就递归调用 `parse_expression`；
- `parse_number` 读取数字和小数部分。

左结合的运算符用**循环**处理：

```c
double value = parse_term(p);
while (下一个记号是 '+' 或 '-') {
    double rhs = parse_term(p);
    value = (op == '+') ? value + rhs : value - rhs;
}
```

这样 `1 - 2 - 3` 会算成 `(1 - 2) - 3`，而不是 `1 - (2 - 3)`。

右结合的一元运算符用**递归**处理：

```c
if (*p->cursor == '-') {
    ++p->cursor;
    return -parse_factor(p);
}
```

这样 `--1` 会先解析内层 `-1`，再取负，得到 1。

## 11.4 解析器状态

解析器需要记住三样东西：

```c
struct Parser {
    const char *input;     // 原始字符串，用于计算错误偏移
    const char *cursor;    // 当前读取位置
    const char *error;     // 第一条错误信息
    const char *error_at;  // 错误发生的位置
};
```

- `input` 不变，用于 `error_at - input` 计算偏移；
- `cursor` 随着解析前进；
- `error` 为 `NULL` 表示没有错误；
- `error_at` 记录错误位置，方便报告。

`set_error` 只记录**第一条**错误：

```c
static void set_error(struct Parser *p, const char *message)
{
    if (p->error == NULL) {
        p->error = message;
        p->error_at = p->cursor;
    }
}
```

解析器出错后，上层函数会检查 `p->error` 并停止继续计算。这样错误不会被后续解析覆盖，也不会产生一连串连锁错误。

## 11.5 逐层解析

### `parse_expression`

```c
static double parse_expression(struct Parser *p)
{
    double value = parse_term(p);

    while (p->error == NULL) {
        skip_ws(p);
        char op = *p->cursor;
        if (op != '+' && op != '-') {
            break;
        }
        ++p->cursor;
        double rhs = parse_term(p);
        if (p->error != NULL) {
            return 0.0;
        }
        value = (op == '+') ? value + rhs : value - rhs;
    }
    return value;
}
```

它只关心加减，乘除交给 `parse_term`。遇到不是 `+`/`-` 的字符就停止，让上层决定这个字符是否有意义。

### `parse_term`

```c
static double parse_term(struct Parser *p)
{
    double value = parse_factor(p);

    while (p->error == NULL) {
        skip_ws(p);
        char op = *p->cursor;
        if (op != '*' && op != '/') {
            break;
        }
        const char *op_at = p->cursor;
        ++p->cursor;
        double rhs = parse_factor(p);
        if (p->error != NULL) {
            return 0.0;
        }
        if (op == '*') {
            value *= rhs;
        } else {
            if (rhs == 0.0) {
                p->error = "division by zero";
                p->error_at = op_at;
                return 0.0;
            }
            value /= rhs;
        }
    }
    return value;
}
```

除零在这里检查。因为 `rhs` 是 `double`，`0.0` 和 `-0.0` 都等于 0，都会被拒绝。

### `parse_factor`

```c
static double parse_factor(struct Parser *p)
{
    skip_ws(p);

    if (*p->cursor == '+') {
        ++p->cursor;
        return parse_factor(p);
    }
    if (*p->cursor == '-') {
        ++p->cursor;
        return -parse_factor(p);
    }
    if (*p->cursor == '(') {
        ++p->cursor;
        double value = parse_expression(p);
        if (p->error != NULL) {
            return 0.0;
        }
        skip_ws(p);
        if (*p->cursor != ')') {
            set_error(p, "expected ')'");
            return 0.0;
        }
        ++p->cursor;
        return value;
    }
    return parse_number(p);
}
```

这里体现了递归：括号里可以是一个完整的 `expression`，所以 `parse_factor` 又调回 `parse_expression`。递归的终止条件是数字或错误。

### `parse_number`

```c
static double parse_number(struct Parser *p)
{
    double value = 0.0;
    int digits = 0;

    skip_ws(p);

    while (is_digit(*p->cursor)) {
        value = value * 10.0 + (*p->cursor - '0');
        ++p->cursor;
        ++digits;
    }

    if (*p->cursor == '.') {
        double scale = 0.1;
        ++p->cursor;
        while (is_digit(*p->cursor)) {
            value += (*p->cursor - '0') * scale;
            scale *= 0.1;
            ++p->cursor;
            ++digits;
        }
    }

    if (digits == 0) {
        set_error(p, "expected a number");
        return 0.0;
    }
    return value;
}
```

这是手写的十进制浮点解析，没有用 `strtod`。它故意保持简单：

- 不支持指数；
- 不支持千位分隔符；
- 小数部分的精度受 `double` 限制；
- 超过 `double` 范围时没有额外检查。

## 11.6 入口与错误报告

`evaluate` 负责一次完整求值：

```c
static int evaluate(const char *text, double *result,
                    const char **error, ptrdiff_t *error_offset)
{
    struct Parser p = {text, text, NULL, NULL};
    double value = parse_expression(&p);

    if (p.error == NULL) {
        skip_ws(&p);
        if (*p.cursor != '\0') {
            set_error(&p, "unexpected trailing input");
        }
    }

    if (p.error != NULL) {
        *error = p.error;
        *error_offset = p.error_at - p.input;
        return 0;
    }

    *result = value;
    return 1;
}
```

它做了两件事：

1. 解析表达式；
2. 检查是否还有剩余字符。

第二步很重要。没有它，`1e3` 会被解析成 `1`，然后悄悄忽略 `e3`。现在它会报告 `unexpected trailing input`，错误位置指向 `e`。

`report` 打印结果或错误：

```c
if (evaluate(text, &result, &error, &offset)) {
    printf("%s = %.10g\n", text, result);
} else {
    printf("%s\n  error at offset %td: %s\n",
           text, offset, error);
}
```

`%.10g` 最多打印 10 位有效数字。浮点结果是近似值，不要期待无限精度。

## 11.7 编译与运行

在本章目录：

```sh
cc -std=c17 -Wall -Wextra -Wpedantic calc.c -o calc
./calc
```

不带参数时会跑一组内置示例：

```text
== expression evaluator ==
1 + 2 * 3 = 7
(1 + 2) * 3 = 9
10 / 4 = 2.5
-3 + 5 = 2
2 * (3 + 4) - 5 = 9
0.1 + 0.2 = 0.3
1 / 0
  error at offset 2: division by zero

usage: calc "1 + 2 * 3"
```

带参数时一次求值一个或多个表达式：

```sh
./calc "1 + 2 * 3" "-(2 + 3) * 4"
```

输出：

```text
1 + 2 * 3 = 7
-(2 + 3) * 4 = -20
```

注意 shell 的引号：表达式里有空格和括号，必须作为一个参数传给程序。

## 11.8 已知限制

这个求值器是教学程序，不是生产计算器：

- 只支持 `double`，没有精确十进制或任意精度；
- 没有变量、赋值、函数调用；
- 没有指数记法和隐式乘法；
- 没有溢出、NaN、无穷大的显式检查；
- 没有 tokenizer，字符扫描和解析混在一起；
- 递归深度受调用栈限制，恶意深层括号可以栈溢出；
- 错误只报告第一条，不做恢复；
- 空白只识别 ASCII 的空格、制表符、回车和换行；
- 小数点是 `.`，不处理 locale。

这些限制不是缺陷，而是边界。知道边界，才能安全地扩展它。

## 11.9 这个程序用到了什么

| 章节 | 用到的内容 |
|------|------------|
| 第 1 章 | `double`、`int`、`size_t`、`ptrdiff_t`、显式转换 |
| 第 2 章 | 算术、关系、逻辑、三元运算符、优先级 |
| 第 3 章 | `if`、`while`、`break`、`return` |
| 第 4 章 | 函数、递归、参数、返回值 |
| 第 5 章 | 指针、字符串游标、`const char *` |
| 第 6 章 | 字符串字面量、字符判断、`argv` |
| 第 7 章 | `struct Parser` |
| 第 8 章 | `static` 函数、局部对象生命期 |
| 第 9 章 | 代码组织、头文件思想（本章单文件） |
| 第 10 章 | 输入检查、除零检查、边界意识 |

它没有用 `switch`、`enum`、`union` 和位域。你可以把它们作为练习加进来。

## 11.10 练习与扩展

1. **指数记法**：支持 `1e3`、`2.5e-2`。注意 `e` 后面可以跟正负号。
2. **变量**：支持 `x = 3` 和后续表达式引用 `x`。需要一个符号表（`struct` 数组）和赋值语句的语法。
3. **函数**：支持 `sqrt(9)`、`abs(-3)`。需要函数名、参数列表和标准库数学函数。
4. **取余**：增加 `%`，但只允许整数操作数。考虑类型系统怎么变复杂。
5. **幂运算**：增加 `^`，注意它是右结合，并且优先级高于乘除、低于一元负号（或按你选择的规则）。
6. **Tokenizer**：把字符扫描拆成 `enum TokenType` + `struct Token`，让解析器消费 token 流。这是更正规的写法。
7. **错误恢复**：一次报告多个错误，而不是遇到第一条就停止。
8. **测试**：为 `evaluate` 写一组断言，覆盖优先级、括号、负数、小数、除零、缺少括号、尾随字符。
9. **多文件**：把 `Parser`、解析函数、输入输出拆成 `.h`/`.c`，用第 8 章的链接知识组织。
10. **`switch`/`enum`**：用 `enum Error` 代替字符串错误信息，用 `switch` 分派错误码。
11. **交互模式**：从标准输入逐行读取表达式并求值；注意缓冲区边界和 `fgets` 的返回值。
12. **浮点边界**：检查 `isfinite`、`isnan`，处理溢出和无穷大；思考这些检查应该放在哪一层。

## 11.11 结束语

这套教材从一行 `Hello, World!` 开始，到这里已经覆盖了 C17 的核心语言特性：类型、表达式、控制流、函数、数组、指针、字符串、结构体、联合体、枚举、存储期、链接、预处理器，以及未定义行为。

但 C 的学习不会在“读完语法”时结束。真正的进步来自三件事：

1. **读标准**：遇到不确定的地方，查 C 标准或可靠的参考资料，而不是猜。
2. **读代码**：看成熟项目如何组织接口、处理错误、写测试。
3. **写代码**：把每一章的练习真的做一遍，再故意改坏它们。

如果这套教材只能留下一个习惯，希望是：**在看到“应该没问题”时，再问一句“标准保证了什么？”**
