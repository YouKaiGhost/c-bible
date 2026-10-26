# 附录 A · C11/C17 补遗与 C23 速览

> 主线以 ISO C17 为基线。本附录补齐几个主线没有展开的 C11/C17 特性，并给出 C23 的速览。附录中的代码是示意，不进入 `make` 构建；C23 示例需要支持 C23 的编译器，不能用本仓库的 C17 配置验证。

## A.1 `_Alignas`：指定对齐

C11 的 `_Alignas` 可以给对象指定更严格的对齐：

```c
#include <stdalign.h>

_Alignas(16) unsigned char buffer[64];
```

- `_Alignas(16)` 要求 `buffer` 至少按 16 字节对齐。
- 不能降低类型本身的自然对齐；只能加强或保持一致。
- 只能用于对象声明，不能用于函数、参数或 `typedef`。
- `_Alignof(type)` 查询类型的对齐要求，结果是 `size_t`。
- C11/C17 的 `<stdalign.h>` 提供 `alignas`/`alignof` 宏；C23 起 `alignas`/`alignof` 成为关键字。

```c
printf("%zu\n", _Alignof(double));
```

## A.2 可变长度数组（VLA）

C99 允许数组长度在运行时确定：

```c
void fill(size_t n)
{
    int a[n];          // VLA：长度在运行时确定

    for (size_t i = 0; i < n; ++i) {
        a[i] = (int)i;
    }
}
```

规则与限制：

- VLA 只能有自动存储期，不能是文件作用域或 `static`。
- 不能初始化 VLA：`int a[n] = {0};` 不合法。
- `sizeof(a)` 要在运行时计算。
- 长度必须大于 0；0 或负数不符合约束，不要依赖。
- VLA 通常放在栈上，`n` 很大时可能栈溢出。
- C11 起 VLA 是**可选**特性；实现可以定义 `__STDC_NO_VLA__` 表示不支持。
- C11 起，`goto` 不能跳进 VLA 的作用域。

函数参数也可以使用 VLA 形式：

```c
void sum_rows(size_t rows, size_t cols, int m[rows][cols]);
```

在参数中，第一维仍会退化为指针，但后续维度可以依赖前面的参数。实际项目中，VLA 的使用要谨慎；很多编码规范禁止它，因为栈空间不可控。

## A.3 变参函数

`printf` 是变参函数。定义自己的变参函数需要 `<stdarg.h>`：

```c
#include <stdarg.h>

int sum(int count, ...)
{
    va_list ap;
    int total = 0;

    va_start(ap, count);
    for (int i = 0; i < count; ++i) {
        total += va_arg(ap, int);
    }
    va_end(ap);

    return total;
}
```

调用：

```c
int total = sum(3, 10, 20, 30);   // 60
```

规则：

- `...` 前至少要有一个具名参数；`va_start` 的第二个参数是最后一个具名参数。
- `va_arg(ap, type)` 的 `type` 必须是实参**提升后**的类型。
- 默认实参提升：`char`/`short` → `int`，`float` → `double`。
- 变参函数没有参数个数和类型的编译期检查；调用者和函数必须遵守同一套约定。
- `va_end` 必须调用；复制 `va_list` 用 `va_copy`。
- `va_start` 的最后一个具名参数不能是 `register`、函数类型或数组类型（具体限制见标准）。

`printf` 的格式串就是这种“约定”的典型：编译器能用 `-Wformat` 检查一部分，但运行时完全依赖格式串和实参匹配。

## A.4 复数类型

C99 提供可选的复数类型：

```c
#include <complex.h>

double complex z = 1.0 + 2.0 * I;
double re = creal(z);
double im = cimag(z);
```

- 类型：`float complex`、`double complex`、`long double complex`。
- `<complex.h>` 提供 `I`、`creal`、`cimag`、`cabs`、`carg` 等。
- 复数支持是可选的；C11 的 `__STDC_NO_COMPLEX__` 表示不支持。
- 链接数学函数通常需要 `-lm`。
- `_Imaginary` 是另一个可选特性，支持范围更窄。

复数在普通应用代码里不常见，但它是 C 语言类型系统的一部分。

## A.5 原子与线程

### `_Atomic`

C11 的 `_Atomic` 提供原子类型和原子操作：

```c
#include <stdatomic.h>

_Atomic int counter = 0;

atomic_fetch_add(&counter, 1);
```

- 原子操作不会被数据竞争撕裂；普通 `int` 的并发读写是数据竞争，属于 UB。
- `volatile` 不是原子操作，也不提供内存顺序。
- C11 原子支持是可选的；`__STDC_NO_ATOMICS__` 表示不支持。
- 某些类型可能需要链接 `-latomic`。
- 内存顺序（`memory_order_relaxed`、`acquire`、`release`、`seq_cst`）是并发编程的核心话题，超出本书范围。

### `_Thread_local`

C11 的 `_Thread_local` 给每个线程一份独立对象：

```c
_Thread_local int tls_counter = 0;
```

- 可以和 `static`、`extern` 组合。
- 每个线程看到自己的副本。
- 线程支持本身是可选的；`__STDC_NO_THREADS__` 表示不支持线程库。

### `_Noreturn`

C11 的 `_Noreturn` 表示函数不会返回：

```c
_Noreturn void fatal(const char *message);
```

C23 更推荐属性写法：

```c
[[noreturn]] void fatal(const char *message);
```

`_Noreturn` 在 C23 中被标记为弃用，但为了兼容仍然保留。

## A.6 C23 速览

C23 是 ISO/IEC 9899:2024，`__STDC_VERSION__` 为 `202311L`。下面是常见变化，不是完整清单。

### 关键字与拼写

C23 引入了不带下划线的新拼写（旧拼写通常保留以兼容）：

```c
bool ready = true;
static_assert(sizeof(int) >= 2);
alignas(16) char buffer[64];
thread_local int tls;
```

- `bool`、`true`、`false`：布尔关键字/常量；`<stdbool.h>` 仍可用于兼容。
- `static_assert`：编译期断言。
- `alignas`、`alignof`：对齐说明符/运算符。
- `thread_local`：线程存储期。
- 旧的 `_Static_assert`、`_Alignas`、`_Alignof`、`_Thread_local` 继续保留。

### `nullptr`

```c
int *p = nullptr;
```

C23 引入 `nullptr` 和 `nullptr_t`，作为类型安全的空指针常量。`NULL` 仍然存在。和 `0`/`NULL` 相比，`nullptr` 不会在重载或 `_Generic` 中与整数混淆。

### `constexpr`

C23 的 `constexpr` 用于对象常量：

```c
constexpr int n = 10;
int a[n];
```

它只适用于对象定义，不是 C++ 的 `constexpr` 函数。初始化器必须是常量表达式。

### `auto` 类型推断

C23 改变了 `auto` 的含义：

```c
auto x = 1;      // int
auto y = 2.0;    // double
```

它需要初始值来推断类型。C17 的 `auto` 是存储类说明符，含义完全不同。

### `typeof`

```c
typeof(x) y = x;
typeof_unqual(x) z = x;   // 去掉顶层限定符
```

`typeof` 在 GCC/Clang 里长期作为扩展存在，C23 将其标准化。

### `_BitInt`

```c
_BitInt(17) n = 0;
unsigned _BitInt(17) u = 0;
```

位精确整数类型，宽度由整数常量指定。最大宽度由实现定义。

### 属性

```c
[[nodiscard]] int important(void);
[[deprecated("use new_function")]] void old_function(void);
[[fallthrough]];
[[maybe_unused]] static int debug_only;
[[noreturn]] void fatal(void);
```

C23 标准化了 `[[...]]` 属性语法，常见属性包括 `deprecated`、`fallthrough`、`maybe_unused`、`nodiscard`、`noreturn`、`reproducible`、`unsequenced`。

### 预处理器

```c
#embed "data.bin"
```

- `#embed`：把二进制文件内容嵌入数组初始化。
- `__VA_OPT__`：可变参数宏中处理空参数。
- `#elifdef` / `#elifndef`：条件编译的简写。
- `#warning`：标准化的警告指令。

### 字面量

```c
int mask = 0b1010'1010;
```

- 二进制字面量 `0b...`；
- 数字分隔符 `'`；
- `u8` 字符常量；
- `u8` 字符串字面量的类型改为 `char8_t[]`；
- `char8_t` 是库提供的 UTF-8 字符类型（`<uchar.h>`）。

### 其他变化

- C23 要求有符号整数使用二进制补码；C17 允许补码、反码和符号-数值。
- 旧式函数声明和定义（没有原型）被移除。
- `realloc(p, 0)` 的行为变为未定义。
- 允许空初始化器 `= {}`。
- 标签后可以直接跟声明，也可以出现在块末尾。
- 支持十进制定浮点类型 `_Decimal32`、`_Decimal64`、`_Decimal128`（可选）。

## A.7 特性检测

不要假设某个特性一定存在。用特性测试宏做条件编译：

```c
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    /* C11 或更新 */
#endif

#ifdef __STDC_NO_VLA__
    /* 没有 VLA */
#endif
```

常见宏：

| 宏 | 含义 |
|----|------|
| `__STDC_VERSION__` | 标准版本；C11 是 `201112L`，C17 是 `201710L`，C23 是 `202311L` |
| `__STDC_NO_VLA__` | 不支持可变长度数组 |
| `__STDC_NO_ATOMICS__` | 不支持原子类型 |
| `__STDC_NO_THREADS__` | 不支持线程库 |
| `__STDC_NO_COMPLEX__` | 不支持复数 |
| `__has_include` | 检查头文件是否存在；C23 标准化，部分编译器更早支持 |

## A.8 快速对照表

| 特性 | 标准 | 关键字/头文件 | 备注 |
|------|------|----------------|------|
| `_Alignas` / `alignas` | C11 / C23 | `<stdalign.h>` | 指定对象对齐 |
| `_Atomic` | C11 | `<stdatomic.h>` | 原子类型与操作 |
| `_Thread_local` / `thread_local` | C11 / C23 | — | 线程存储期 |
| `_Noreturn` / `[[noreturn]]` | C11 / C23 | — | 函数不返回 |
| `_Generic` | C11 | — | 编译期类型选择 |
| `_Static_assert` / `static_assert` | C11 / C23 | —（C11 的 `static_assert` 宏在 `<assert.h>`） | 编译期断言 |
| VLA | C99 | — | C11 起可选 |
| 变参函数 | C89 | `<stdarg.h>` | `...` 与 `va_list` |
| 复数 | C99 | `<complex.h>` | 可选 |
| `nullptr` | C23 | `<stddef.h>` | 类型安全的空指针 |
| `constexpr` | C23 | — | 对象常量 |
| `typeof` / `typeof_unqual` | C23 | — | 类型推导 |
| `auto` 类型推断 | C23 | — | 需要初始值 |
| `_BitInt(N)` | C23 | — | 位精确整数 |
| `[[attributes]]` | C23 | — | 标准属性 |
| `#embed` | C23 | — | 嵌入二进制数据 |
| `__VA_OPT__` | C23 | — | 变参宏可选内容 |
| 二进制字面量 / 数字分隔符 | C23 | — | `0b...`、`'` |
| `char8_t` | C23 | `<uchar.h>` | UTF-8 字符类型 |
| 十进制定浮点 | C23 | — | `_Decimal32/64/128`，可选 |

## A.9 回到主线

C23 很新，编译器支持参差不齐。本书的主线仍然以 C17 为基线，因为它足够现代、支持广泛，也能覆盖绝大多数 C 代码。

学习顺序建议：

1. 先把 C17 的核心语言掌握牢固；
2. 遇到 C11/C17 的边界特性时，知道去哪里查；
3. 需要 C23 时，先确认编译器和目标平台支持，再用特性测试宏隔离。

语言的版本会变，但“先问标准保证了什么”的习惯不会过时。
