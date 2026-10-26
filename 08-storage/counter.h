#ifndef COUNTER_H
#define COUNTER_H

/*
 * shared_counter 在 counter.c 中定义，在这里只声明。
 * 这是外部链接：别的翻译单元也能访问同一个对象。
 */
extern int shared_counter;

void bump(void);
int read_counter(void);
int internal_bump_count(void);

/*
 * 下面两个函数在 counter.c 中读取 storage.c 定义的对象，
 * 用来演示跨翻译单元的外部链接。
 */
int read_file_scope(void);
int read_external_const(void);

#endif
