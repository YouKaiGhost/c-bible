# 第 4 章 · 函数与作用域

> **卷一 · 语言**　|　前置知识：第 3 章　|　建议用时：40 分钟

> 函数把“做什么”从“在哪里做”里拆出来；作用域决定一个名字能走多远。

程序长到一定程度，就必须开始分段。函数是最基本的分段工具：它给一段代码起名字，规定输入和输出，让调用者不必关心内部细节。

这一章还会讲**作用域**和**生命期**。它们经常被混为一谈，但其实是两件事：作用域是“名字在哪里可见”，生命期是“对象什么时候存在”。

## 4.1 本章语法地图

| 语法 | 本章出现的位置 |
|------|----------------|
| 函数声明（原型） | `int square(int x);` |
| 函数定义 | `int square(int x) { return x * x; }` |
| 函数调用 | `square(7)` |
| 参数与返回值 | `int add(int a, int b)` |
| `void` | 不返回值、或表示没有参数 |
| `return` | `return value;`、`return;` |
| 递归 | `factorial`、`fib`、`gcd` |
| 块作用域 | `{ int x = 1; }` |
| 文件作用域 | 函数外定义的 `file_scope` |
| 静态局部变量 | `static int id = 0;` |
| 数组参数退化 | `const int a[]` 等价于 `const int *a` |
| `main` 参数 | `int main(int argc, char *argv[])` |

## 4.2 函数声明、定义与调用

函数的使用分三步：

```c
int square(int x);          // 1. 声明：告诉编译器函数的类型

int main(void) {
    int n = square(7);      // 3. 调用
    return 0;
}

int square(int x) {         // 2. 定义：给出函数体
    return x * x;
}
```

### 声明

函数声明也叫**原型**（prototype）。它告诉编译器：

- 函数名；
- 返回类型；
- 参数的类型和个数。

```c
int square(int x);
int square(int);       // 参数名可以省略，类型不能省

void hello(void);      // 明确表示：没有参数
```

原型让编译器在调用点检查参数类型。没有原型时，C 早期版本会做“隐式声明”，这是一种非常危险的遗留行为。C99 起，调用未声明的函数必须被诊断；不要依赖隐式声明。

`(void)` 和空括号 `()` 不一样：在**声明**中，`void f()` 表示“参数未指定”，而 `void f(void)` 明确表示“没有参数”。在函数**定义**中，`void f()` 也表示没有参数，但统一写 `(void)` 可以少一个歧义。

### 定义

定义提供函数体：

```c
int square(int x)
{
    return x * x;
}
```

一个函数可以在多个翻译单元里声明，但真正的定义通常只能有一个。`static` 函数是例外：它只在本翻译单元可见，第 8 章会展开链接规则。

### 调用

```c
int result = square(7);
```

调用时，实参 `7` 被求值，然后按值传给形参 `x`。函数返回一个 `int`，赋给 `result`。

函数调用本身是一个表达式，它的类型就是函数的返回类型。返回 `void` 的函数调用不能作为值使用：

```c
void hello(void);

hello();             // 合法
int x = hello();     // 错误：void 没有值
```

## 4.3 参数是按值传递的

这是 C 函数最重要的规则之一：

> 函数收到的参数是调用者实参的**副本**。

```c
void try_change(int x)
{
    x = 99;
}

int main(void)
{
    int value = 7;
    try_change(value);
    printf("%d\n", value);   // 仍然是 7
}
```

`try_change` 修改的是自己的局部副本 `x`，不是 `main` 里的 `value`。函数返回后，副本消失，`value` 毫发无损。

如果想修改调用者的对象，需要传它的地址；这要用指针，第 5 章会讲。

### 数组参数是一个例外吗？

不完全是。数组作为函数参数时，会**退化**成指向首元素的指针：

```c
void print_array(const int a[], size_t n);
/* 等价于 */
void print_array(const int *a, size_t n);
```

所以在函数内部：

```c
sizeof(a)   // 是指针的大小，不是整个数组的大小
```

真正的数组大小必须由调用者额外传进来，或者用其他方式记录。这也是为什么标准库函数通常要同时接收数组和长度。

`const int a[]` 里的 `const` 约束的是“通过这个指针不能修改元素”，不是“数组不能被修改”。调用者仍然可以在别处修改它。

## 4.4 返回值

`return` 语句结束函数，并把一个值交给调用者：

```c
int add(int a, int b)
{
    return a + b;
}
```

返回值会转换成函数的返回类型：

```c
double half(int x)
{
    return x / 2;        // 先做整数除法，再转换成 double
}
```

上面这个函数很容易写错。`x / 2` 是整数除法，`half(3)` 得到 `1.0`，不是 `1.5`。要写成 `(double)x / 2` 或 `x / 2.0`。

`void` 函数可以不返回值，也可以写 `return;` 提前结束：

```c
void print_positive(int x)
{
    if (x <= 0) {
        return;
    }
    printf("%d\n", x);
}
```

非 `void` 函数如果执行到 `}` 还没有返回值，调用者若使用这个值，行为是未定义。编译器通常会用 `-Wreturn-type` 提醒。`main` 是例外：C99 起，执行到 `}` 等价于返回 0。

## 4.5 递归

函数可以调用自己。递归必须有两个部分：

1. **基本情况**：什么时候停止；
2. **递归步骤**：把问题缩小，再调用自己。

```c
unsigned long long factorial(unsigned int n)
{
    if (n == 0) {
        return 1;
    }
    return (unsigned long long)n * factorial(n - 1);
}
```

`factorial(5)` 展开成 `5 * 4 * 3 * 2 * 1 * 1`。每次调用都有自己的参数 `n` 和返回地址，这些信息放在调用栈上。

递归不是免费的：

- 每层调用都占栈空间；递归太深会栈溢出。
- 有些递归会重复计算。比如朴素的 `fib(n)`：

```c
unsigned long long fib(unsigned int n)
{
    if (n < 2) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}
```

它的调用次数随 `n` 指数增长。`fib(40)` 已经明显变慢，`fib(50)` 在普通机器上可能要等很久。生产代码通常用迭代或记忆化。

递归适合结构本身递归的问题：树、语法分析、分治、欧几里得算法。`gcd` 就是一个漂亮的例子：

```c
int gcd(int a, int b)
{
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}
```

## 4.6 作用域：名字在哪里可见

**作用域**（scope）是名字可以被使用的区域。C 有几种作用域：

- **块作用域**：在 `{ ... }` 内声明的名字，从声明处到块结束可见。
- **文件作用域**：在所有函数外声明的名字，从声明处到文件结束可见。
- **函数作用域**：只有标签（`goto` 的标签）具有函数作用域。
- **函数原型作用域**：原型参数名只在原型内部有效。

```c
int file_scope = 100;          // 文件作用域

void demo(void)
{
    int x = 1;                 // 块作用域
    {
        int x = 2;             // 内层块作用域，遮蔽外层 x
        printf("%d\n", x);     // 2
    }
    printf("%d\n", x);         // 1
}
```

内层 `x` **遮蔽**（shadow）外层 `x`。这不是错误，但容易让人误读。GCC 的 `-Wshadow` 可以提醒你。

### 作用域不等于生命期

```c
int next_id(void)
{
    static int id = 0;   // 名字的作用域是函数块，生命期是整个程序
    return ++id;
}
```

`id` 的名字只在 `next_id` 内可见，但它的存储从程序开始一直存在到程序结束。每次调用 `next_id`，`id` 都保留上一次的值：

```text
next_id() -> 1
next_id() -> 2
next_id() -> 3
```

`static` 局部变量只初始化一次，之后一直保留。它适合做计数器、缓存，但也意味着函数有隐藏状态，不是可重入的，也不是线程安全的。

### 文件作用域与 `static`

```c
int file_scope = 100;          // 外部链接：别的翻译单元也能用
static int file_static = 200;  // 内部链接：只在本翻译单元可见
```

`static` 在文件作用域表示“内部链接”，在块作用域表示“静态存储期”。同一个关键字，两种含义，第 8 章会把链接讲透。

## 4.7 `main` 的参数

`main` 可以写成无参数形式：

```c
int main(void)
```

也可以接收命令行参数：

```c
int main(int argc, char *argv[])
```

- `argc` 是参数个数，**包含程序名本身**。
- `argv` 是一个指向字符串的指针数组。
- `argv[0]` 通常是程序名。
- `argv[1]` 到 `argv[argc - 1]` 是真正的命令行参数。
- `argv[argc]` 保证是空指针。

例如执行：

```sh
./args alpha beta
```

会得到 `argc == 3`，`argv[1]` 是 `"alpha"`，`argv[2]` 是 `"beta"`。

`char *argv[]` 作为参数，等价于 `char **argv`。第 5 章讲指针、第 6 章讲字符串之后，这个类型会变得很自然。

## 4.8 函数声明的位置与顺序

C 编译器从上到下读源文件。函数在调用点之前必须已经声明，否则编译器不知道它的返回类型和参数：

```c
int square(int x);   // 先声明

int main(void)
{
    return square(3);
}

int square(int x)    // 后定义
{
    return x * x;
}
```

原型通常放在文件开头，或者放在头文件里。定义可以放在调用之前，也可以放在之后；只要调用点之前有声明即可。

C 不允许在函数内部定义另一个函数（嵌套函数是 GCC 扩展，不是标准 C）。需要组织代码时，用文件作用域的 `static` 函数。

## 4.9 代码导览

本章有两个程序：

### [`functions.c`](functions.c)

1. `square` 和 `gcd`：最简单的函数调用；
2. `try_change`：证明参数是副本；
3. `factorial`、`fib`：递归和重复计算；
4. `scope_demo`：块作用域、遮蔽、文件作用域；
5. `next_id`：静态局部变量的隐藏状态；
6. `sum_array`、`print_array`：数组参数退化，`sizeof` 在函数内外不同。

编译运行：

```sh
cc -std=c17 -Wall -Wextra -Wpedantic functions.c -o functions
./functions
```

重点观察：

- `try_change` 里面把 `x` 改成 99，外面的 `value` 仍然是 7；
- `next_id` 连续调用得到 1、2、3；
- `sizeof(values)` 是 20，而 `print_array` 里的 `sizeof(a)` 是指针大小（常见为 8）。

### [`args.c`](args.c)

编译后带参数运行：

```sh
cc -std=c17 -Wall -Wextra -Wpedantic args.c -o args
./args alpha beta
```

它会打印 `argc`、`argv[argc] == 0`，以及每个参数。不带参数运行则打印 `argc = 1` 和 `no command-line arguments`。

## 4.10 常见陷阱

| 陷阱 | 说明 |
|------|------|
| 以为函数能修改按值传入的变量 | 参数是副本；要修改调用者对象，传指针。 |
| 在函数里 `sizeof` 数组参数 | 数组参数已退化为指针，得到的是指针大小。 |
| 忘记写原型 | C99 起调用未声明函数要被诊断；不要依赖隐式声明。 |
| 非 `void` 函数不返回值 | 调用者使用返回值时行为未定义；打开 `-Wreturn-type`。 |
| 递归没有基本情况 | 无限递归，最终栈溢出。 |
| 递归重复计算 | 朴素 `fib` 是指数复杂度；用迭代或记忆化。 |
| 以为 `static` 局部变量每次调用都重置 | 它只初始化一次，跨调用保留值。 |
| 以为 `static` 只有一种含义 | 文件作用域是内部链接，块作用域是静态存储期。 |
| 在块里定义同名变量 | 内层遮蔽外层；`-Wshadow` 可以帮助发现。 |
| 认为 `void main()` 是标准写法 | 标准要求 `main` 返回 `int`。 |

## 4.11 语法卡片

| 术语 | 一句话定义 | 本章例子 |
|------|------------|----------|
| 函数声明 | 告诉编译器函数的类型 | `int square(int x);` |
| 函数定义 | 提供函数体 | `int square(int x) { ... }` |
| 形参 | 函数定义中的参数变量 | `int x` |
| 实参 | 调用时传入的表达式 | `square(7)` 里的 `7` |
| 按值传递 | 实参的值复制给形参 | `try_change(value)` |
| 递归 | 函数直接或间接调用自己 | `factorial(n - 1)` |
| 块作用域 | 名字在 `{ ... }` 内可见 | `int x` |
| 文件作用域 | 名字在整个源文件内可见 | `file_scope` |
| 静态局部变量 | 块作用域、静态存储期、只初始化一次 | `static int id` |
| 数组参数退化 | 数组参数等价于指向首元素的指针 | `int a[]` → `int *a` |

## 4.12 练习

1. **三个数最大**：写 `int max3(int a, int b, int c)`，只用 `return` 和一个局部变量实现。
2. **素数**：写 `int is_prime(unsigned int n)`，用循环试除到 `i * i <= n`。注意 `i * i` 的溢出风险。
3. **按值传递**：写 `void swap(int a, int b)`，在函数里交换两个参数，然后在 `main` 里观察调用者的变量没有变化。解释为什么；指针版本留到第 5 章。
4. **递归与迭代**：分别用递归和循环实现阶乘、斐波那契、最大公约数，比较代码和运行时间。
5. **静态计数器**：写 `int next_id(void)`，连续调用三次，打印 1、2、3。再把 `static` 去掉，观察结果。
6. **作用域**：写三层嵌套块，每层定义一个同名 `int x`，打印每一层的值；再用 `-Wshadow` 编译。
7. **数组参数**：写 `int sum(const int a[], size_t n)`，在函数内外分别 `sizeof`，解释差异。
8. **命令行参数**：编译 `args.c`，分别运行 `./args`、`./args alpha`、`./args alpha beta`，解释 `argc` 和 `argv`。
9. **缺少返回值**：写一个非 `void` 函数，在一条路径上不写 `return`，用 `-Wreturn-type` 编译；不要运行依赖该返回值的版本。
10. **函数顺序**：把 `square` 的定义移到 `main` 之后，删除原型，看编译器报什么错；再恢复原型。

## 4.13 下一章

**第 5 章 · 数组与指针**

下一章是 C 语言的分水岭：数组、指针、地址、越界、`const` 指针、函数指针。你会看到为什么“数组名不是指针”，又为什么它在大多数表达式里会退化成指针。
