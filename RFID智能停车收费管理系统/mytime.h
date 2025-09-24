#ifndef __MYTIME_H__
#define __MYYIME_H__

//获取当前时间
char * get_time();


//将一个字符串时间修改为一个年,月,日,时，分
void change_time(char *buf,int *y,int *m,int *d,int *h,int *mi);
#endif