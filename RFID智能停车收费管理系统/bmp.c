#include "bmp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "lcd.h"
/**
 * dispaly_bmp:显示一张BMP图片
 * @const char *bmp_path:bmp图片的路径
 * @int x,y:显示图片的位置（x,y）
 * @unsigned int *plcd:映射屏幕的首地址
 * 返回值：
 *      void
 */
void display_bmp(const char *bmp_path,int x,int y,unsigned int *plcd)
{
    // ①打开BMP图片
    int bmp_fd = open(bmp_path,O_RDONLY);
    if(-1 == bmp_fd)
    {
        perror("open bmp error");
        return ;
    }
    //读取图片宽度
    int w = 0;
    lseek(bmp_fd,0x12,SEEK_SET);
    read(bmp_fd,&w,4);
    //读取图片高度
    int h = 0;
    lseek(bmp_fd,0x16,SEEK_SET);
    read(bmp_fd,&h,4);
    //读取色深
    short depth = 0;
    lseek(bmp_fd,0x1c,SEEK_SET);
    read(bmp_fd,&depth,2);
    //计算每一行理论上保存像素点的大小
    int line_bytes = abs(w)*depth/8;
    int lazi = 0;//用于保存赖子数（无用的自动填充的）
    if(line_bytes % 4)
    {
        lazi = 4 - (line_bytes%4); //计算赖子数量
    }
    //实际一行的大小为理论值+赖子数
    line_bytes = line_bytes + lazi;
    //计算像素的总大小也就是所求的像素数组的大小 用每一行的*多少行（高度）
    int bytes = line_bytes * abs(h);
    //定义一个像素数组保存图片的像素信息
    unsigned char buf[bytes];
    //将光标偏移到保存像素信息的位置，读取像素信息
    lseek(bmp_fd,0x36,SEEK_SET);
    read(bmp_fd,buf,bytes);
    //得到像素数组后关闭图片文件
    close(bmp_fd);

    //下面的部分为  图片的显示
    unsigned char a,r,g,b; //用于保存每个像素点的分量信息
    int cx,cy; //像素数组存储像素点的坐标  不是显示的坐标
    int i = 0; //用于遍历像素数组

    for(cy = 0; cy < abs(h);cy++) //一行一行的遍历我们图片的像素点
    {
        for(cx = 0;cx < abs(w);cx++)//每一行有多少个像素点信息，一个一个的遍历
        {
            b = buf[i++];   //像素点的颜色  蓝所占分量值
            g = buf[i++];   //像素点的颜色  绿色所占分量值
            r = buf[i++];   //像素点的颜色  红色所占分量值
            if(depth == 32)//只有当图片的色深为32时，a才会有值
            {
                a = buf[i++];
            }
            else
            {
                a = 0;
            }
            // 合成像素点原色值
            unsigned int color = a << 24 | r << 16 | g << 8 | b;

            int sx,sy;//用与保存图片的像素在屏幕上的位置
            if(w > 0 && h > 0)
            {
                sx = cx;
                sy = abs(h)-cy-1;
            }
            else if(w > 0 && h < 0)
            {
                sx = cx;
                sy = cy;
            }
            else if(w < 0 && h > 0)
            {
                sx = abs(w)-cx-1;
                sy = abs(h)-cy-1;
            }
            else if (w < 0 && h < 0)
            {
                sx = abs(w)-cx-1;
                sy = cy;
            }
            
            display_point(x+sx,y+sy,color,plcd);
        }
        i+=lazi;
    }
}