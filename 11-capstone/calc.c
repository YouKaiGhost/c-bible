/*
 * ============================================================================
 *  C 语言圣经 · 第 11 章
 *  综合练习：一个小型表达式求值器
 * ============================================================================
 *
 *  语法：
 *
 *      expression := term (('+' | '-') term)*
 *      term       := factor (('*' | '/') factor)*
 *      factor     := number | '(' expression ')' | '-' factor | '+' factor
 *      number     := digit+ ('.' digit*)? | '.' digit+
 *
 *  支持：
 *      + - * /、括号、一元正负号、十进制小数、空白
 *
 *  不支持：
 *      变量、函数、指数记法、隐式乘法
 *
 *  编译：
 *      cc -std=c17 -Wall -Wextra -Wpedantic calc.c -o calc
 *
 *  运行：
 *      ./calc
 *      ./calc "1 + 2 * 3"
 * ============================================================================
 */

#include <stddef.h>
#include <stdio.h>

struct Parser {
    const char *input;
    const char *cursor;
    const char *error;
    const char *error_at;
};

static void skip_ws(struct Parser *p);
static int is_digit(char c);
static void set_error(struct Parser *p, const char *message);
static double parse_expression(struct Parser *p);
static double parse_term(struct Parser *p);
static double parse_factor(struct Parser *p);
static double parse_number(struct Parser *p);
static int evaluate(const char *text, double *result,
                    const char **error, ptrdiff_t *error_offset);
static void report(const char *text);

int main(int argc, char *argv[])
{
    static const char *const samples[] = {
        "1 + 2 * 3",
        "(1 + 2) * 3",
        "10 / 4",
        "-3 + 5",
        "2 * (3 + 4) - 5",
        "0.1 + 0.2",
        "1 / 0"
    };

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            report(argv[i]);
        }
        return 0;
    }

    printf("== expression evaluator ==\n");
    for (size_t i = 0; i < sizeof samples / sizeof samples[0]; ++i) {
        report(samples[i]);
    }
    printf("\nusage: calc \"1 + 2 * 3\"\n");

    return 0;
}

static void skip_ws(struct Parser *p)
{
    while (*p->cursor == ' ' ||
           *p->cursor == '\t' ||
           *p->cursor == '\n' ||
           *p->cursor == '\r') {
        ++p->cursor;
    }
}

static int is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static void set_error(struct Parser *p, const char *message)
{
    if (p->error == NULL) {
        p->error = message;
        p->error_at = p->cursor;
    }
}

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

        if (op == '+') {
            value += rhs;
        } else {
            value -= rhs;
        }
    }

    return value;
}

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

static double parse_number(struct Parser *p)
{
    double value = 0.0;
    int digits = 0;

    skip_ws(p);

    while (is_digit(*p->cursor)) {
        value = value * 10.0 + (double)(*p->cursor - '0');
        ++p->cursor;
        ++digits;
    }

    if (*p->cursor == '.') {
        double scale = 0.1;

        ++p->cursor;
        while (is_digit(*p->cursor)) {
            value += (double)(*p->cursor - '0') * scale;
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

static void report(const char *text)
{
    double result;
    const char *error;
    ptrdiff_t offset;

    if (evaluate(text, &result, &error, &offset)) {
        printf("%s = %.10g\n", text, result);
    } else {
        printf("%s\n  error at offset %td: %s\n",
               text, offset, error);
    }
}
