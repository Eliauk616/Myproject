#include "lcd.h"
#include "bmp.h"
#include "font.h"
#include "stdio.h"
#include "event.h"
#include "mytime.h"
#include "unistd.h"

void display_management_interface(unsigned int *plcd) {
    // 清屏
    display_re(0, 0, 800, 480, 0xFFFFFF, plcd);
    
    // 显示标题
    show_font(300, 50, 40, 35, word[16], 0x000000, plcd);  // 管
    show_font(340, 50, 40, 35, word[17], 0x000000, plcd);  // 理
    show_font(380, 50, 40, 35, word[18], 0x000000, plcd);  // 界
    show_font(420, 50, 40, 35, word[19], 0x000000, plcd);  // 面
    
    // 开卡按钮
    display_re(200, 150, 400, 80, 0x73FBFD, plcd);  // 浅蓝色背景
    show_font(350, 170, 40, 35, word[20], 0x000000, plcd);  // 开
    show_font(390, 170, 40, 35, word[21], 0x000000, plcd);  // 卡
    
    // 充值按钮
    display_re(200, 250, 400, 80, 0x39107B, plcd);  // 紫色背景
    show_font(350, 270, 40, 35, word[22], 0xFFFFFF, plcd);  // 充
    show_font(390, 270, 40, 35, word[23], 0xFFFFFF, plcd);  // 值
    
    // 退出按钮
    display_re(200, 350, 400, 80, 0xEB3324, plcd);  // 红色背景
    show_font(350, 370, 40, 35, word[15], 0xFFFFFF, plcd);  // 退
    show_font(390, 370, 40, 35, word[12], 0xFFFFFF, plcd);  // 出
    
}
/*
    信息显示界面 
    @money：余额
    @state：状态
    @time:  时间
*/
void display_parking_info(int num,int money, int state, int y ,int m ,int d ,int h ,int mi , unsigned int* plcd) {

    // 清屏
    display_re(0, 0, 800, 480, 0xFFFFFF, plcd);
    // int x3 = -1,y3 = -1;
    // int f3 = event(&x3, &y3);
    
    // 显示标题
    show_font(300, 50, 40, 35, word[0], 0x000000, plcd);  // 车
    show_font(340, 50, 40, 35, word[1], 0x000000, plcd);  // 牌
    show_font(380, 50, 40, 35, word[2], 0x000000, plcd);  // 信
    show_font(420, 50, 40, 35, word[3], 0x000000, plcd);  // 息
    
    // 显示车牌
    show_font(200, 120, 40, 35, word[0], 0x000000, plcd);  // 车
    show_font(240, 120, 40, 35, word[1], 0x000000, plcd);  // 牌
    
    // 显示车牌内容
    // 这里假设车牌是"粤A12345"，你可以根据实际车牌修改
    show_font(400, 120, 40, 35, word[4], 0xFF0000, plcd);  // 粤
    show_font(440, 120, 40, 35, word[5], 0xFF0000, plcd);  // A
    show_num(480, 120, num, 0xFF0000, plcd);

    
    // 显示金额
    show_font(200, 200, 40, 35, word[6], 0x000000, plcd);  // 金
    show_font(240, 200, 40, 35, word[7], 0x000000, plcd);  // 额
    
    // 显示金额数值
    show_num(400, 200, money, 0xFF0000, plcd);

    
    // 显示状态
    show_font(200, 280, 40, 35, word[9], 0x000000, plcd);  // 状
    show_font(240, 280, 40, 35, word[10], 0x000000, plcd);  // 态
    
    if (state == 1) {
        show_font(400, 280, 40, 35, word[11], 0x00FF00, plcd);  // IN
    } else {
        show_font(400, 280, 40, 35, word[12], 0xFF0000, plcd);  // OUT
    }
    
    // 显示时间
    show_font(200, 360, 40, 35, word[13], 0x000000, plcd);  // 时
    show_font(240, 360, 40, 35, word[14], 0x000000, plcd);  // 间
    
    
    show_num(400,360, y, 0x0000FF, plcd);
    show_num(500,360, m, 0x0000FF, plcd);
    show_num(528,360, d, 0x0000FF, plcd);
    show_num(556,360, h, 0x0000FF, plcd);
    show_num(584,360, mi, 0x0000FF, plcd);
    
    // 显示退出按钮
    display_re(700, 380, 80, 80, 0xFF0000, plcd);
    show_font(710, 400, 40, 35, word[15], 0xFFFFFF, plcd);  // 退
    show_font(750, 400, 40, 35, word[12], 0xFFFFFF, plcd);  // 出
    // while (1)
    // {
    //     if(x3 >=700&&x3 <=780 && y3 >=380&&y3 <=460)
    //     {
    //         display_management_interface(plcd);
    //         break;
    //     }
       
    // }
    
}

