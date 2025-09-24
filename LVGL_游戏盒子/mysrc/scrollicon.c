#include "lvgl/lvgl.h"
#include "../inc/lv_font38.h"
#include "qieshuiguo.h"
#include "ui.h"
#include "2048.h"
#include "mplayer/inc/lvgl_player.h"
#include "mplayer/inc/head.h"

// 图标数量
#define icon_count 6
// 图标之间的距离
#define icon_distance 220
// 小图标的尺寸
#define icon_size_small 120
// 大图标的尺寸
#define icon_size_big 200

// 定义图标结构体，包含图标对象指针和x坐标
typedef struct
{
    lv_obj_t *obj;
    int32_t x;
} icon_typedef;

// 声明图标结构体数组
static icon_typedef icon[icon_count];
// 触摸状态标志
static bool touched = false;
// 屏幕宽度
static int32_t scr_w;
// 触摸偏移量x
static int32_t t_offset_x;
// 屏幕对象指针
static lv_obj_t *screen;
// 使用全局视频播放器变量 player
extern VideoPlayer player;

// 按下事件回调函数声明
static void pressing_cb(lv_event_t *e);
// 释放事件回调函数声明
static void released_cb(lv_event_t *e);
// 设置x坐标回调函数声明
static void set_x_cb(void *var, int32_t v);
// 切水果游戏图标点击事件回调函数声明
static void fruit_game_icon_cb(lv_event_t *e);
// 2048游戏图标点击事件回调函数声明
static void game_2048_icon_cb(lv_event_t *e);
// 视频播放器图标点击事件回调函数声明
static void mplayer_icon_cb(lv_event_t *e);
// 自定义动画创建函数声明
static void lv_myanim_creat(void *var, lv_anim_exec_xcb_t exec_cb, uint32_t time, uint32_t delay, lv_anim_path_cb_t path_cb,
                            int32_t start, int32_t end, lv_anim_ready_cb_t completed_cb, lv_anim_deleted_cb_t deleted_cb);

// 滚动图标功能函数
void scrollicon()
{
    int32_t i;

    // 获取默认显示器的垂直分辨率（这里可能是获取屏幕宽度的误写，根据后续使用推测）
    scr_w = lv_disp_get_ver_res(lv_disp_get_default());
    // 初始化图标结构体数组为0
    lv_memset(icon, 0, sizeof(icon));

    // 获取当前活动屏幕
    lv_obj_t *active_screen = lv_scr_act();

    // 在当前活动屏幕上创建一个容器用于放置滚动图标
    screen = lv_obj_create(active_screen);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(70));
    lv_obj_align(screen, LV_ALIGN_CENTER, 0, 0);
    // 设置背景为透明，以显示主界面的背景图片
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    // 为屏幕添加按下事件回调函数
    lv_obj_add_event_cb(screen, pressing_cb, LV_EVENT_PRESSING, 0);
    // 为屏幕添加释放事件回调函数
    lv_obj_add_event_cb(screen, released_cb, LV_EVENT_RELEASED, 0);

    for (i = 0; i < icon_count; i++)
    {
        // 创建图标对象并添加到屏幕上
        icon[i].obj = lv_obj_create(screen);
        // 设置图标对象的用户数据为对应的图标结构体指针
        icon[i].obj->user_data = &icon[i];
        // 为不同的图标设置不同的背景颜色
        switch (i % 6)
        {
        case 0:
            lv_obj_set_style_bg_color(icon[i].obj, lv_color_hex(0xff0000), LV_PART_MAIN);
            break; // 红色
        case 1:
        {
            // 创建图片对象并设置为背景
            lv_obj_t *img1 = lv_image_create(icon[i].obj); // 将图标对象作为父对象
            lv_image_set_src(img1, "A:/wxj/video/videotb.png");
            // 确保父对象不会传递滚动事件
            lv_obj_clear_flag(icon[i].obj, LV_OBJ_FLAG_SCROLLABLE);
            // 将图片居中于父对象
            lv_obj_center(img1);
            break;
        } // 视频播放器图标
        case 2:
        {
            // 创建图片对象并设置为背景
            lv_obj_t *img2 = lv_image_create(icon[i].obj); // 将图标对象作为父对象
            lv_image_set_src(img2, "A:/wxj/2048/2048tb.png");
            // 确保父对象不会传递滚动事件
            lv_obj_clear_flag(icon[i].obj, LV_OBJ_FLAG_SCROLLABLE);
            // 将图片居中于父对象
            lv_obj_center(img2);
            break;
        }
        case 3:
        {
            // 创建图片对象并设置为背景
            lv_obj_t *img3 = lv_image_create(icon[i].obj); // 将图标对象作为父对象
            lv_image_set_src(img3, "A:/wxj/qsg/qsgtb.png");
            // 确保父对象不会传递滚动事件
            lv_obj_clear_flag(icon[i].obj, LV_OBJ_FLAG_SCROLLABLE);
            // 将图片居中于父对象
            lv_obj_center(img3);
            break;
        }
        case 4:
            lv_obj_set_style_bg_color(icon[i].obj, lv_color_hex(0xff00ff), LV_PART_MAIN);
            break; // 紫色
        case 5:
            lv_obj_set_style_bg_color(icon[i].obj, lv_color_hex(0x00ffff), LV_PART_MAIN);
            break; // 青色
        }

        // // 在图标对象上创建一个标签
        // lv_obj_t *l = lv_label_create(icon[i].obj);
        // lv_obj_set_style_text_font(l, &lv_font38, 0);

        // // 为不同的图标设置不同的应用名称
        // switch (i % 6)
        // {
        // case 0:
        //     lv_label_set_text(l, "11");
        //     break;
        // case 1:
        //     lv_label_set_text(l, "22");
        //     break;
        // case 2:
        //     lv_label_set_text(l, "33");
        //     break;
        // case 3:
        //     lv_label_set_text(l, "切水果");
        //     break;
        // case 4:
        //     lv_label_set_text(l, "44");
        //     break;
        // case 5:
        //     lv_label_set_text(l, "55");
        //     break;
        // }
        // 计算图标x坐标
        icon[i].x = (i - icon_count / 2) * icon_distance;
        // 将图标居中显示
        lv_obj_center(icon[i].obj);
        // 如果是中间的图标，设置为大尺寸
        if (i == icon_count / 2)
        {
            lv_obj_set_size(icon[i].obj, icon_size_big, icon_size_big);
        }
        else
        {
            // 否则设置为小尺寸
            lv_obj_set_size(icon[i].obj, icon_size_small, icon_size_small);
        }

        // 设置图标x坐标
        lv_obj_set_x(icon[i].obj, icon[i].x);
        // 为图标添加按下事件回调函数
        lv_obj_add_event_cb(icon[i].obj, pressing_cb, LV_EVENT_PRESSING, 0);
        // 为图标添加释放事件回调函数
        lv_obj_add_event_cb(icon[i].obj, released_cb, LV_EVENT_RELEASED, 0);

        // 为切水果图标添加点击事件
        if (i % 6 == 3)
        {
            lv_obj_add_event_cb(icon[i].obj, fruit_game_icon_cb, LV_EVENT_CLICKED, NULL);
        }
        // 为2048图标添加点击事件
        if (i % 6 == 2)
        {
            lv_obj_add_event_cb(icon[i].obj, game_2048_icon_cb, LV_EVENT_CLICKED, NULL);
        }
        // 为视频播放器图标添加点击事件
        if (i % 6 == 1)
        {
            lv_obj_add_event_cb(icon[i].obj, mplayer_icon_cb, LV_EVENT_CLICKED, NULL);
        }
    }
}

// 按下事件回调函数
static void pressing_cb(lv_event_t *e)
{
    static lv_point_t click_point1, click_point2;
    static uint32_t last_process_time = 0;
    uint32_t current_time = lv_tick_get();
    int32_t v, i;

    // 限制处理频率，降低CPU使用率（每16ms处理一次，约60fps）
    if (current_time - last_process_time < 16 && touched)
    {
        return;
    }
    last_process_time = current_time;

    // 如果当前未处于触摸状态
    if (touched == false)
    {
        for (i = 0; i < icon_count; i++)
        {
            // 删除图标对象上的动画（如果有）
            lv_anim_del(icon[i].obj, set_x_cb);
        }

        // 获取当前输入设备的点击点坐标
        lv_indev_get_point(lv_indev_get_act(), &click_point1);
        // 设置触摸状态为已触摸
        touched = true;
        return;
    }
    else
    {
        // 如果已经处于触摸状态，获取当前点击点坐标
        lv_indev_get_point(lv_indev_get_act(), &click_point2);
    }

    // 计算触摸偏移量x
    t_offset_x = click_point2.x - click_point1.x;
    // 更新上一次点击点坐标
    click_point1.x = click_point2.x;

    // 只在有明显移动时更新图标位置
    if (abs(t_offset_x) > 0)
    {
        for (int32_t i = 0; i < icon_count; i++)
        {
            // 更新图标x坐标
            icon[i].x += t_offset_x;
            // 处理图标x坐标超出范围的情况（循环滚动）
            if (icon[i].x < (-icon_count / 2) * icon_distance)
            {
                icon[i].x += (icon_count)*icon_distance;
            }
            else if (icon[i].x > (icon_count / 2) * icon_distance)
            {
                icon[i].x -= (icon_count)*icon_distance;
            }
            // 设置图标对象的x坐标
            lv_obj_set_x(icon[i].obj, icon[i].x);

            // 如果图标x坐标超出一定范围，设置为小尺寸
            if (icon[i].x >= icon_distance || icon[i].x <= -icon_distance)
            {
                lv_obj_set_size(icon[i].obj, icon_size_small, icon_size_small);
                continue;
            }

            // 根据x坐标计算图标尺寸
            if (icon[i].x >= 0)
            {
                v = icon[i].x;
            }
            else
            {
                v = -icon[i].x;
            }

            // 使用整数运算代替浮点运算，提高性能
            int32_t size_diff = icon_size_big - icon_size_small;
            int32_t size_factor = (icon_distance - v) * size_diff / icon_distance;
            int32_t new_size = icon_size_small + size_factor;

            // 只在尺寸变化超过一定阈值时才更新，减少不必要的重绘
            static int32_t last_size[icon_count] = {0};
            if (abs(new_size - last_size[i]) > 2)
            {
                lv_obj_set_size(icon[i].obj, new_size, new_size);
                last_size[i] = new_size;
            }
        }
    }
}

// 释放事件回调函数
static void released_cb(lv_event_t *e)
{
    int32_t offset_x;
    offset_x = 0;
    // 设置触摸状态为未触摸
    touched = false;

    for (int32_t i = 0; i < icon_count; i++)
    {
        // 如果图标x坐标大于0
        if (icon[i].x > 0)
        {
            // 根据x坐标与图标距离的关系计算偏移量
            if (icon[i].x % icon_distance > icon_distance / 2)
            {
                offset_x = icon_distance - icon[i].x % icon_distance;
            }
            else
            {
                offset_x = -icon[i].x % icon_distance;
            }
            break;
        }
    }

    for (int32_t i = 0; i < icon_count; i++)
    {
        // 创建动画，使图标回到合适位置
        lv_myanim_creat(icon[i].obj, set_x_cb, t_offset_x > 0 ? 300 + t_offset_x * 5 : 300 - t_offset_x * 5, 0, lv_anim_path_ease_out, icon[i].x, icon[i].x + offset_x + t_offset_x / 20 * icon_distance, 0, 0);
        // 更新图标x坐标
        icon[i].x += offset_x + t_offset_x / 20 * icon_distance;
        // 处理图标x坐标超出范围的情况（循环滚动）
        while (icon[i].x < (-icon_count / 2) * icon_distance)
        {
            icon[i].x += (icon_count)*icon_distance;
        }
        while (icon[i].x > (icon_count / 2) * icon_distance)
        {
            icon[i].x -= (icon_count)*icon_distance;
        }
    }
}

// 自定义动画创建函数
static void lv_myanim_creat(void *var, lv_anim_exec_xcb_t exec_cb, uint32_t time, uint32_t delay, lv_anim_path_cb_t path_cb,
                            int32_t start, int32_t end, lv_anim_ready_cb_t completed_cb, lv_anim_deleted_cb_t deleted_cb)
{
    lv_anim_t xxx;
    // 初始化动画对象
    lv_anim_init(&xxx);
    // 设置动画对象的变量
    lv_anim_set_var(&xxx, var);
    // 设置动画执行回调函数
    lv_anim_set_exec_cb(&xxx, exec_cb);
    // 设置动画时间
    lv_anim_set_time(&xxx, time);
    // 设置动画延迟
    lv_anim_set_delay(&xxx, delay);
    // 设置动画的起始值和结束值
    lv_anim_set_values(&xxx, start, end);
    // 如果有路径回调函数，设置路径回调函数
    if (path_cb)
        lv_anim_set_path_cb(&xxx, path_cb);
    // 如果有动画完成回调函数，设置动画完成回调函数
    if (completed_cb)
        lv_anim_set_ready_cb(&xxx, completed_cb);
    // 如果有动画删除回调函数，设置动画删除回调函数
    if (deleted_cb)
        lv_anim_set_deleted_cb(&xxx, deleted_cb);
    // 启动动画
    lv_anim_start(&xxx);
}

// 设置x坐标回调函数
static void set_x_cb(void *var, int32_t v)
{
    // 处理x坐标超出范围的情况（循环滚动）
    while (v < (-icon_count / 2) * icon_distance)
    {
        v += (icon_count)*icon_distance;
    }
    while (v > (icon_count / 2) * icon_distance)
    {
        v -= (icon_count)*icon_distance;
    }

    // 设置对象的x坐标
    lv_obj_set_x(var, v);

    // 获取图标结构体指针
    icon_typedef *xxx = (icon_typedef *)(((lv_obj_t *)var)->user_data);
    // 更新图标结构体中的x坐标
    xxx->x = v;

    // 如果x坐标为0，设置图标为大尺寸
    if (v == 0)
    {
        lv_obj_set_size(var, icon_size_big, icon_size_big);
        return;
    }

    // 如果x坐标超出一定范围，设置图标为小尺寸
    if (v >= icon_distance || v <= -icon_distance)
    {
        lv_obj_set_size(var, icon_size_small, icon_size_small);
        return;
    }

    // 如果x坐标小于0，取绝对值
    if (v < 0)
        v = -v;

    // 根据x坐标计算图标尺寸并设置
    lv_obj_set_size(var, icon_size_small + (float)(icon_distance - v) / (float)icon_distance * (icon_size_big - icon_size_small), icon_size_small + (float)(icon_distance - v) / (float)icon_distance * (icon_size_big - icon_size_small));
}

// 切水果游戏图标点击事件回调函数
static void fruit_game_icon_cb(lv_event_t *e)
{
    // 调用切水果游戏
    qieshuiguo();
}

// 2048游戏图标点击事件回调函数
static void game_2048_icon_cb(lv_event_t *e)
{
    // 调用2048游戏
    game_2048();
}

// 视频播放器图标点击事件回调函数
static void mplayer_icon_cb(lv_event_t *e)
{
    // 初始化播放器界面
    init_player_ui(&player);
    
    // 更新播放列表显示
    update_playlist(&player);
    
    // 启动第一个视频
    start_mplayer(&player);
}