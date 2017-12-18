#include<reg51.h>
#include<intrins.h>
#define delaytime 1500		 //黑屏时间：（数值） ms
#define lighttime 15		 //亮屏时间：（数值*50） ms
//以上两个数值决定闪烁效果

typedef unsigned char uchar;
typedef unsigned int uint;

uchar k=0,key=0;
sbit p1_0=P1^0;
sbit p1_1=P1^1;
sbit p1_2=P1^2;
sbit p1_3=P1^3;
sbit p1_4=P1^4;

sbit SHC = P0^1;		//定义595寄存器和输入端
sbit STC = P0^0;
sbit DSC = P0^2;
sbit SHG = P2^1;
sbit STG = P2^2;
sbit DSG = P2^0;
//
uchar code shang[]={0xF7,0xE3,0xD5,0xB6,0xF7,0xF7,0xF7,0xF7};
uchar code xia[]={0xF7,0xF7,0xF7,0xF7,0xB6,0xD5,0xE3,0xF7};
uchar code zuo[]={0xF7,0xFB,0xFD,0x00,0xFD,0xFB,0xF7,0xFF};
uchar code you[]={0xEF,0xDF,0xBF,0x00,0xBF,0xDF,0xEF,0xFF};
uchar code zhong[]={0xF7,0xEB,0xDD,0xBE,0xDD,0xEB,0xF7,0xFF};
//
uchar code hang[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
void clr_8x8();
void displayshang();
void displayxia();
void displayzuo();
void displayyou();
void displayzhong();
/*为三个595送数据的函数，颜色控制端和公共端*****/
void d595_in(uchar hong,uchar lv,uchar gg)	 
{
    uchar i;
	for(i = 0; i < 8; i++)  
    {
        SHC = 0;       
        _nop_();
        if((hong & 0x80)== 0x80)
            DSC =1;
        else
            DSC =0;
        hong <<= 1;           
        SHC = 1;
        _nop_();
    }
    for(i = 0; i < 8; i++)  
    {
        SHC = 0;       
        _nop_();
        if((lv & 0x80)== 0x80)
            DSC =1;
        else
            DSC =0;
        lv <<= 1;           
        SHC = 1;
        _nop_();
    }


	    for(i = 0; i < 8; i++)  
    {
        SHG = 0;       
        _nop_();
        if((gg & 0x80)== 0x80)
            DSG =1;
        else
            DSG =0;
        gg <<= 1;           
        SHG = 1;
        _nop_();
    }
}

/******595输出*******/
void d595_out(void) 
{
	STG=0;
	_nop_();
	STG=1;
    STC = 0;
    _nop_();
    STC = 1;

}
/**************
使用定时器0定时扫描按键（五向摇杆）
定时器1定时清屏（产生闪烁效果）
主函数进行点阵的动态显示
***************/
void main()
{
TMOD=0x11;
TH0=55536/256;	//10ms定时初值
TL0=55536%256;
TH1=15536/256; 	//50ms定时初值
TL1=15536%256;
ET0=1;
ET1=1;
PT0=1;
EA=1;
TR0=1;
TR1=1;
while(1)
switch(key)
{
	case 1:displayshang();break;
	case 2:displayzuo();break;
	case 3:displayxia();break;
	case 4:displayyou();break;
	case 5:displayzhong();break;
	default :break;
}
}

void delay(uint time)			 //延时函数，大约延时 time ms
{
	uint x,y;
	for(x=time;x>0;x--)
		for(y=110;y>0;y--);
}

void t0(void) interrupt 1		 //定时中断0，检测按键（摇杆）
{
if(p1_0==0)key=1;
if(p1_1==0)key=2;
if(p1_2==0)key=3;
if(p1_3==0)key=4;
if(p1_4==0)key=5;
TH0=55536/256;					//10ms定时初值
TL0=55536%256;
}

void t1(void) interrupt 3
{
++k;
if(k==lighttime){clr_8x8();delay(delaytime);k=0;}
TH1=15536/256; 	//50ms定时初值
TL1=15536%256;
}
//清屏函数//
void clr_8x8()
{
	d595_in(0xff,0xff,0);
	d595_out();
}

//上 下 左 右 中 的显示函数//
void  displayshang(void)
{
uchar i;
for(i=0;i<8;i++)		             //	逐行显示，8行
	{							  
	clr_8x8();						 //	 清屏
	d595_in(0xff,shang[i],hang[i]);	  //传公共端（第i行）颜色端
	d595_out();					   	//595输出
	delay(1);						 //	延时产生显示效果
	}
}

void displayxia(void)
{
uchar i;
for(i=0;i<8;i++)
	{
	clr_8x8();
	d595_in(0xff,xia[i],hang[i]);	//把0xff和xia[i]互换会显示红色，其他函数也一样
	d595_out();
	delay(1);
	}
}

void displayzuo(void)
{
uchar i;
for(i=0;i<8;i++)
	{
	clr_8x8();
	d595_in(0xff,zuo[i],hang[i]);
	d595_out();
	delay(1);
	}
}

void displayyou(void)
{
uchar i;
for(i=0;i<8;i++)
	{
	clr_8x8();
	d595_in(0xff,you[i],hang[i]);
	d595_out();
	delay(1);	 
	}
}

void displayzhong(void)
{
uchar i;
for(i=0;i<8;i++)
	{
	clr_8x8();
	d595_in(0xff,zhong[i],hang[i]);
	d595_out();
	delay(1);
	}
}