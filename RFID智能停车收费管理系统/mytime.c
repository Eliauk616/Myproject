#include "mytime.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

//获取当前时间
char * get_time()
{
    struct  timeval tv;
    gettimeofday(&tv,NULL);
    time_t sec = tv.tv_sec;
    suseconds_t usec = tv.tv_usec;
    struct tm *lt = localtime(&sec);
    
    int year = lt->tm_year+1900+10;
    int mon = lt->tm_mon+1+1+4;
    int day = lt->tm_mday+4;
    int min = lt->tm_min+35;
    int hour = lt->tm_hour+9;
    int s = lt->tm_sec;
    if(min >= 60)
    {
        hour = hour +1;
        min = min - 60;
    }
    
    printf("获取的时间为：%d年%02d月%02d日%02d时%02d分\n",year,mon,day,hour,min);
     
    char *buf = malloc(20);
    sprintf(buf,"%d/%02d/%02d/%02d/%02d",year,mon,day,hour,min);
    printf("时间为：%s\n",buf);
    return buf;
}

//将一个字符串时间修改为一个年,月,日,时，分，秒
void change_time(char *buf,int *y,int *m,int *d,int *h,int *mi)
{
    char bufs[20] ={0};
    strcpy(bufs,buf);
    *y = atoi(bufs);
    *m = atoi(bufs+5);
    *d = atoi(bufs+8);
    *h = atoi(bufs+11);
    *mi = atoi(bufs+14);

    printf("y = %d  m = %d  d = %d h = %d mi = %d \n",*y,*m,*d,*h,*mi);
}