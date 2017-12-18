#include <REG51.H>
#include <intrins.h>
sbit DS = P2 ^ 2; //串行数据输入
sbit SH = P2 ^ 1; //移位寄存器
sbit ST = P2 ^ 0; //存储寄存器
typedef  unsigned char uchar;

uchar code duan[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0xbf}; //段码
uchar wei[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; //位码
uchar disbuf[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //选择段码
uchar disbitcnt;


void c595_in (unsigned char Data) //传送数据

{
    unsigned char i;
    for (i = 0; i < 8; i++)  //循环8次，刚好移完8位
        {
            SH = 0;        //先将移位寄存器控制引脚置为低
            _nop_();
            DS = (Data & 0x80) == 0x80 ? 1 : 0;
            Data <<= 1;           //将数据的次高位移到最高位
            SH = 1;            //再置为高，产生移位时钟上升沿，上升沿时数据寄存器的数据移位
            _nop_();
        }
}
void c595_out (void) //输出
{
    ST = 0;  //先将存储寄存器引脚置为低
    _nop_();
    ST = 1;    //再置为高，产生移位时钟上升沿，上升沿时移位寄存器的数据进入数据存储寄存器，更新显示数据。
}

void main()
{
    while (1)
        {

            c595_in (0xff);         //清屏
            c595_in (0xff);
            c595_out();
            c595_in (wei[disbitcnt]);
            c595_in (duan[disbuf[disbitcnt]]);
            c595_out();
            disbitcnt++;
            if (disbitcnt == 8)
                {
                    disbitcnt = 4;
                }
                    disbuf[4] = k0 / 10;
                    disbuf[5] = k0 % 10;
                    disbuf[6] = k1 / 10;
                    disbuf[7] = k1 % 10;

}



void T_0 (void) interrupt 1
{

}
void T_0 (void) interrupt 3
{

}
void I1 (void) interrupt 2
{

}
void I0 (void) interrupt 0	
{

}
void UARTInterrupt (void) interrupt 4
{
	if (RI==1)
	{
		RI = 0;         

	}
	else
	{
		TI = 0;
	}
}
