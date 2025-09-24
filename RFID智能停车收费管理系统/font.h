#ifndef __FRONT_H__
#define __FRONT_H__

extern char word[24][40/8*35];
//在想要的位置显示想要的字符
/**
 *show_font:显示字符
 *@int x,y:显示的位置
 *@int w,h:显示字符的宽高
 *@char *word_array:字符的字符数组
 *int color：字符的颜色
 *unsigned int *plcd：屏幕映射
 */
void show_font(int x,int y,int w,int h,char *word_array, int color,unsigned int *plcd);

//在想要的位置，显示你想要的数字
/**
 * show_num:显示数字
 * x,y :需要显示的位置
 * number:需要显示的数字
 * color ：字体颜色
 * plcd ：屏幕映射mmap（）的返回值
 */
void show_num(int x,int y,int number,int color,unsigned int *plcd);



#endif