#include "event.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>//open函数的头文件

#include <unistd.h>//close与write函数的头文件

#include <linux/input.h>

#include <string.h> //memset函数头文件

#include <stdlib.h>
#include <stdio.h>

/**
 * event:获取屏幕的点击坐标与滑动方向
 * @int *x：用于保存点击点的横坐标
 * @int *y:用于保存点击点的纵坐标
 * 返回值：
 *      返回方向
 */
int event(int *x ,int *y)
{
    int x1 = -1,x2 = -1,y1 = -1,y2 = -1;
    //1、打开触摸屏文件  /dev/input/event0
    int fd = open("/dev/input/event0",O_RDWR);
    if(-1 == fd)
    {
        perror("open event0 error");
        return -1;
    }
    
    //2、定义一个结构体事件结构体，用于保存结构体信息
    struct input_event ev;
    memset(&ev,0,sizeof(ev));//清空ev的内容

    int re = -1; 
    while(re = read(fd,&ev,sizeof(ev)))
    {
        if(re == sizeof(struct input_event))
        {
            if(ev.type == EV_ABS)//表示触摸屏事件
            {
                if(ev.code == ABS_X)//表示X轴
                {
                    if(x1 == -1)
                    {
                        x1 = ev.value*800/1024; //起始点的横坐标坐标
                    }
                    x2 = ev.value*800/1024; //结束点的横坐标
                }
                else if(ev.code == ABS_Y) //表示Y轴
                {
                    if(y1 == -1)
                    {
                        y1 = ev.value*480/600; //起始点的纵坐标
                    }
                    y2 = ev.value*480/600;//结束点纵坐标
                }
                else if(ev.code == ABS_PRESSURE && ev.value == 0)//触摸结束
                {
                    break;
                }
            }
            else if(ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0)
            {
                break;
            }
        }
    }
    close(fd);
    *x = -1;
    *y = -1;
    //判断获得滑动方向
    int xx = abs(x1-x2);
    int yy = abs(y1-y2);

    if(xx > yy)//是水平滑动
    {
        if(x1 > x2) // 左滑
        {
            return LEFT;            
        }
        else if (x1 < x2) // 右滑
        {
            return RIGHT;
        }
    }
    else if(xx < yy) //垂直滑动
    {
        if(y1 > y2) // 上滑
        { 
            return UP;
        }
        else if(y1 < y2) //下滑
        {
            return DOWN;
        }
    }
    else if(xx == yy && xx == 0)//点击
    {
        *x = x1;
        *y = y1;
        return CLICK;
    }
}