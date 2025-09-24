#include "head.h"
#include <pthread.h>
#include "mysrc/ui.h"

// 图片路径前缀
#define IMG_PATH_PREFIX "A:/wxj/video/"

// 按钮样式
static lv_style_t btn_style;
static lv_style_t btn_pressed_style;
static lv_style_t title_style;
static lv_style_t bar_style;
static lv_style_t bar_indicator_style;
static lv_style_t knob_style;          // 新增滑块旋钮样式
static lv_style_t list_style;          // 列表样式
static lv_style_t list_selected_style; // 列表选中项样式
pthread_t get_time_pid;                // 保存获取时间的线程的ID号
int time_value = 0;

// 初始化样式
static void init_styles(void)
{
    // 普通按钮样式 - 透明背景效果
    lv_style_init(&btn_style);
    lv_style_set_bg_opa(&btn_style, LV_OPA_TRANSP);  // 透明背景

    // 按钮按下样式 - 轻微高亮效果
    lv_style_init(&btn_pressed_style);
    lv_style_set_bg_color(&btn_pressed_style, lv_color_hex(0x3498db)); // 亮蓝色
    lv_style_set_bg_opa(&btn_pressed_style, LV_OPA_30);  // 半透明效果

    // 标题样式
    lv_style_init(&title_style);
    lv_style_set_text_font(&title_style, &lv_font_montserrat_24);
    lv_style_set_text_color(&title_style, lv_color_white());
    lv_style_set_text_align(&title_style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_letter_space(&title_style, 1);
    lv_style_set_text_line_space(&title_style, 2);

    // 进度条样式
    lv_style_init(&bar_style);
    lv_style_set_bg_opa(&bar_style, LV_OPA_COVER);
    lv_style_set_bg_color(&bar_style, lv_color_hex(0x7f8c8d)); // 灰色背景
    lv_style_set_radius(&bar_style, 8);                        // 增大圆角
    lv_style_set_border_width(&bar_style, 0);
    lv_style_set_pad_all(&bar_style, 0);
    lv_style_set_shadow_width(&bar_style, 5);
    lv_style_set_shadow_color(&bar_style, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&bar_style, LV_OPA_30);

    // 进度条指示器样式 - 添加渐变效果
    lv_style_init(&bar_indicator_style);
    lv_style_set_bg_opa(&bar_indicator_style, LV_OPA_COVER);
    lv_style_set_bg_color(&bar_indicator_style, lv_color_hex(0x3498db)); // 蓝色进度
    lv_style_set_bg_grad_color(&bar_indicator_style, lv_color_hex(0x2980b9));
    lv_style_set_bg_grad_dir(&bar_indicator_style, LV_GRAD_DIR_VER);
    lv_style_set_radius(&bar_indicator_style, 8);
    lv_style_set_shadow_width(&bar_indicator_style, 3);
    lv_style_set_shadow_color(&bar_indicator_style, lv_color_hex(0x3498db));
    lv_style_set_shadow_opa(&bar_indicator_style, LV_OPA_50);

    // 滑块旋钮样式 - 增强视觉效果
    lv_style_init(&knob_style);
    lv_style_set_bg_opa(&knob_style, LV_OPA_COVER);
    lv_style_set_bg_color(&knob_style, lv_color_hex(0xffffff));
    lv_style_set_radius(&knob_style, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(&knob_style, 8);
    lv_style_set_shadow_width(&knob_style, 8);
    lv_style_set_shadow_color(&knob_style, lv_color_hex(0x3498db));
    lv_style_set_shadow_opa(&knob_style, LV_OPA_70);
    lv_style_set_shadow_ofs_x(&knob_style, 0);
    lv_style_set_shadow_ofs_y(&knob_style, 0);
    lv_style_set_shadow_spread(&knob_style, 2);

    // 列表样式
    lv_style_init(&list_style);
    lv_style_set_bg_opa(&list_style, LV_OPA_TRANSP);
    lv_style_set_border_width(&list_style, 0);
    lv_style_set_text_font(&list_style, &lv_font_montserrat_16);
    lv_style_set_text_color(&list_style, lv_color_white());
    lv_style_set_pad_top(&list_style, 8);
    lv_style_set_pad_bottom(&list_style, 8);
    lv_style_set_pad_left(&list_style, 10);
    lv_style_set_pad_right(&list_style, 10);

    // 列表选中项样式 - 增强视觉效果
    lv_style_init(&list_selected_style);
    lv_style_set_bg_color(&list_selected_style, lv_color_hex(0x3498db)); // 蓝色选中
    lv_style_set_bg_grad_color(&list_selected_style, lv_color_hex(0x2980b9));
    lv_style_set_bg_grad_dir(&list_selected_style, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&list_selected_style, LV_OPA_COVER);
    lv_style_set_text_color(&list_selected_style, lv_color_white());
    lv_style_set_radius(&list_selected_style, 8);
    lv_style_set_shadow_width(&list_selected_style, 5);
    lv_style_set_shadow_color(&list_selected_style, lv_color_hex(0x3498db));
    lv_style_set_shadow_opa(&list_selected_style, LV_OPA_50);
}

// 按钮回调函数
static void play_btn_cb(lv_event_t *e);
static void prev_btn_cb(lv_event_t *e);
static void next_btn_cb(lv_event_t *e);
static void seek_forward_btn_cb(lv_event_t *e);
static void seek_backward_btn_cb(lv_event_t *e);
static void quit_btn_cb(lv_event_t *e);
static void playlist_item_cb(lv_event_t *e);   // 新增：播放列表项回调
static void start_mplayer_adapter(void *data); // 新增：启动mplayer的适配器函数

// 进度条事件回调
static void progress_slider_event_cb(lv_event_t *e); // 修改为滑块事件

// 进度更新定时器回调
static void progress_update_timer_cb(lv_timer_t *timer);

// 更新时间标签
static void update_time_label(VideoPlayer *player);

// 恢复进度定时器回调
static void resume_progress_timer_cb(lv_timer_t *t);

// 时间格式化函数
static void format_time(double seconds, char *buf)
{
    int total_sec = (int)seconds;
    int hours = total_sec / 3600;
    int minutes = (total_sec % 3600) / 60;
    int secs = total_sec % 60;

    if (hours > 0)
    {
        snprintf(buf, 16, "%02d:%02d:%02d", hours, minutes, secs);
    }
    else
    {
        snprintf(buf, 16, "%02d:%02d", minutes, secs);
    }
}

// 适配器函数，用于将VideoPlayer*转换为void*
static void start_mplayer_adapter(void *data)
{
    VideoPlayer *player = (VideoPlayer *)data;
    start_mplayer(player);
}

// 更新进度显示
void update_progress(VideoPlayer *player)
{
    if (player->total_time > 0)
    {
        // printf("current_time: %.2f  total_time: %.2f", player->current_time, player->total_time);
        int progress = (int)((player->current_time / player->total_time) * 100);
        // printf("progress:%d\n", progress);
        if (progress > 100)
            progress = 100;
        lv_slider_set_value(player->progress_slider, progress, LV_ANIM_OFF);
        update_time_label(player);
    }
}

// 恢复进度定时器
static void resume_progress_timer_cb(lv_timer_t *t)
{
    VideoPlayer *player = (VideoPlayer *)t->user_data;
    player->is_seeking = false;
    if (player->progress_update_timer)
    {
        lv_timer_resume(player->progress_update_timer);
    }
    lv_timer_del(t);
}

// 初始化播放器界面
void init_player_ui(VideoPlayer *player)
{
    // 初始化样式
    init_styles();

    // 创建主容器 - 添加渐变背景和阴影效果
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    player->main_container = cont; // 保存容器指针
    lv_obj_set_size(cont, 1024, 600);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);

    // 设置渐变背景
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x1a1a2e), 0);
    lv_obj_set_style_bg_grad_color(cont, lv_color_hex(0x16213e), 0);
    lv_obj_set_style_bg_grad_dir(cont, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

    // 添加背景图片
    lv_obj_set_style_bg_image_src(cont, IMG_PATH_PREFIX "video.bmp", 0);

    // 播放列表面板
    player->playlist_panel = lv_obj_create(cont);
    lv_obj_set_style_bg_opa(player->playlist_panel, LV_OPA_80, 0);
    lv_obj_set_size(player->playlist_panel, 190, 580);
    lv_obj_align(player->playlist_panel, LV_ALIGN_LEFT_MID, 10, 0);

    // 添加渐变背景和圆角
    lv_obj_set_style_bg_color(player->playlist_panel, lv_color_hex(0x2c3e50), 0);
    lv_obj_set_style_bg_grad_color(player->playlist_panel, lv_color_hex(0x34495e), 0);
    lv_obj_set_style_bg_grad_dir(player->playlist_panel, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_radius(player->playlist_panel, 15, 0);

    // 添加阴影效果
    lv_obj_set_style_shadow_width(player->playlist_panel, 15, 0);
    lv_obj_set_style_shadow_color(player->playlist_panel, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(player->playlist_panel, LV_OPA_60, 0);
    lv_obj_set_style_shadow_ofs_x(player->playlist_panel, 5, 0);
    lv_obj_set_style_shadow_ofs_y(player->playlist_panel, 5, 0);
    lv_obj_set_style_border_width(player->playlist_panel, 0, 0);

    // 播放列表标题
    lv_obj_t *playlist_title = lv_label_create(player->playlist_panel);
    lv_label_set_text(playlist_title, "Video List");
    lv_obj_set_style_text_font(playlist_title, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(playlist_title, lv_color_white(), 0);
    lv_obj_set_style_text_letter_space(playlist_title, 1, 0);
    lv_obj_align(playlist_title, LV_ALIGN_TOP_MID, 0, 10);

    // 添加标题下划线装饰
    lv_obj_t *title_line = lv_obj_create(player->playlist_panel);
    lv_obj_set_size(title_line, 150, 2);
    lv_obj_align(title_line, LV_ALIGN_TOP_MID, 0, 45);
    lv_obj_set_style_bg_color(title_line, lv_color_hex(0x3498db), 0);
    lv_obj_set_style_bg_opa(title_line, LV_OPA_80, 0);
    lv_obj_set_style_radius(title_line, 1, 0);
    lv_obj_clear_flag(title_line, LV_OBJ_FLAG_SCROLLABLE);

    // 创建播放列表控件
    player->playlist = lv_list_create(player->playlist_panel);
    lv_obj_set_size(player->playlist, 170, 500); 
    lv_obj_align(player->playlist, LV_ALIGN_BOTTOM_MID, 0, -15);
    lv_obj_add_style(player->playlist, &list_style, 0);
    lv_obj_set_style_bg_opa(player->playlist, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(player->playlist, 0, 0);
    lv_obj_set_style_radius(player->playlist, 10, 0);

    // 控制面板区域
    lv_obj_t *control_panel = lv_obj_create(cont);
    lv_obj_set_style_bg_opa(control_panel, LV_OPA_80, 0);
    lv_obj_set_size(control_panel, 120, 580);
    lv_obj_align(control_panel, LV_ALIGN_RIGHT_MID, -10, 0);

    // 添加渐变背景和圆角
    lv_obj_set_style_bg_color(control_panel, lv_color_hex(0x2c3e50), 0);
    lv_obj_set_style_bg_grad_color(control_panel, lv_color_hex(0x34495e), 0);
    lv_obj_set_style_bg_grad_dir(control_panel, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_radius(control_panel, 15, 0);

    // 添加阴影效果
    lv_obj_set_style_shadow_width(control_panel, 15, 0);
    lv_obj_set_style_shadow_color(control_panel, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(control_panel, LV_OPA_60, 0);
    lv_obj_set_style_shadow_ofs_x(control_panel, -5, 0);
    lv_obj_set_style_shadow_ofs_y(control_panel, 5, 0);

    lv_obj_set_flex_flow(control_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(control_panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(control_panel, 10, 0);
    lv_obj_set_style_border_width(control_panel, 0, 0);

    // 标题标签
    player->title_label = lv_label_create(cont); // 在主容器中创建
    lv_obj_add_style(player->title_label, &title_style, 0);
    lv_obj_set_width(player->title_label, 200); // 增加宽度
    lv_obj_align(player->title_label, LV_ALIGN_CENTER, 0, -250); // 移动到屏幕中上部分


    // 播放/暂停按钮
    player->play_btn = lv_btn_create(control_panel);
    lv_obj_add_style(player->play_btn, &btn_style, 0);
    lv_obj_add_style(player->play_btn, &btn_pressed_style, LV_STATE_PRESSED);
    lv_obj_set_size(player->play_btn, 80, 80);
    lv_obj_align(player->play_btn, LV_ALIGN_TOP_MID, 0, 70);
    player->play_btn_img = lv_img_create(player->play_btn);
    lv_img_set_src(player->play_btn_img, IMG_PATH_PREFIX "play.png");
    lv_obj_center(player->play_btn_img);
    lv_obj_add_event_cb(player->play_btn, play_btn_cb, LV_EVENT_CLICKED, player);

    // 上一个按钮
    player->prev_btn = lv_btn_create(control_panel);
    lv_obj_add_style(player->prev_btn, &btn_style, 0);
    lv_obj_add_style(player->prev_btn, &btn_pressed_style, LV_STATE_PRESSED);
    lv_obj_set_size(player->prev_btn, 80, 80);
    lv_obj_align_to(player->prev_btn, player->play_btn, LV_ALIGN_BOTTOM_MID, 0, 15);
    player->prev_btn_img = lv_img_create(player->prev_btn);
    lv_img_set_src(player->prev_btn_img, IMG_PATH_PREFIX "next.png");
    lv_obj_center(player->prev_btn_img);
    lv_obj_add_event_cb(player->prev_btn, prev_btn_cb, LV_EVENT_CLICKED, player);

    // 下一个按钮
    player->next_btn = lv_btn_create(control_panel);
    lv_obj_add_style(player->next_btn, &btn_style, 0);
    lv_obj_add_style(player->next_btn, &btn_pressed_style, LV_STATE_PRESSED);
    lv_obj_set_size(player->next_btn, 80, 80);
    lv_obj_align_to(player->next_btn, player->prev_btn, LV_ALIGN_BOTTOM_MID, 0, 15);
    player->next_btn_img = lv_img_create(player->next_btn);
    lv_img_set_src(player->next_btn_img, IMG_PATH_PREFIX "prev.png");
    lv_obj_center(player->next_btn_img);
    lv_obj_add_event_cb(player->next_btn, next_btn_cb, LV_EVENT_CLICKED, player);

    // 快进按钮
    player->seek_forward_btn = lv_btn_create(control_panel);
    lv_obj_add_style(player->seek_forward_btn, &btn_style, 0);
    lv_obj_add_style(player->seek_forward_btn, &btn_pressed_style, LV_STATE_PRESSED);
    lv_obj_set_size(player->seek_forward_btn, 80, 80);
    lv_obj_align_to(player->seek_forward_btn, player->next_btn, LV_ALIGN_BOTTOM_MID, 0, 15);
    player->seek_forward_btn_img = lv_img_create(player->seek_forward_btn);
    lv_img_set_src(player->seek_forward_btn_img, IMG_PATH_PREFIX "fast_forward.png");
    lv_obj_center(player->seek_forward_btn_img);
    lv_obj_add_event_cb(player->seek_forward_btn, seek_forward_btn_cb, LV_EVENT_CLICKED, player);

    // 快退按钮
    player->seek_backward_btn = lv_btn_create(control_panel);
    lv_obj_add_style(player->seek_backward_btn, &btn_style, 0);
    lv_obj_add_style(player->seek_backward_btn, &btn_pressed_style, LV_STATE_PRESSED);
    lv_obj_set_size(player->seek_backward_btn, 80, 80);
    lv_obj_align_to(player->seek_backward_btn, player->seek_forward_btn, LV_ALIGN_BOTTOM_MID, 0, 15);
    player->seek_backward_btn_img = lv_img_create(player->seek_backward_btn);
    lv_img_set_src(player->seek_backward_btn_img, IMG_PATH_PREFIX "fast_rewind.png");
    lv_obj_center(player->seek_backward_btn_img);
    lv_obj_add_event_cb(player->seek_backward_btn, seek_backward_btn_cb, LV_EVENT_CLICKED, player);

    // 退出按钮
    player->quit_btn = lv_btn_create(control_panel);
    lv_obj_add_style(player->quit_btn, &btn_style, 0);
    lv_obj_add_style(player->quit_btn, &btn_pressed_style, LV_STATE_PRESSED);
    lv_obj_set_size(player->quit_btn, 80, 80);
    lv_obj_align_to(player->quit_btn, player->seek_backward_btn, LV_ALIGN_BOTTOM_MID, 0, 15);
    player->quit_btn_img = lv_img_create(player->quit_btn);
    lv_img_set_src(player->quit_btn_img, IMG_PATH_PREFIX "quit.png");
    lv_obj_center(player->quit_btn_img);
    lv_obj_add_event_cb(player->quit_btn, quit_btn_cb, LV_EVENT_CLICKED, player);

    // 创建进度条区域
    lv_obj_t *progress_cont = lv_obj_create(cont);
    lv_obj_set_style_bg_opa(progress_cont, LV_OPA_80, 0);
    lv_obj_set_size(progress_cont, 640, 60);
    lv_obj_align(progress_cont, LV_ALIGN_BOTTOM_MID, 0, -20);

    // 添加渐变背景和圆角
    lv_obj_set_style_bg_color(progress_cont, lv_color_hex(0x2c3e50), 0);
    lv_obj_set_style_bg_grad_color(progress_cont, lv_color_hex(0x34495e), 0);
    lv_obj_set_style_bg_grad_dir(progress_cont, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_radius(progress_cont, 15, 0);

    // 添加阴影效果
    lv_obj_set_style_shadow_width(progress_cont, 15, 0);
    lv_obj_set_style_shadow_color(progress_cont, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(progress_cont, LV_OPA_60, 0);
    lv_obj_set_style_shadow_ofs_x(progress_cont, 0, 0);
    lv_obj_set_style_shadow_ofs_y(progress_cont, -5, 0);

    lv_obj_set_style_pad_all(progress_cont, 10, 0);
    lv_obj_set_style_border_width(progress_cont, 0, 0);

    // 创建进度滑块
    player->progress_slider = lv_slider_create(progress_cont);
    lv_obj_set_size(player->progress_slider, 620, 30);
    lv_obj_align(player->progress_slider, LV_ALIGN_TOP_MID, 0, 5);
    lv_slider_set_range(player->progress_slider, 0, 100);

    // 应用样式
    lv_obj_add_style(player->progress_slider, &bar_style, LV_PART_MAIN);
    lv_obj_add_style(player->progress_slider, &bar_indicator_style, LV_PART_INDICATOR);

    // 添加旋钮样式
    lv_obj_add_style(player->progress_slider, &knob_style, LV_PART_KNOB);

    // 添加事件回调
    lv_obj_add_event_cb(player->progress_slider, progress_slider_event_cb, LV_EVENT_ALL, player);

    // 创建时间标签
    player->time_label = lv_label_create(progress_cont);
    lv_label_set_text(player->time_label, "00:00 / 00:00");
    lv_obj_align(player->time_label, LV_ALIGN_BOTTOM_MID, 0, -5);

    // 增强时间标签样式
    lv_obj_set_style_text_color(player->time_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(player->time_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_letter_space(player->time_label, 1, 0);

    // 添加时间标签背景效果
    static lv_style_t time_label_style;
    lv_style_init(&time_label_style);
    lv_style_set_bg_color(&time_label_style, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&time_label_style, LV_OPA_40);
    lv_style_set_radius(&time_label_style, 10);
    lv_style_set_pad_all(&time_label_style, 5);
    lv_obj_add_style(player->time_label, &time_label_style, 0);

    // 初始化进度条相关变量
    player->total_time = 0;
    player->current_time = 0;
    player->is_dragging = false;
    player->is_seeking = false;
}

// 更新播放列表显示顺序（按链表顺序）
void update_playlist(VideoPlayer *player)
{
    if (!player->video_list || !player->video_list->first)
        return;

    // 清空现有列表
    lv_obj_clean(player->playlist);
    player->selected_item = NULL;

    // 如果没有当前播放的视频，则从第一个开始
    if (!player->current_video)
    {
        player->current_video = player->video_list->first;
    }

    // 从链表的第一个节点开始遍历（固定顺序）
    Binode *cur = player->video_list->first;
    do
    {
        const char *filename = strrchr(cur->data, '/');
        if (filename)
            filename++;
        else
            filename = cur->data;

        lv_obj_t *item = lv_list_add_btn(player->playlist, NULL, filename);
        lv_obj_set_user_data(item, cur);
        lv_obj_add_event_cb(item, playlist_item_cb, LV_EVENT_CLICKED, player);

        // 如果是当前播放的视频，高亮显示
        if (cur == player->current_video)
        {
            lv_obj_add_style(item, &list_selected_style, 0);
            player->selected_item = item;
        }

        cur = cur->next;
    } while (cur != player->video_list->first);
}

// 高亮当前播放项（新增）
void highlight_current_item(VideoPlayer *player)
{
    if (!player->playlist || !player->video_list)
        return;

    // 移除之前的高亮
    if (player->selected_item)
    {
        lv_obj_remove_style(player->selected_item, &list_selected_style, 0);
    }

    // 查找并高亮当前项
    Binode *cur = player->video_list->first;
    do
    {
        if (cur == player->current_video)
        {
            // 查找对应的列表项
            for (int i = 0; i < lv_obj_get_child_cnt(player->playlist); i++)
            {
                lv_obj_t *item = lv_obj_get_child(player->playlist, i);
                if (lv_obj_get_user_data(item) == cur)
                {
                    lv_obj_add_style(item, &list_selected_style, 0);
                    player->selected_item = item;
                    // 滚动到可见区域
                    lv_obj_scroll_to_view(item, LV_ANIM_OFF);
                    break;
                }
            }
            break;
        }
        cur = cur->next;
    } while (cur != player->video_list->first);
}

// 播放列表项点击回调（新增）
static void playlist_item_cb(lv_event_t *e)
{
    VideoPlayer *player = (VideoPlayer *)lv_event_get_user_data(e);
    lv_obj_t *item = lv_event_get_target(e);
    Binode *video = (Binode *)lv_obj_get_user_data(item);

    if (video && video != player->current_video)
    {
        player->current_video = video;
        start_mplayer(player);
        highlight_current_item(player);
    }
}

// 初始化管道
void init_fifos(VideoPlayer *player)
{
    // 创建控制管道
    int ret = mkfifo(MPLAYER_CTL_FIFO, 0777);
    if (ret == -1 && errno != EEXIST)
    {
        perror("mkfifo for control fail");
        exit(1);
    }
    player->mplayer_ctl_fd = open(MPLAYER_CTL_FIFO, O_RDWR);
    if (player->mplayer_ctl_fd == -1)
    {
        perror("open control fifo fail");
        exit(1);
    }

    // 设置管道为非阻塞模式
    int flags = fcntl(player->mplayer_ctl_fd, F_GETFL, 0);
    fcntl(player->mplayer_ctl_fd, F_SETFL, flags | O_NONBLOCK);
}

/*
    get_current_time: 线程处理获取自动播放的当前播放时间
*/
void *get_current_time(void *arg)
{
    pthread_detach(pthread_self());
    VideoPlayer *player = (VideoPlayer *)arg;
    player->thread_running = true;

    while (player->thread_running)
    {
        if (player->mplayer_pid > 0 && player->state == PLAYER_PLAYING && !(player->is_dragging))
        {
            if (player->is_seeking)
            {
                fcntl(player->mplayer_resp_fd, F_SETFL, O_NONBLOCK);
                char dummy[256];
                while (read(player->mplayer_resp_fd, dummy, sizeof(dummy)) > 0)
                    ;
                player->is_seeking = false;
                continue;
            }

            fcntl(player->mplayer_resp_fd, F_SETFL, O_NONBLOCK);
            char dummy[256];
            while (read(player->mplayer_resp_fd, dummy, sizeof(dummy)) > 0)
                ;

            write(player->mplayer_ctl_fd, "get_time_pos\n", 13);
            if (player->total_time <= 0)
            {
                write(player->mplayer_ctl_fd, "get_time_length\n", 16);
            }

            usleep(100000);

            char buf[1024];
            ssize_t n = read(player->mplayer_resp_fd, buf, sizeof(buf) - 1);

            if (n > 0)
            {
                buf[n] = '\0';
                char *line = buf;
                while (line)
                {
                    char *next_line = strchr(line, '\n');
                    if (next_line)
                        *next_line++ = '\0';

                    if (strstr(line, "ANS_TIME_POSITION=") == line)
                    {
                        player->current_time = atof(line + 18);
                        // printf("get_current_timer: %.2f\n", player->current_time);
                    }
                    else if (strstr(line, "ANS_LENGTH=") == line)
                    {
                        player->total_time = atof(line + 11);
                    }
                    line = next_line;
                }
            }
            // 检测视频是否播放结束
            // 检测视频是否播放结束
            else if (player->current_time >= player->total_time - 0.5 && player->total_time > 0)
            {
                // 视频播放结束，播放下一个
                // printf("Video playback finished, playing next.\n");
                if (player->video_list && player->current_video)
                {
                    player->current_video = player->current_video->next;
                    // 使用适配器函数调用start_mplayer
                    lv_async_call(start_mplayer_adapter, player);
                    highlight_current_item(player);
                }
                // 重置当前时间，避免重复触发
                player->current_time = 0;
                usleep(100000); // 添加短暂延时，避免CPU占用过高
                continue;
            }
        }
        // 添加短暂休眠避免过度占用CPU
        usleep(100000);
    }
    return NULL;
}
// 启动MPlayer进程
void start_mplayer(VideoPlayer *player)
{
    if (player->mplayer_pid > 0) // 杀死原来播放mplayer的子进程
    {
        kill(player->mplayer_pid, SIGKILL);
        wait(NULL);
        pthread_cancel(get_time_pid); // 杀死原来获取时间的线程
    }

    // 计算视频位置
    int x_offset = (1024 - 600) / 2;
    int y_offset = (600 - 400) / 2;

    // 创建响应管道（用于读取MPlayer的响应）
    int resp_pipe[2];
    if (pipe(resp_pipe) < 0)
    {
        perror("pipe fail");
        return;
    }

    // 创建子进程
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork fail");
        close(resp_pipe[0]);
        close(resp_pipe[1]);
        return;
    }

    if (pid == 0) // 子进程：执行mplayer
    {
        close(resp_pipe[0]); // 关闭管道的读端

        // 重定向stdout
        dup2(resp_pipe[1], STDOUT_FILENO);
        // close(resp_pipe[1]);

        // 执行MPlayer
        int ret = execl("/usr/bin/mplayer64", "mplayer64",
                        "-slave",
                        "-quiet",
                        "-vo", "fbdev2",
                        "-ao", "alsa",
                        "-input", "file=./mplayer_ctl.fifo",
                        "-geometry", "212:100",
                        "-zoom",
                        "-x", "600",
                        "-y", "400",
                        player->current_video->data, NULL);
        if (ret == -1)
        {
            perror("execl fail");
            exit(1);
        }
    }
    else // 父进程
    {
        close(resp_pipe[1]); // 关闭管道的写端

        player->mplayer_pid = pid;

        // 保存mplayer输出time信息的管道的读端文件描述符
        player->mplayer_resp_fd = resp_pipe[0];

        // 设置非阻塞
        int flags = fcntl(player->mplayer_resp_fd, F_GETFL, 0);
        fcntl(player->mplayer_resp_fd, F_SETFL, flags | O_NONBLOCK);

        // 更新标题
        const char *filename = strrchr(player->current_video->data, '/');
        if (filename)
            filename++;
        else
            filename = player->current_video->data;

        char title[256];
        snprintf(title, sizeof(title), "now: %s", filename);
        lv_label_set_text(player->title_label, title);

        // 重置状态
        player->total_time = 0;
        player->current_time = 0;
        player->is_dragging = false; // 自动
        player->is_seeking = false;
        time_value = 0;
        if (player->progress_update_timer != NULL)
        {
            lv_timer_del(player->progress_update_timer);
            player->progress_update_timer = NULL;
        }
        update_progress(player);

        // 启动后立即发送获取总时长命令
        usleep(100000); // 等待100ms让mplayer启动

        // 播放状态设置为开始播放
        player->state = PLAYER_PLAYING;
        update_play_btn_img(player); // 更新暂停与继续标签

        // 高亮当前播放项
        highlight_current_item(player);

        // 确保之前的线程已停止
        if (player->thread_running)
        {
            player->thread_running = false;
            pthread_cancel(get_time_pid);
            usleep(100000); // 等待线程结束
        }
        
        // 创建线程不断获取当前播放时间
        pthread_create(&get_time_pid, NULL, get_current_time, (void *)player);

        // 创建新的进度更新定时器
        player->progress_update_timer = lv_timer_create(progress_update_timer_cb, 500, player);
    }
}

// 清理资源
void player_cleanup(VideoPlayer *player)
{
    // 先停止获取时间的线程
    if (player->thread_running)
    {
        player->thread_running = false;
        pthread_cancel(get_time_pid);
        // 等待线程结束
        usleep(100000);
    }
    
    // 停止mplayer进程
    if (player->mplayer_pid > 0)
    {
        kill(player->mplayer_pid, SIGKILL);
        wait(NULL);
        player->mplayer_pid = -1;
    }

    // 关闭文件描述符
    if (player->mplayer_ctl_fd != -1)
    {
        close(player->mplayer_ctl_fd);
        player->mplayer_ctl_fd = -1;
    }
    if (player->mplayer_resp_fd != -1)
    {
        close(player->mplayer_resp_fd);
        player->mplayer_resp_fd = -1;
    }

    // 删除管道文件
    unlink(MPLAYER_CTL_FIFO);

    // 删除定时器
    if (player->progress_update_timer)
    {
        lv_timer_del(player->progress_update_timer);
        player->progress_update_timer = NULL;
    }
    
    // 清理视频列表
    if (player->video_list)
    {
        clear_bilinklist(player->video_list);
        free(player->video_list);
        player->video_list = NULL;
        player->current_video = NULL;
    }
    
    // 直接删除整个容器，LVGL会自动清理所有子对象
    if (player->main_container)
    {
        lv_obj_del(player->main_container);
        player->main_container = NULL;
    }
}

// 更新播放/暂停按钮图片
void update_play_btn_img(VideoPlayer *player)
{
    if (player->state == PLAYER_PLAYING)
    {
        lv_img_set_src(player->play_btn_img, IMG_PATH_PREFIX "play.png");
    }
    else
    {
        lv_img_set_src(player->play_btn_img, IMG_PATH_PREFIX "pause.png");
    }
}

// 更新时间标签
static void update_time_label(VideoPlayer *player)
{
    char current_buf[16], total_buf[16];
    format_time(player->current_time, current_buf);
    format_time(player->total_time, total_buf);

    char buf[32];
    snprintf(buf, sizeof(buf), "%s / %s", current_buf, total_buf);
    lv_label_set_text(player->time_label, buf);
    // printf("%s %s\n", current_buf, total_buf);
}

// 进度滑块事件回调
static void progress_slider_event_cb(lv_event_t *e)
{
    VideoPlayer *player = (VideoPlayer *)lv_event_get_user_data(e);
    lv_obj_t *slider = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (player->state != PLAYER_PLAYING && player->state != PLAYER_PAUSED)
    {
        return;
    }

    if (code == LV_EVENT_PRESSED) // 用户开始拖动进度条
    {
        player->is_dragging = true; // 表示开始手动拖动了
    }
    else if (code == LV_EVENT_PRESS_LOST || code == LV_EVENT_RELEASED) // 用户手离开屏幕
    {
        int value = lv_slider_get_value(slider);
        double seek_time = player->total_time * (value / 100.0);

        // 清空管道缓冲区
        fcntl(player->mplayer_resp_fd, F_SETFL, O_NONBLOCK);
        char dummy[256];
        while (read(player->mplayer_resp_fd, dummy, sizeof(dummy)) > 0)
            ;

        // 发送跳转命令
        char cmd[32];
        snprintf(cmd, sizeof(cmd), "seek %d 2\n", (int)seek_time);
        // printf("seek cmd: %s\n", cmd);
        write(player->mplayer_ctl_fd, cmd, strlen(cmd));

        usleep(100000); // 等待响应

        // 更新当前时间
        player->current_time = seek_time;
        update_progress(player);

        player->is_dragging = false;
        player->is_seeking = true;
    }
    else if (code == LV_EVENT_VALUE_CHANGED && player->is_dragging)
    {
        int value = lv_slider_get_value(slider);
        // printf("LV_EVENT_VALUE_CHANGED: %d\n", value);
        double seek_time = player->total_time * value / 100.0;
        player->current_time = seek_time;

        update_progress(player);
    }
}

// 进度更新定时器回调
static void progress_update_timer_cb(lv_timer_t *timer)
{
    VideoPlayer *player = (VideoPlayer *)timer->user_data;

    // 如果正在拖动或跳转，跳过更新
    if (player->is_dragging)
    {
        return;
    }

    if (player->state != PLAYER_PLAYING && player->state != PLAYER_PAUSED)
    {
        return;
    }

    // printf("timer: ");
    update_progress(player);
}

// 按钮回调函数
static void play_btn_cb(lv_event_t *e)
{
    VideoPlayer *player = (VideoPlayer *)lv_event_get_user_data(e);
    if (player->mplayer_ctl_fd == -1)
        return;

    if (player->state == PLAYER_PAUSED)
    {
        write(player->mplayer_ctl_fd, "play\n", 5);
        player->state = PLAYER_PLAYING;
    }
    else if (player->state == PLAYER_PLAYING)
    {
        write(player->mplayer_ctl_fd, "pause\n", 6);
        player->state = PLAYER_PAUSED;
    }
    else if (player->state == PLAYER_STOPPED)
    {
        start_mplayer(player);
    }

    update_play_btn_img(player);
}

static void prev_btn_cb(lv_event_t *e)
{
    VideoPlayer *player = (VideoPlayer *)lv_event_get_user_data(e);
    if (player->video_list && player->current_video)
    {
        player->current_video = player->current_video->prev;
        start_mplayer(player);
        highlight_current_item(player); // 更新高亮
    }
}

static void next_btn_cb(lv_event_t *e)
{
    VideoPlayer *player = (VideoPlayer *)lv_event_get_user_data(e);
    if (player->video_list && player->current_video)
    {
        player->current_video = player->current_video->next;
        start_mplayer(player);
        highlight_current_item(player); // 更新高亮
    }
}

static void seek_forward_btn_cb(lv_event_t *e)
{
    VideoPlayer *player = (VideoPlayer *)lv_event_get_user_data(e);
    if (player->mplayer_ctl_fd != -1)
    {
        write(player->mplayer_ctl_fd, "seek +5\n", strlen("seek +5\n"));
    }
}

static void seek_backward_btn_cb(lv_event_t *e)
{
    VideoPlayer *player = (VideoPlayer *)lv_event_get_user_data(e);
    if (player->mplayer_ctl_fd != -1)
    {
        write(player->mplayer_ctl_fd, "seek -5\n", strlen("seek -5\n"));
    }
}

static void quit_btn_cb(lv_event_t *e)
{
    VideoPlayer *player = (VideoPlayer *)lv_event_get_user_data(e);
    player_cleanup(player);
    extern lv_obj_t *main_screen_obj;
    lv_screen_load(main_screen_obj);
}
