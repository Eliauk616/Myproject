#ifndef __GRFID_H__
#define __GRFID_H__

//对应开发板的三个串口文件路径
#define COM2 "/dev/ttySAC1"
#define COM3 "/dev/ttySAC2"
#define COM4 "/dev/ttySAC3" 

extern unsigned int cardid;
extern char cardBuff[4];
extern unsigned char passwd[6];
extern char loft[20];
/*
    串口初始化函数
    file:表示你需要初始化的串口，这里填COM2或者COM3或者COM4
    baudrate:波特率，假设设置波特率为9600.那么这里就填入9600
*/
int serial_init(const char *file, int baudrate);
//校验和验证函数
unsigned char CalBcc(unsigned char *buf);

/*
    寻卡
    fd:表示串口文件的文件描述符
*/
int PiccRequest(int fd);

/*
    防冲突
    fd:表示串口文件的文件描述符
*/
int PiccAnticoll(int fd);

/*
    选卡
    fd:表示串口文件的文件miaoshuf
*/
int PiccSelect(int fd);

/*
    获取卡号
  fd:串口文件描述符
*/
void getcard(int fd);

/*
    秘钥验证
    fd:串口文件描述符
    AorB:是A秘钥还是B秘钥
    uid：保存卡号的数组
    passwd：保存密码的数组
    block_num:扇区编号
*/
int PassWD(int fd, char AorB, char *uid, unsigned char *passwd,char block_num);

/*
    读数据
    fd:串口文件描述符
    num_bolck:扇区编号
    lodt:用于保存读取的数据
*/
void ReadData(int fd, char num_block, char *loft);


/*
    写数据
    fd:串口文件描述符号
    num_block:扇区编号
    message：需要写入的内容
*/
void WriteData(int fd, char num_block, char *message); 


#endif