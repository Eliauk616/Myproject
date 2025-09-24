#include "lvgl/lvgl.h"
#include "../../lv_mygec_font.h"
#include "ui.h"
#include "scrollicon.h"
#include <time.h>
#include <stdio.h>

// 全局变量，用于存储屏幕对象
static lv_obj_t *lock_screen_obj = NULL;
lv_obj_t *main_screen_obj = NULL; // 改为全局变量，以便其他文件可以访问
static lv_obj_t *slider_obj = NULL;
static lv_obj_t *time_label = NULL;      // 时间标签对象
static lv_obj_t *date_label = NULL;      // 日期标签对象
static lv_obj_t *main_time_label = NULL; // 主界面时间标签对象

// 更新时间的定时器回调函数
static void update_time(lv_timer_t *timer)
{
    time_t now;
    struct tm *time_info;
    char time_str[16];
    char date_str[32];

    // 获取当前时间
    time(&now);
    time_info = localtime(&now);

    // 格式化时间字符串
    strftime(time_str, sizeof(time_str), "%H:%M:%S", time_info);

    // 格式化日期字符串
    strftime(date_str, sizeof(date_str), "%Y年%m月%d日 星期%w", time_info);
    // 将星期数字转换为中文
    if (date_str[strlen(date_str) - 1] == '0')
    {
        strcpy(date_str + strlen(date_str) - 1, "日");
    }
    else if (date_str[strlen(date_str) - 1] == '1')
    {
        strcpy(date_str + strlen(date_str) - 1, "一");
    }
    else if (date_str[strlen(date_str) - 1] == '2')
    {
        strcpy(date_str + strlen(date_str) - 1, "二");
    }
    else if (date_str[strlen(date_str) - 1] == '3')
    {
        strcpy(date_str + strlen(date_str) - 1, "三");
    }
    else if (date_str[strlen(date_str) - 1] == '4')
    {
        strcpy(date_str + strlen(date_str) - 1, "四");
    }
    else if (date_str[strlen(date_str) - 1] == '5')
    {
        strcpy(date_str + strlen(date_str) - 1, "五");
    }
    else if (date_str[strlen(date_str) - 1] == '6')
    {
        strcpy(date_str + strlen(date_str) - 1, "六");
    }

    // 更新时间显示
    if (time_label)
    {
        lv_label_set_text(time_label, time_str);
    }

    // 更新主界面时间显示
    if (main_time_label)
    {
        lv_label_set_text(main_time_label, time_str);
    }

    // 更新日期显示
    if (date_label)
    {
        lv_label_set_text(date_label, date_str);
    }
}

// 滑块事件回调函数
static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);
    int32_t max_value = lv_slider_get_max_value(slider);

    // 当滑块滑动到最右侧时，解锁屏幕
    if (value >= max_value - 5)
    { // 留一点误差范围
        // 切换到主屏幕
        lv_screen_load(main_screen_obj);
        // 重置滑块位置
        lv_slider_set_value(slider, 0, LV_ANIM_OFF);
    }
}

// 创建锁屏界面
void create_lock_screen(void)
{
    // 创建锁屏屏幕
    lock_screen_obj = lv_obj_create(NULL);

    // 创建图片对象并设置为背景
    lv_obj_t *bg_obj = lv_image_create(lock_screen_obj);
    lv_image_set_src(bg_obj, "A:/wxj/bg.bmp");

    // 调整大小以覆盖整个屏幕
    lv_obj_set_width(bg_obj, LV_PCT(100));
    lv_obj_set_height(bg_obj, LV_PCT(100));
    lv_obj_center(bg_obj);

    // 将图片移至底层，确保其他控件在其上方
    lv_obj_move_background(bg_obj);

    // 添加半透明遮罩层，使前景内容更清晰
    lv_obj_t *overlay = lv_obj_create(lock_screen_obj);
    lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
    lv_obj_center(overlay);
    lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(overlay, LV_OPA_30, 0);
    lv_obj_set_style_border_width(overlay, 0, 0);
    lv_obj_move_background(overlay); // 确保遮罩在图片上方，其他控件下方

    // 创建时间标签
    time_label = lv_label_create(lock_screen_obj);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_font(time_label, &lv_mygec_font, 0);
    lv_label_set_text(time_label, "获取时间中...");
    lv_obj_align(time_label, LV_ALIGN_TOP_MID, 0, 50);

    // 创建日期标签
    date_label = lv_label_create(lock_screen_obj);
    lv_obj_set_style_text_color(date_label, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_font(date_label, &lv_mygec_font, 0);
    lv_label_set_text(date_label, "获取日期中...");
    lv_obj_align(date_label, LV_ALIGN_TOP_MID, 0, 100);

    // 创建定时器，每秒更新一次时间
    lv_timer_t *timer = lv_timer_create(update_time, 1000, NULL);
    // 立即更新一次时间
    update_time(NULL);

    // 创建解锁提示标签
    lv_obj_t *unlock_label = lv_label_create(lock_screen_obj);
    lv_obj_set_style_text_color(unlock_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(unlock_label, &lv_mygec_font, 0);
    lv_label_set_text(unlock_label, "滑动解锁");
    lv_obj_align(unlock_label, LV_ALIGN_BOTTOM_MID, 0, -100);

    // 创建滑块用于解锁
    slider_obj = lv_slider_create(lock_screen_obj);
    lv_obj_set_width(slider_obj, 400);
    lv_obj_align(slider_obj, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_slider_set_range(slider_obj, 0, 100);
    lv_slider_set_value(slider_obj, 0, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_obj, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 滑块整体高度
    lv_obj_set_height(slider_obj, 120);

    // 主轨道样式
    lv_obj_set_style_bg_color(slider_obj, lv_color_hex(0x2C3E50), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(slider_obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(slider_obj, lv_color_hex(0x34495E), LV_PART_MAIN);
    lv_obj_set_style_border_width(slider_obj, 3, LV_PART_MAIN); // 边框宽度
    lv_obj_set_style_radius(slider_obj, 20, LV_PART_MAIN);      // 圆角
    lv_obj_set_style_height(slider_obj, 20, LV_PART_MAIN);      // 主轨道高度

    // 进度指示器样式
    lv_obj_set_style_bg_color(slider_obj, lv_color_hex(0x3498DB), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(slider_obj, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider_obj, 20, LV_PART_INDICATOR); // 圆角

    // 滑块按钮样式
    lv_obj_set_style_bg_color(slider_obj, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(slider_obj, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_border_color(slider_obj, lv_color_hex(0x3498DB), LV_PART_KNOB);
    lv_obj_set_style_border_width(slider_obj, 3, LV_PART_KNOB); // 边框宽度
    lv_obj_set_style_radius(slider_obj, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_width(slider_obj, 70, LV_PART_KNOB);  // 滑块按钮宽度
    lv_obj_set_style_height(slider_obj, 70, LV_PART_KNOB); // 滑块按钮高度

    // 滑块按钮阴影效果
    lv_obj_set_style_shadow_width(slider_obj, 15, LV_PART_KNOB); // 阴影宽度
    lv_obj_set_style_shadow_color(slider_obj, lv_color_hex(0x3498DB), LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(slider_obj, LV_OPA_70, LV_PART_KNOB); // 阴影不透明度
    lv_obj_set_style_shadow_spread(slider_obj, 3, LV_PART_KNOB);      // 阴影扩散
}

// 锁屏按钮事件回调函数
static void lock_btn_event_cb(lv_event_t *e)
{
    lv_screen_load(lock_screen_obj);
}

// 创建主界面
void create_main_screen(void)
{
    // 创建主屏幕
    main_screen_obj = lv_obj_create(NULL);

    // 设置背景图片
    lv_obj_t *bg_img = lv_image_create(main_screen_obj);
    lv_image_set_src(bg_img, "A:/wxj/fll.bmp");
    // 设置图片大小与屏幕完全一致
    lv_obj_set_width(bg_img, lv_disp_get_hor_res(NULL));
    lv_obj_set_height(bg_img, lv_disp_get_ver_res(NULL));
    lv_obj_align(bg_img, LV_ALIGN_CENTER, 0, 0);
    lv_obj_move_background(bg_img); // 将图片移至底层

    // 创建顶部状态栏
    lv_obj_t *status_bar = lv_obj_create(main_screen_obj);
    lv_obj_set_size(status_bar, LV_PCT(100), 40);
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(status_bar, lv_color_hex(0x34495e), 0);
    lv_obj_set_style_border_width(status_bar, 0, 0);
    lv_obj_set_style_radius(status_bar, 0, 0); // 角弧度
    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);

    // 创建标题标签
    lv_obj_t *title_label = lv_label_create(status_bar);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(title_label, &lv_mygec_font, 0);
    lv_label_set_text(title_label, "主界面");
    lv_obj_align(title_label, LV_ALIGN_LEFT_MID, 10, 0);

    // 创建时间标签（主界面）
    main_time_label = lv_label_create(status_bar);
    lv_obj_set_style_text_color(main_time_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(main_time_label, &lv_mygec_font, 0);
    lv_label_set_text(main_time_label, "00:00:00");
    lv_obj_align(main_time_label, LV_ALIGN_RIGHT_MID, -10, 0);

    // 创建底部导航栏
    lv_obj_t *nav_bar = lv_obj_create(main_screen_obj);
    lv_obj_set_size(nav_bar, LV_PCT(100), 60);
    lv_obj_align(nav_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(nav_bar, lv_color_hex(0x34495e), 0);
    lv_obj_set_style_border_width(nav_bar, 0, 0);
    lv_obj_set_style_radius(nav_bar, 0, 0); // 角弧度
    lv_obj_clear_flag(nav_bar, LV_OBJ_FLAG_SCROLLABLE);

    // 创建返回锁屏按钮
    lv_obj_t *lock_btn = lv_button_create(nav_bar);
    lv_obj_set_size(lock_btn, 100, 50);
    lv_obj_set_style_radius(lock_btn, 20, 0);
    lv_obj_set_style_bg_color(lock_btn, lv_color_hex(0x2c3e50), 0);
    lv_obj_align(lock_btn, LV_ALIGN_CENTER, 0, 0); // 底部导航栏的居中位置

    // 按钮点击事件
    lv_obj_add_event_cb(lock_btn, lock_btn_event_cb, LV_EVENT_CLICKED, NULL);

    // 创建锁屏图标
    lv_obj_t *lock_icon = lv_label_create(lock_btn);
    lv_obj_set_style_text_color(lock_icon, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lock_icon, &lv_mygec_font, 0);
    lv_label_set_text(lock_icon, "锁屏");
    lv_obj_center(lock_icon);

    // 创建主内容区域
    lv_obj_t *main_content = lv_obj_create(main_screen_obj);
    lv_obj_set_size(main_content, LV_PCT(100), LV_PCT(75));
    lv_obj_align(main_content, LV_ALIGN_CENTER, 0, 0);
    // 设置背景为透明，以显示背景图片
    lv_obj_set_style_bg_opa(main_content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(main_content, 0, 0);
    lv_obj_clear_flag(main_content, LV_OBJ_FLAG_SCROLLABLE);

    // 创建欢迎信息
    lv_obj_t *welcome_label = lv_label_create(main_content);
    lv_obj_set_style_text_color(welcome_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_text_font(welcome_label, &lv_mygec_font, 0);
    lv_label_set_text(welcome_label, "欢迎进入主界面");
    lv_obj_align(welcome_label, LV_ALIGN_TOP_MID, 0, 20);

    // 设置当前屏幕为活动屏幕，以便在scrollicon函数中使用
    lv_screen_load(main_screen_obj);

    // 调用滚动图标功能
    scrollicon();

    // 创建定时器，每秒更新一次时间
    lv_timer_create(update_time, 1000, NULL);
    // 立即更新一次时间
    update_time(NULL);
}

// 初始化UI界面
void ui_init(void)
{
    // 创建锁屏界面
    create_lock_screen();

    // 创建主界面
    create_main_screen();

    // 加载锁屏界面
    lv_screen_load(lock_screen_obj);
}
