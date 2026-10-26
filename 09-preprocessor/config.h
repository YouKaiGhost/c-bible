#ifndef CONFIG_H
#define CONFIG_H

/*
 * 这个头文件被 preprocessor.c 包含两次，
 * 用来演示 include guard 如何阻止重复定义。
 */
#define APP_NAME "C Bible"
#define ENABLE_EXTRA 1

enum {
    CONFIG_MAGIC = 42
};

#endif
