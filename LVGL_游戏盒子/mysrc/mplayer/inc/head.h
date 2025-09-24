#ifndef __HEAD_H__
#define __HEAD_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>

#include "circle_bilinklist.h"
#include "save_path.h"
#include "lvgl_player.h"

#define MPLAYER_CTL_FIFO "./mplayer_ctl.fifo"
#define MPLAYER_PATH_FIFO "./mplayer_path.fifo"

// 添加定时器头文件
#include "lvgl/src/misc/lv_timer.h"

#endif