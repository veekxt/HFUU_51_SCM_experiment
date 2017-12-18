#include <REG52.H>
#include <intrins.h>
sbit SDA1 = P2^2;//串行数据输入
sbit SCL1 = P2^1;//移位寄存器
sbit SCL2 = P2^0;//存储寄存器
unsigned char k=0,i=0;
unsigned char code dispcode[]= {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0xbf}; //段码
unsigned char dispbitcode[]= {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}; //位码
unsigned char dispbuf[8]= {0,0,0,0,0,0,0,0}; //选择段码
unsigned char dispbitcnt;
unsigned char second; //时  分 秒
unsigned char minite;
unsigned char hour;

void d595_in(unsigned char Data) //传送数据

{
    unsigned char i;

    for(i = 0; i < 8; i++)   //循环8次，刚好移完8位
    {
        SCL1 = 0;         //先将移位寄存器控制引脚置为低
        _nop_();
        if((Data & 0x80)== 0x80)
            SDA1 =1;
        else
            SDA1 =0;
        Data <<= 1;           //将数据的次高位移到最高位
        SCL1 = 1;            //再置为高，产生移位时钟上升沿，上升沿时数据寄存器的数据移位
        _nop_();
    }
}
void d595_out(void) //输出
{
    SCL2 = 0;   //先将存储寄存器引脚置为低
    _nop_();
    SCL2 = 1;    //再置为高，产生移位时钟上升沿，上升沿时移位寄存器的数据进入数据存储寄存器，更新显示数据。
}

void main()
{
    TMOD=0x10;                 //T1定时器方式1，手动恢复初值
    TH1=(65536-50000)/256;//500ms 延时
    TL1=(65536-50000)%256;
    TR1=1;
    IT1=0;
	EX1=1;
    ET1=1;
    EA=1;
    while(1)
    {

            d595_in(0xff);
            d595_in(0xff);
            d595_out();
            d595_in(dispbitcode[dispbitcnt]);
            d595_in(dispcode[dispbuf[dispbitcnt]]);
            d595_out();
            dispbitcnt++;
            if(dispbitcnt==8)
            {
                dispbitcnt=0;
            }

        if(k==20)                   //20个50ms延时正好是1s
        {
            k=0;
            second++;
            if(second==60)
            {
                second=0;
                minite++;
                if(minite==60)
                {
                    minite=0;
                    hour++;
                    if(hour==24)
                    {
                        hour=0;
                    }
                }
            }
            dispbuf[5]=second%10; //3、7位显示时钟
            dispbuf[4]=second/10;
            dispbuf[3]=minite%10;
            dispbuf[2]=minite/10;
            dispbuf[1]=hour%10;
            dispbuf[0]=hour/10;
		
        }
	if(i<100){
	//0、1位按键计数
	dispbuf[6]=i/10;
	dispbuf[7]=i%10;}
	else{dispbuf[6]=0;
	dispbuf[7]=0;i=0;}
    }
}
void t1(void) interrupt 3           //定时器1中断
{
    k++;
    TH1=(65536-50000)/256;
    TL1=(65536-50000)%256;
}
void delay(unsigned int time)
{
int i,j;
for(j=time;j>0;j--)
for(i=110;i>0;i--);

}
void I1(void) interrupt 2
{

delay(15);
if(INT1==0){

		i++;

	    }

}
