/*
 * counter.c：与 storage.c 链接成一个程序的另一个翻译单元。
 */

#include "counter.h"

int shared_counter = 0;

/*
 * static 文件作用域：内部链接。
 * 这个对象只在 counter.c 里可见，别的翻译单元不能按名字访问它。
 */
static int internal_bumps = 0;

/*
 * 下面两个对象定义在 storage.c 里；这里只是声明，不分配存储。
 * 链接器会把它们和 storage.c 里的定义连起来。
 */
extern int file_scope;
extern const int external_const;

/*
 * static 函数：内部链接，只在本翻译单元可见。
 */
static void note_bump(void)
{
    ++internal_bumps;
}

void bump(void)
{
    note_bump();
    ++shared_counter;
}

int read_counter(void)
{
    return shared_counter;
}

int internal_bump_count(void)
{
    return internal_bumps;
}

int read_file_scope(void)
{
    return file_scope;
}

int read_external_const(void)
{
    return external_const;
}
