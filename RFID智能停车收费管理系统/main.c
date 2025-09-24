#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "mytime.h"
#include "font.h"
#include "bmp.h"
#include "event.h"
#include "gRFID.h"
#include "lcd.h"
#include "interface.h"


#if 0
int main()
{

    // 屏幕初始化函数
    int lcd_fd = -1;
    unsigned int *plcd = init_lcd(&lcd_fd);


    // 初始化串口
    int serial_fd = serial_init(COM3, 9600);
 

    
    //显示开始界面
    int x1=-1,y1=-1;
    // 清屏
    display_re(0, 0, 800, 480, 0xFFFFFF, plcd);

    while(1)
    {
        int f1 = event(&x1,&y1);
        if(f1 == UP)//上划进入管理员界面
        {
            //显示管理员界面
            display_management_interface(plcd);
            int x2 = -1,y2 = -1;
            while (1)
            {
                int A;
                int f2 = event(&x2,&y2);
                printf("点击坐标(%d,%d)\n",x2,y2);
                if(x2>=200&&x2<=600&&y2>=150&&y2<=230)//开卡
                {
                     //注册新用户
                    //存储信息1:车牌号码					1
                    char mes1[32] = "粤A123456";
                    //存储信息2:余额						6
                    char mes2[32] = "0";
                    //存储信息3:出入场标志信息(in/out)		4
                    char mes3[32] = "new";
                    //存储信息4:时间						5
                    char mes4[32] = "none";

                    /* 获取卡号 */
                    getcard(serial_fd);
                    printf("card id is %x\n", cardid);
                    int ret = 0;
                    ret = PassWD(serial_fd, 'B', cardBuff, passwd, 1);
                    if (!ret) 
                    {
                        WriteData(serial_fd, 1, mes1);
                    }
                    ret = PassWD(serial_fd, 'B', cardBuff, passwd, 4);
                    if (!ret) 
                    {
                        WriteData(serial_fd, 4, mes3);
                        WriteData(serial_fd, 5, mes4);
                        WriteData(serial_fd, 6, mes2);
                    }
                    sleep(1);
                    printf("注册成功:\n");

                    ret = PassWD(serial_fd, 'B', cardBuff, passwd, 1);
                    if (!ret) 
                    {
                        ReadData(serial_fd, 1, loft);
                        printf("车牌信息为: %s\n", loft);
                    

                    }
                    ret = PassWD(serial_fd, 'B', cardBuff, passwd, 4);
                    if (!ret) 
                    {
                        ReadData(serial_fd, 6, loft);
                        printf("余额: %s\n", loft);

                        ReadData(serial_fd, 4, loft);
                        printf("状态: %s\n", loft);
                        
                        ReadData(serial_fd, 5, loft);
                        printf("时间: %s\n", loft);
                    } 
                }
                else if(x2>=200&&x2<=600&&y2>=250&&y2<=330)//充值
                {
                    /*充值*/
                    /*获取卡号 */
                    getcard(serial_fd);
                    printf("card id is %x\n",cardid);

                    /*验证秘钥 */
                    int ret = 0;
                    ret = PassWD(serial_fd,'B',cardBuff,passwd,4);
                    if(!ret)
                    {
                        /*读取一下你当前的余额*/ //读取6数据块的内容
                        if(!ret)//ret == 0
                        {
                            //写
                            // char mes1[32] = "内容";
                            // WriteDate(serial_fd,1,mes1); //这样就把数据写入到了1数据块
                            //读
                            ReadData(serial_fd,6,loft);
                            printf("读取到的内容：%s\n",loft);
                        }
                        printf("余额：%s\n",loft);

                        /*充值金额 */
                        int x = 100;
                        int new = atoi(loft) + x;
                        A = new;
                        char str[20] = {0};
                        sprintf(str,"%d",new);
                        printf("str = %s\n",str);

                        /*把新的金额写入到数据块6中去*/
                        WriteData(serial_fd,6,str);

                        //充值成功，最新金额为：
                        ReadData(serial_fd,6,loft);           //读取一下金额，也就是读取数据区6的内容
                        printf("最新金额为：%s\n",loft);

                    }                  
                }
                else if(x2>=200&&x2<=600&&y2>=350&&y2<=430)//退出管理员界面
                {
                   //清屏
                    display_re(0, 0, 800, 480, 0xFFFFFF, plcd);
                    break;
                }
                display_parking_info(123456,A,1,2025,6,5,0,0,plcd);
            }
            
        }
         else if(f1 == DOWN) //下滑进入进出场检测
        {
            while (1)
            {
                /*获取卡号*/
                getcard(serial_fd);
                printf("card id  is %x\n",cardid);

                /*验证秘钥 6区域**/
                int ret = 0;
                ret = PassWD(serial_fd,'B',cardBuff,passwd,4);
                if(!ret)
                {
                    /*读取一下，卡中金额 */
                    ReadData(serial_fd,6,loft);
                    printf("当前金额为%s\n",loft);
                    int num = atoi(loft);

                    if(num <= 0 )
                    {
                        display_re(0, 0, 800, 480, 0xFFFFFF, plcd);
                        break;
                    }
                    
                    ret = PassWD(serial_fd,'B',cardBuff,passwd,4);
                if(!ret)
                {
                    ReadData(serial_fd,4,loft);
                    printf("修改前状态：%s\n",loft);
                    char t1[32] = "input";
                    char t2[32] = "output";
                    char t3[32] = "new";
                    if(!strcmp(loft,t3))//新卡进入停车
                    {
                        WriteData(serial_fd,4,t1);//将状态修改为input
                        //获取时间
                        char *buf = get_time();
                        //写入时间
                        WriteData(serial_fd,5,buf);
                    }
                    else if(!strcmp(loft,t2))//如果状态是output那么说明此时在外面，需要进入停车场
                    {
                        WriteData(serial_fd,4,t1);//将状态修改为input
                        //获取时间
                        char *buf = get_time();
                        //写入时间
                        WriteData(serial_fd,5,buf);
                    }
                    else if(!strcmp(loft,t1))//如果状态是input那么说明此时在里面，需要出停车场
                    {
                        WriteData(serial_fd,4,t2);//将状态修改为output
                        int s = num - 20;
                        char buf[32]  = {0};
                        sprintf(buf,"%d",s);
                        printf("扣掉后的余额为：%s\n",buf);

                        WriteData(serial_fd,6,buf);//把修改后的余额存入数据6区

                        //获取时间
                        char *bufs = get_time();
                        //写入时间
                        WriteData(serial_fd,5,bufs);

                    }
                }
                printf("---------------------------------------------\n");
                sleep(1);
                /*
                    以下获取当前卡牌呢内的数据状态，用于开发板显示
                */
                int money  = -1;//保存余额
                int state = -1;//保存状态
                int y = -1,m = -1,d = -1,h = -1,mi = -1;//保存年，月，日，时，分
                /*获取卡号*/
                getcard(serial_fd);
                printf("卡号：%x\n",cardid);
                //检验秘钥
                int re = -1;
                re = PassWD(serial_fd,'B',cardBuff,passwd,1);
                if(!ret)
                {
                    ReadData(serial_fd,1,loft);
                    printf("车牌信息为：%s\n",loft);
                }

                re = PassWD(serial_fd,'B',cardBuff,passwd,4);
                if(!ret)
                {
                    ReadData(serial_fd,6,loft);
                    money = atoi(loft);
                    printf("金额为：%s\n",loft);

                    ReadData(serial_fd,4,loft);
                    if(!strcmp(loft,"input"))
                    {
                        state = 0;//表示在停车场内
                    }
                    else if(!strcmp(loft,"output"))
                    {
                        state = 1;//表示在停车场外
                    }
                    printf("状态：%s\n",loft);

                    ReadData(serial_fd,5,loft);
                    printf("时间：%s\n",loft);
                    //将时间字符串修改为整形数据保存，用于后面显示
                    change_time(loft,&y,&m,&d,&h,&mi);
                }
                //显示信息界面
                display_parking_info(123456,money, state, y,m,d,h,mi, plcd);//卡号，车牌，年，月，日，时，分，状态，余额

                getchar();//按下回车继续
            }
        }
        }
        else 
        {           //不是上，下滑动就退出
            printf("程序结束\n");
            break;
        }
    }
    
    // 关闭
    close_lcd(plcd, lcd_fd);
    close(serial_fd);
    return 0;
}
#endif

int main()
{
    //初始化屏幕
    int lcd_fd = -1;
    unsigned int * plcd =   init_lcd(&lcd_fd);

    //初始化串口
    int serial_fd =  serial_init(COM2,9600);

    //显示一个开始界面
    display_bmp("5.bmp",0,0,plcd);
    int x1 = -1,y1 = -1;
    while (1)
    {   
        int f1 = event(&x1,&y1);
        if(f1 == UP)//上划进入管理员界面
        {
            //显示管理员届满
            display_management_interface(plcd);
            int x2 = -1,y2 = -1;
            while (1)
            {
                int A;
                int f2 = event(&x2,&y2);
                printf("点击坐标(%d,%d)\n",x2,y2);
                if(x2>=200&&x2<=600&&y2>=150&&y2<=230)//开卡
                {
                     //注册新用户
                    //存储信息1:车牌号码					1
                    char mes1[32] = "湘A888888";
                    //存储信息2:余额						6
                    char mes2[32] = "0";
                    //存储信息3:出入场标志信息(in/out)		4
                    char mes3[32] = "new";
                    //存储信息4:时间						5
                    char mes4[32] = "none";

                    /* 获取卡号 */
                    getcard(serial_fd);
                    printf("card id is %x\n", cardid);
                    int ret = 0;
                    ret = PassWD(serial_fd, 'B', cardBuff, passwd, 1);
                    if (!ret) 
                    {
                        WriteData(serial_fd, 1, mes1);
                    }
                    ret = PassWD(serial_fd, 'B', cardBuff, passwd, 4);
                    if (!ret) 
                    {
                        WriteData(serial_fd, 4, mes3);
                        WriteData(serial_fd, 5, mes4);
                        WriteData(serial_fd, 6, mes2);
                    }
                    sleep(1);
                    printf("注册成功:\n");

                    ret = PassWD(serial_fd, 'B', cardBuff, passwd, 1);
                    if (!ret) 
                    {
                        ReadData(serial_fd, 1, loft);
                        printf("车牌信息为: %s\n", loft);
                    

                    }
                    ret = PassWD(serial_fd, 'B', cardBuff, passwd, 4);
                    if (!ret) 
                    {
                        ReadData(serial_fd, 6, loft);
                        printf("余额: %s\n", loft);

                        ReadData(serial_fd, 4, loft);
                        printf("状态: %s\n", loft);
                        
                        ReadData(serial_fd, 5, loft);
                        printf("时间: %s\n", loft);
                    } 
                }
                else if(x2>=200&&x2<=600&&y2>=250&&y2<=330)//充值
                {
                    /*充值*/
                    /*获取卡号 */
                    getcard(serial_fd);
                    printf("card id is %x\n",cardid);

                    /*验证秘钥 */
                    int ret = 0;
                    ret = PassWD(serial_fd,'B',cardBuff,passwd,4);
                    if(!ret)
                    {
                        /*读取一下你当前的余额*/ //读取6数据块的内容
                        if(!ret)//ret == 0
                        {
                            //写
                            // char mes1[32] = "内容";
                            // WriteDate(serial_fd,1,mes1); //这样就把数据写入到了1数据块
                            //读
                            ReadData(serial_fd,6,loft);
                            printf("读取到的内容：%s\n",loft);
                        }
                        printf("余额：%s\n",loft);

                        /*充值金额 */
                        int x = 100;
                        int new = atoi(loft) + x;
                        A = new;
                        char str[20] = {0};
                        sprintf(str,"%d",new);
                        printf("str = %s\n",str);

                        /*把新的金额写入到数据块6中去*/
                        WriteData(serial_fd,6,str);

                        //充值成功，最新金额为：
                        ReadData(serial_fd,6,loft);           //读取一下金额，也就是读取数据区6的内容
                        printf("最新金额为：%s\n",loft);

                    }
                }
                else if(x2>=200&&x2<=600&&y2>=350&&y2<=430)//退出管理员界面
                {
                    display_bmp("5.bmp",0,0,plcd);//在运行一下上面第24行的显示一个界面的代码
                    break;
                }
                //调用显示车牌，金额，状态，时间的函数
                display_parking_info(888888,A, 1, 2025,6,5,0,0, plcd);
            }
            
        }
         else if(f1 == DOWN) //下滑进入进出场检测
        {
            while (1)
            {
                /*获取卡号*/
                getcard(serial_fd);
                printf("card id  is %x\n",cardid);

                /*验证秘钥 6区域**/
                int ret = 0;
                ret = PassWD(serial_fd,'B',cardBuff,passwd,4);
                if(!ret)
                {
                    /*读取一下，卡中金额 */
                    ReadData(serial_fd,6,loft);
                    printf("当前金额为%s\n",loft);
                    int num = atoi(loft);

                    if(num <= 0 )
                    {
                        display_bmp("6.bmp",0,0,plcd);
                        break;
                    }
                    
                    ret = PassWD(serial_fd,'B',cardBuff,passwd,4);
                if(!ret)
                {
                    ReadData(serial_fd,4,loft);
                    printf("修改前状态：%s\n",loft);
                    char t1[32] = "input";
                    char t2[32] = "output";
                    char t3[32] = "new";
                    if(!strcmp(loft,t3))//新卡进入停车
                    {
                        WriteData(serial_fd,4,t1);//将状态修改为input
                        //获取时间
                        char *buf = get_time();
                        //写入时间
                        WriteData(serial_fd,5,buf);
                    }
                    else if(!strcmp(loft,t2))//如果状态是output那么说明此时在外面，需要进入停车场
                    {
                        WriteData(serial_fd,4,t1);//将状态修改为input
                        //获取时间
                        char *buf = get_time();
                        //写入时间
                        WriteData(serial_fd,5,buf);
                    }
                    else if(!strcmp(loft,t1))//如果状态是input那么说明此时在里面，需要出停车场
                    {
                        WriteData(serial_fd,4,t2);//将状态修改为output
                        int s = num - 20;
                        char buf[32]  = {0};
                        sprintf(buf,"%d",s);
                        printf("扣掉后的余额为：%s\n",buf);

                        WriteData(serial_fd,6,buf);//把修改后的余额存入数据6区

                        //获取时间
                        char *bufs = get_time();
                        //写入时间
                        WriteData(serial_fd,5,bufs);

                    }
                }
                printf("---------------------------------------------\n");
                sleep(1);
                /*
                    以下获取当前卡牌呢内的数据状态，用于开发板显示
                */
                int yu_e  = -1;//保存余额
                int status = -1;//保存状态
                int y = -1,m = -1,d = -1,h = -1,mi = -1;//保存年，月，日，时，分
                /*获取卡号*/
                getcard(serial_fd);
                printf("卡号：%x\n",cardid);
                //检验秘钥
                int re = -1;
                re = PassWD(serial_fd,'B',cardBuff,passwd,1);
                if(!ret)
                {
                    ReadData(serial_fd,1,loft);
                    printf("车牌信息为：%s\n",loft);
                }

                re = PassWD(serial_fd,'B',cardBuff,passwd,4);
                if(!ret)
                {
                    ReadData(serial_fd,6,loft);
                    yu_e = atoi(loft);
                    printf("金额为：%s\n",loft);

                    ReadData(serial_fd,4,loft);
                    if(!strcmp(loft,"input"))
                    {
                        status = 0;//表示在停车场内
                    }
                    else if(!strcmp(loft,"output"))
                    {
                        status = 1;//表示在停车场外
                    }
                    printf("状态：%s\n",loft);

                    ReadData(serial_fd,5,loft);
                    printf("时间：%s\n",loft);
                    //将时间字符串修改为整形数据保存，用于后面显示
                    change_time(loft,&y,&m,&d,&h,&mi);
                }
                //显示信息界面
                display_parking_info(888888,yu_e, status, y,m,d,h,mi, plcd);

                getchar();//按下回车继续
            }
        }
        }
        else 
        {           //不是上，下滑动就退出
            printf("程序结束\n");
            break;
        }
    
    }

    close_lcd(plcd,lcd_fd);
    close(serial_fd);
    return 0;
}