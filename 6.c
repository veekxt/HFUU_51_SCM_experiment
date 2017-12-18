/**********

*******晶振使用12Mhz**************
*******波特率选择4800*************

***********/
#include <reg51.h>
#include <intrins.h>
#define xiang 1
sfr AUXR = 0x8e;
sfr BRT = 0x9c; 
//定义位变量
sbit DS = P2 ^ 2;
sbit SH = P2 ^ 1;
sbit ST = P2 ^ 0;
sbit P1_0 = P1 ^ 0;

typedef  unsigned char uchar;

uchar code duan[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0xbf};//段码
uchar wei[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; //位码
uchar disbuf[] = {0, 0, 0, 0};//可以理解为显示缓冲区
uchar key_sum = 0;      //按键计数
uchar received = 0;     //接收的数据
uchar time_50ms = 0;        //50ms延时次数
uchar disbitcnt = 3;        //数码管第几位
uchar time_xiang = 0;       //蜂鸣器响的时间（s）
uchar zt = 0;               //状态标志：是否刚刚倒计时完成
/****************
进一步封装的595写入函数，传送位码和段码
然后在595输出端输出。
****************/
void  hc595 (unsigned char data1, unsigned char data2)
{
	unsigned char i;
	for (i = 0; i < 8; i++)             //循环8次
	{
		SH = 0;
		_nop_();
		DS = (data1 & 0x80) == 0x80 ? 1 : 0;//获取最高位数据
		data1 <<= 1;                      //左移一位，次高位移至最高位
		SH = 1;                         //SH置为高，制造上升沿，传送最高位数据
		_nop_();
	}
	for (i = 0; i < 8; i++)
	{
		SH = 0;
		_nop_();
		DS = (data2 & 0x80) == 0x80 ? 1 : 0;
		data2 <<= 1;
		SH = 1;
		_nop_();
	}
	ST = 0;
	_nop_();
	ST = 1;             //ST上升沿，数据从595端口输出
}
/************
定时器和串口的初始化函数
************/
void UartInit(void)		//4800bps@12.000MHz
{
	PCON |= 0x80;		//使能波特率倍速位SMOD
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xFB;		//独立波特率发生器时钟为Fosc/12,即12T
	BRT = 0xF3;		//设定独立波特率发生器重装值
	AUXR |= 0x01;		//串口1选择独立波特率发生器为波特率发生器
	AUXR |= 0x10;		//启动独立波特率发生器
}

void Init (void)
{
	UartInit();
	IT0 = 1;        //外部中断0跳沿触发
	TMOD = 0x11;        //定时器0：方式1；定时器1：方式1
	TH0 = 60;       //50ms定时初值
	TL0 = 176;      
	TH1 = 0xFC;     //1MS定时初值
	TL1 = 0x18;
	ET0 = 1;        //开定时器中断0
	EX0 = 1;        //开外部中断0
	ES = 1;     //开串口中断
	EA = 1;     //开中断
	ET1 = 0;	 //关定时器1中断
	TR0 = 1;        //启动定时器0
	TR1 = 1;        //启动定时器1
}

void main (void)
{
	Init();

	while (1)
	{
		hc595 (0xff, 0xff);

		disbitcnt++;

		if (disbitcnt == 8)
		{
			disbitcnt = 4;
		}
		hc595 ( wei[disbitcnt],duan[disbuf[disbitcnt - 4]]);
		disbuf[0] = key_sum  / 10;
		disbuf[1] = key_sum  % 10;
		disbuf[2] = received / 10;
		disbuf[3] = received % 10;
		if (time_50ms == 20)        //如果过了 1秒
		{
			SBUF = key_sum;
			if (received != 0)      //
			{
				--received;         //
				zt = 1;             //
			}
			if (received == 0 && zt == 1)   //如果zt==1，说明第一次检测到received == 0，
			{
				ET1 = 1;                //让蜂鸣器响
				time_xiang = 0;             //响的时间从0开始计数
				zt = 0;                     //zt置零，防止多次检测
			}
			time_xiang++;           //对响的时间计数
			if (time_xiang == 2)    //响2秒，
			{
				ET1 = 0;            //则停止。
			}
			time_50ms = 0;                  //50ms重新计时
			                 //串口每一秒传送一次key——sum
		}
	}
}
/* 20ms延时函数 */
void delay20ms (void)
{
	unsigned char a, b;
	for (b = 215; b > 0; b--)
		for (a = 45; a > 0; a--);
}
/* 串口中断函数 */
void UARTInterrupt (void) interrupt 4
{
	if (RI==1)
	{
		RI = 0;         //
		received = SBUF;//
		time_50ms = 0;  //
	}
	else
	{
		TI = 0;
	}
}
/* 外部中断0中断函数 */
void INT0Interrupt (void) interrupt 0
{
	delay20ms();
	if (INT0 == 0)
	{
		key_sum++;
		if (key_sum == 100)
		{
			key_sum = 0;
		}
	}
}
/* 定时器中断0中断函数 */
void T0Interrupt (void) interrupt 1
{
	time_50ms++;
}
/* 定时器中断1中断函数 */
void T1Interrupt (void) interrupt 3
{
	P1_0=~P1_0;
}
