#ifndef UI_H
#define UI_H

#include "lvgl/lvgl.h"

// 全局变量声明
extern lv_obj_t *main_screen_obj;

// 初始化UI界面
void ui_init(void);

// 创建锁屏界面
void create_lock_screen(void);

// 创建主界面
void create_main_screen(void);

#endif

