#ifndef __BMP_H__
#define __BMP_H__

/**
 * dispaly_bmp:显示一张BMP图片
 * @const char *bmp_path:bmp图片的路径
 * @int x,y:显示图片的位置（x,y）
 * @unsigned int *plcd:映射屏幕的首地址
 * 返回值：
 *      void
 */
void display_bmp(const char *bmp_path,int x,int y,unsigned int *plcd); 


#endif