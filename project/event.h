#ifndef __EVENT_H__
#define __EVENT_H__

#define CLICK 0  // 点击
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4
/**
 * event:获取屏幕的点击坐标与滑动方向
 * @int *x：用于保存点击点的横坐标
 * @int *y: 用于保存点击点的纵坐标
 * 返回值：
 *      返回方向
 */
int event(int *x ,int *y);


#endif