#include "lvgl/lvgl.h"
#include <pthread.h>
#include <time.h>
#include "head.h"
#include "mysrc/ui.h"

// 全局视频播放器变量
VideoPlayer player = {0};

int main(int argc, char **argv)
{
    lv_init();

    /* Linux frame buffer device init */
    lv_display_t *disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, "/dev/fb0");

    lv_indev_t *indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event6");

    ui_init();


    // 创建视频列表
    player.video_list = create_bilinklist();

    // 检查命令行参数
    if (argc > 1)
    {
        recursion_save_path(argv[1], player.video_list);
    }
    else
    {
        // 默认视频路径
        recursion_save_path("/wxj/video", player.video_list);
    }

    // 检查是否有视频文件
    if (player.video_list->count > 0)
    {
        player.current_video = player.video_list->first;
    }
    else
    {
        printf("未找到视频文件\n");
        return 1;
    }

    // 初始化管道
    init_fifos(&player);

    // 主循环
    while (1)
    {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}