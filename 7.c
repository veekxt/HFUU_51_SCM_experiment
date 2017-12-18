#include <REG51.H>
#include <intrins.h>

#define AddWr 0x90   //写数据地址
#define AddRd 0x91   //读数据地址

typedef  unsigned char uchar;

sbit DS = P2 ^ 2;
sbit SH = P2 ^ 1;
sbit ST = P2 ^ 0;
sbit SDA = P1 ^ 0;
sbit SCL = P1 ^ 1;
uchar ys;

uchar code duan[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0xbf}; //段码
uchar wei[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; //位码
uchar disbuf[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //选择段码
uchar disbitcnt;


void c595_in (unsigned char Data)

{
    unsigned char i;
    for (i = 0; i < 8; i++)
        {
            SH = 0;
            _nop_();
            DS = (Data & 0x80) == 0x80 ? 1 : 0;
            Data <<= 1;
            SH = 1;
            _nop_();
        }
}
void c595_out (void)
{
    ST = 0;
    _nop_();
    ST = 1;
}
void Start(void)//启动IIC总线
  {
   SDA=1;
   _nop_();
   SCL=1;
   _nop_();
   SDA=0;
   _nop_();
   SCL=0;
}
void Stop(void)//停止IIC总线
{
   SDA=0;
   _nop_();
   SCL=1;
   _nop_();
   SDA=1;
   _nop_();
   SCL=0;
}

void Ack(void)//应答IIC总线
{
    SDA=0;
	_nop_();
	SCL=1;
	_nop_();
	SCL=0;
	_nop_();
}

void NoAck(void)//非应答IIC总线
{
	 SDA=1;
	 _nop_();
	 SCL=1;
	 _nop_();
	 SCL=0;
	 _nop_();
}
void Send(unsigned char Data)//发送一个字节
{
    unsigned char BitCounter=8;
    unsigned char temp;

    do
    {
        temp=Data;
        SCL=0;
        _nop_();
        if((temp&0x80)==0x80)
        SDA=1;
        else
        SDA=0;

        SCL=1;
        temp=Data<<1;
        Data=temp;
        BitCounter--;
        }
	  while(BitCounter);
	      SCL=0;
}

unsigned char Read(void)//读入一个字节并返回
{
	   unsigned char temp=0;
	   unsigned char temp1=0;
	   unsigned char BitCounter=8;

	   SDA=1;
	   do
	     {
		  SCL=0;
          _nop_();
		  SCL=1;
		  _nop_();
		  if(SDA)
		     temp=temp|0x01;
		  else
		     temp=temp&0xfe;

		  if(BitCounter-1)
		     {
			  temp1=temp<<1;
			  temp=temp1;
			  }
			  BitCounter--;
			 }
		while(BitCounter);
return(temp);
}

unsigned char ReadADC(unsigned char Chl)//读取AD模数转换的值，有返回值
{
       unsigned char Data;
		   Start();        //写入芯片地址
		   Send(AddWr);
		   Ack();
 		   Send(0x40|Chl);
		                  //Chl的值分别为0、1、2、3，分别代表1-4通道
		   Ack();
		   Start();
		   Send(AddRd);    //读入地址
		   Ack();
		   Data=Read();    //读数据
		   SCL=0;
		   NoAck();
		   Stop();
		   return Data;   //返回值
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
			ys=ReadADC(0);
            disbitcnt++;
            if (disbitcnt == 8)
                {
                    disbitcnt = 5;
                }
			disbuf[5] = ys / 100;
            disbuf[6] = (ys % 100) /10;
            disbuf[7] = ys % 10;

		}
}
/*void delay()		//@12.000MHz
{
	unsigned char i;

	i = 3;
	while (--i);
}*/
