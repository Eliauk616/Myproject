#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdio.h>
#include <sys/mman.h>


/** 
 *init_lcd:初始化lcd屏幕
 *@int lcd_Fd：用于保存屏幕文件的文件描述符
 返回值：
    unsigned int *返回屏幕文件的首地址
 */
unsigned int *init_lcd(int *lcd_fd)
{
    //打开LCD屏幕
    *lcd_fd = open("/dev/fb0",O_RDWR);
    if(*lcd_fd == -1)
    {
        perror("lcd open errror");
        return NULL;
    }
    
    //屏幕映射
    unsigned int * plcd = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,*lcd_fd,0);
    if(NULL == plcd)
    {
        perror("mmap error");
        return NULL;
    }

    return plcd;
}
/**
 * close_lcd：关闭屏幕
 * @unsigned int*plcd：屏幕文件的首地址
 * @int lcd_fd:屏幕文件的文件描述符
 *   返回值：
 *      void
 */
void close_lcd(unsigned int *plcd ,int lcd_fd)
{
    munmap(plcd,800*480*4);
    close(lcd_fd);
}

/**
 * display_point：给屏幕的一个点显示颜色
 * @x：显示点的横坐标
 * @y  显示点的纵坐标
 * @color：显示的颜色
 * @plcd：屏幕文件的文件描述符也就是mmap函数的返回值
 * 返回值：
 *      void
 */
void display_point(int x,int y,unsigned int color,unsigned int *plcd)
{
    if(x >= 0 && x <800 && y < 480 && y >= 0)
    {
        *(plcd + 800 * y + x ) = color;
    }
}

/**
 * display_re:画一个矩形
 * @int x：矩形位置的横坐标
 * @int y：矩形位置的纵坐标
 * @int w：矩形的宽度
 * @int h：矩形的高度
 * @unsigned int color:矩形的颜色
 * @unsigned int *plcd:屏幕文件描述符
 * 返回值
 *      void
 */
void display_re(int x,int y,int w,int h,unsigned int color,unsigned int *plcd)
{
    for(int j = y; j < h+y;j++)
    {
        for(int i = x;i < w+x;i++)
        {
            display_point(i,j,color,plcd);
        }
    }
}

 /**
  * display_clr:画一个圆形
  * @int x：圆心坐标横坐标
  * @int y：圆心坐标纵坐标
  * @int r：圆的半径
  * @unsigned int color：圆的颜色
  * @unsigned int*plcd：屏幕文件的首地址
  * 返回值：
  *     void
  */
 void display_clr(int x,int y,int r,unsigned int color,unsigned int *plcd)
 {
    for(int j = 0;j < 480;j++)
    {
        for(int i = 0; i < 800 ;i++)
        {
            if((i - x)*(i - x) + (j - y)*(j - y) <= r*r)
            {
                display_point(i,j,color,plcd);
            }
        }
    }
 }

