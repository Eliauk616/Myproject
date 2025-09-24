#ifndef __LVGL_PLAYER_H__
#define __LVGL_PLAYER_H__

#include "lvgl/lvgl.h"
#include "head.h"

// 播放器状态枚举
enum player_state
{
    PLAYER_STOPPED,
    PLAYER_PLAYING,
    PLAYER_PAUSED
};

// 播放器结构体
typedef struct
{
    lv_obj_t *play_btn;
    lv_obj_t *prev_btn;
    lv_obj_t *next_btn;
    lv_obj_t *seek_forward_btn;
    lv_obj_t *seek_backward_btn;
    lv_obj_t *quit_btn;
    lv_obj_t *title_label;

    // 图片对象
    lv_obj_t *play_btn_img;
    lv_obj_t *prev_btn_img;
    lv_obj_t *next_btn_img;
    lv_obj_t *seek_forward_btn_img;
    lv_obj_t *seek_backward_btn_img;
    lv_obj_t *quit_btn_img;

    Bilinklist *video_list;
    Binode *current_video;

    pid_t mplayer_pid;
    int mplayer_ctl_fd;  // 控制管道
    int mplayer_resp_fd; // 响应管道（新增）

    enum player_state state;
    pthread_mutex_t mutex;

    // 进度条相关
    lv_obj_t *progress_slider; // 修改为滑块
    lv_obj_t *time_label;
    double total_time;
    double current_time;
    lv_timer_t *progress_update_timer;
    bool is_dragging;
    bool is_seeking;

    // 播放列表相关（新增）
    lv_obj_t *playlist_panel; // 播放列表面板
    lv_obj_t *playlist;       // 播放列表控件
    lv_obj_t *selected_item;  // 当前选中的列表项
    
    // 主容器
    lv_obj_t *main_container;  // 播放器主容器
    
    // 线程控制
    bool thread_running;      // 线程运行标志
} VideoPlayer;

// 函数声明
void init_player_ui(VideoPlayer *player);
void start_mplayer(VideoPlayer *player);
void init_fifos(VideoPlayer *player);
void player_cleanup(VideoPlayer *player);
void update_play_btn_img(VideoPlayer *player);
void update_progress(VideoPlayer *player);
void update_playlist(VideoPlayer *player);
void highlight_current_item(VideoPlayer *player);
static int init_video_player(VideoPlayer *player, int argc, char **argv);

// 全局函数声明

#endif