#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "main.h"  
  
//////////////////////////////////////////////////////////////////////////////////	 
/********************************************************************************/
//STM32F103ZE核心板
//2.4寸/2.8寸/3.2寸 TFT液晶驱动	  
//支持驱动IC型号包括:ILI9341   
//触摸屏驱动(支持ADS7843/XPT2046/TSC2046)代码	   

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/********************************************************************************/			 
//////////////////////////////////////////////////////////////////////////////////

#define TP_PRES_DOWN 0x80  		//触屏被按下	  
#define TP_CATH_PRES 0x40  		//有按键按下了 
#define CT_MAX_TOUCH  5    		//电容屏支持的点数,固定为5点

//触摸屏控制器
typedef struct 
{
	uint8_t (*init)(void);			//初始化触摸屏控制器
	uint8_t (*scan)(uint8_t);				//扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	void (*adjust)(void);		//触摸屏校准 
	uint16_t x[CT_MAX_TOUCH]; 		//当前坐标
	uint16_t y[CT_MAX_TOUCH];		//电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
								//x[4],y[4]存储第一次按下时的坐标. 
	uint8_t  sta;					//笔的状态 
								//b7:按下1/松开0; 
	                            //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
	
	
/////////////////////触摸屏校准参数(电容屏不需要校准)//////////////////////		
	
	float xfac;					
	float yfac;
	float xoff;
	float yoff;	 

  
//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
//b1~6:保留.
//b7:0,电阻屏
//   1,电容屏 
	uint8_t touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev;	 	//触屏控制器在touch.c里面定义

//PB1--TPEN, 
//PB2--T-MISO  初始化设置
//PB0--TSCK,  初始化设置
//PC13--T-CS
//PF11--T-MOSI

//电阻屏芯片连接引脚	   

//#define PEN  		PBin(1)    	//PB1--TPEN, 
//#define DOUT 		PBin(2)     //PB2--T-MISO  初始化设置	
//#define TDIN 		PFout(11)  	//PF11--T-MOSI
//#define TCLK 		PBout(0)  	//PB0--TSCK,  初始化设置
//#define TCS  		PCout(13)  	//PC13--T-CS
//   
//电阻屏函数
void TP_Write_Byte(uint8_t num);						//向控制芯片写入一个数据
uint16_t TP_Read_AD(uint8_t CMD);							//读取AD转换值
uint16_t TP_Read_XOY(uint8_t xy);							//带滤波的坐标读取(X/Y)
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y);					//双方向读取(X+Y)
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y);					//带加强滤波的双方向坐标读取
void TP_Drow_Touch_Point(uint16_t x,uint16_t y,uint16_t color);//画一个坐标校准点
void TP_Draw_Big_Point(uint16_t x,uint16_t y,uint16_t color);	//画一个大点
void TP_Save_Adjdata(void);						//保存校准参数
uint8_t TP_Get_Adjdata(void);						//读取校准参数
void TP_Adjust(void);							//触摸屏校准


void TP_Adj_Info_Show(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3,uint16_t fac);//显示校准信息
//电阻屏/电容屏 共用函数
uint8_t TP_Scan(uint8_t tp);								//扫描
uint8_t TP_Init(void);								//初始化
 
#endif

















