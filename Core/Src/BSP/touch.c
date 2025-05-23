#include "touch.h" 
#include "lcd.h"

#include "stdlib.h"
#include "math.h"


#include "w25qxx.h" 


//#include "GBK_LibDrive.h"	
	
#include <string.h>
#include  <stdio.h>
#include  <stdlib.h>



_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};			


//默认为touchtype=0的数据.
uint8_t CMD_RDX=0XD0;
uint8_t CMD_RDY=0X90;


/********************************************************************************/

//2.4寸/2.8寸/3.2寸 TFT液晶驱动	  
//支持驱动IC型号包括:ILI9341   
//触摸屏驱动(支持ADS7843/XPT2046/TSC2046)代码	   

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/********************************************************************************/

//SPI写数据
//向触摸屏IC写入1byte数据    
//num:要写入的数据
//void TP_Write_Byte(uint8_t num)    
//{  
//	uint8_t count=0;   
//	for(count=0;count<8;count++)  
//	{ 	  
//		if(num&0x80)TDIN=1;  
//		else TDIN=0;   
//		num<<=1;    
//		TCLK=0; 
//		delay_us(1);
//		TCLK=1;		//上升沿有效	        
//	}		 			    
//} 
/**
  * @brief  ?? HAL GPIO ???? IC ?? 1 ??? (?? SPI)
  * @param  num: ??????
  * @retval None
  */
void TP_Write_Byte(uint8_t num)
{
    uint8_t count = 0;
    for (count = 0; count < 8; count++)
    {
        // 1. ????? (MOSI)
        if (num & 0x80) {
            HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, GPIO_PIN_SET); // TDIN = 1
        } else {
            HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, GPIO_PIN_RESET); // TDIN = 0
        }
        num <<= 1; // ?????

        // 2. ?????? (SCK ? -> ?)
        HAL_GPIO_WritePin(TSCK_GPIO_Port, TSCK_Pin, GPIO_PIN_RESET); // TCLK = 0
        HAL_Delay(1);// ????
        HAL_GPIO_WritePin(TSCK_GPIO_Port, TSCK_Pin, GPIO_PIN_SET); // TCLK = 1 (???)
        HAL_Delay(1); // ?????????? (??,????????)
                     // ??:?????????????????????????
                     //       ???????????????? SPI ??

        // ????????,???????????????
        // ?????????,?????0??????
        // HAL_GPIO_WritePin(TCLK_GPIO_Port, TCLK_Pin, GPIO_PIN_RESET);
        // delay_us(1); // ???????
    }
     // ?????,????????????(??????)
    HAL_GPIO_WritePin(TSCK_GPIO_Port, TSCK_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  ?? HAL GPIO ???? IC ?? ADC ? (?? SPI)
  * @param  CMD: ???? (?? CMD_RDX ? CMD_RDY)
  * @retval uint16_t: ???? 12 ? ADC ?
  */
uint16_t TP_Read_AD(uint8_t CMD)
{
    uint8_t count = 0;
    uint16_t Num = 0;

    // 1. ????? & ??
    HAL_GPIO_WritePin(TSCK_GPIO_Port, TSCK_Pin, GPIO_PIN_RESET);      // TCLK = 0 (?????)
    HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, GPIO_PIN_RESET);  // TDIN = 0 (MOSI ???,???????????)
    HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, GPIO_PIN_RESET);      // TCS = 0 (????? IC)

    // 2. ??????
    TP_Write_Byte(CMD); // ????? HAL ????

    // 3. ???? (?????????)
    HAL_Delay(1); // ADS7846/XPT2046 ????

    // 4. ?? BUSY ?? (???????,??????)
    //    (??:??????????????)
    HAL_GPIO_WritePin(TSCK_GPIO_Port, TSCK_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(TSCK_GPIO_Port, TSCK_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(TSCK_GPIO_Port, TSCK_Pin, GPIO_PIN_RESET);
    HAL_Delay(1); // ?????????

    // 5. ?? 16 ???
    for (count = 0; count < 16; count++)
    {
        Num <<= 1; // ?????????

        // ?????? (? -> ?)
        HAL_GPIO_WritePin(TSCK_GPIO_Port, TSCK_Pin, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(TSCK_GPIO_Port, TSCK_Pin, GPIO_PIN_SET); // ???

        // ???????????? (MISO)
        if (HAL_GPIO_ReadPin(T_MISO_GPIO_Port, T_MISO_Pin) == GPIO_PIN_SET) // if(DOUT)
        {
            Num++; // ?? MISO ????,????? 1
        }
         HAL_Delay(1); // ?????????? (??)

        // ???????,??????????????
        // ??????
        // HAL_GPIO_WritePin(TCLK_GPIO_Port, TCLK_Pin, GPIO_PIN_RESET);
        // delay_us(1);
    }
     // ?????,????????????(??????)
     HAL_GPIO_WritePin(TSCK_GPIO_Port, TSCK_Pin, GPIO_PIN_RESET);


    // 6. ???? & ????
    Num >>= 4;   // ??? 12 ?????
    HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, GPIO_PIN_SET); // TCS = 1 (????)

    return Num;
}
/********************************************************************************/

////2.4寸/2.8寸/3.2寸 TFT液晶驱动	  
////支持驱动IC型号包括:ILI9341   
////触摸屏驱动(支持ADS7843/XPT2046/TSC2046)代码	   

////DevEBox  大越创新
////淘宝店铺：mcudev.taobao.com
////淘宝店铺：shop389957290.taobao.com	

///********************************************************************************/

////SPI读数据 
////从触摸屏IC读取adc值
////CMD:指令
////返回值:读到的数据	   
//uint16_t TP_Read_AD(uint8_t CMD)	  
//{ 	 
//	uint8_t count=0; 	  
//	uint16_t Num=0; 
//	TCLK=0;		//先拉低时钟 	 
//	TDIN=0; 	//拉低数据线
//	TCS=0; 		//选中触摸屏IC
//	TP_Write_Byte(CMD);//发送命令字
//	delay_us(6);//ADS7846的转换时间最长为6us
//	TCLK=0; 	     	    
//	delay_us(1);    	   
//	TCLK=1;		//给1个时钟，清除BUSY
//	delay_us(1);    
//	TCLK=0; 	     	    
//	for(count=0;count<16;count++)//读出16位数据,只有高12位有效 
//	{ 				  
//		Num<<=1; 	 
//		TCLK=0;	//下降沿有效  	    	   
//		delay_us(1);    
// 		TCLK=1;
// 		if(DOUT)Num++; 		 
//	}  	
//	Num>>=4;   	//只有高12位有效.
//	TCS=1;		//释放片选	 
//	return(Num);   
//}

/********************************************************************************/

//2.4寸/2.8寸/3.2寸 TFT液晶驱动	  
//支持驱动IC型号包括:ILI9341   
//触摸屏驱动(支持ADS7843/XPT2046/TSC2046)代码	   

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/********************************************************************************/

//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
uint16_t TP_Read_XOY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 

/********************************************************************************/

//2.4寸/2.8寸/3.2寸 TFT液晶驱动	  
//支持驱动IC型号包括:ILI9341   
//触摸屏驱动(支持ADS7843/XPT2046/TSC2046)代码	   

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/********************************************************************************/

//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}

/********************************************************************************/

//2.4寸/2.8寸/3.2寸 TFT液晶驱动	  
//支持驱动IC型号包括:ILI9341   
//触摸屏驱动(支持ADS7843/XPT2046/TSC2046)代码	   

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/********************************************************************************/

//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。


#define ERR_RANGE 50 //误差范围 


uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}  

/********************************************************************************/

//2.4寸/2.8寸/3.2寸 TFT液晶驱动	  
//支持驱动IC型号包括:ILI9341   
//触摸屏驱动(支持ADS7843/XPT2046/TSC2046)代码	   

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/********************************************************************************/
//////////////////////////////////////////////////////////////////////////////////		  
//与LCD部分有关的函数  
//画一个触摸点
//用来校准用的
//x,y:坐标
//color:颜色
void TP_Drow_Touch_Point(uint16_t x,uint16_t y,uint16_t color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y,color);//横线
	LCD_DrawLine(x,y-12,x,y+13,color);//竖线
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_Draw_Circle(x,y,6,color);//画中心圈
}	  
//画一个大点(2*2的点)		   
//x,y:坐标
//color:颜色
void TP_Draw_Big_Point(uint16_t x,uint16_t y,uint16_t color)
{	    
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//中心点 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}						  
//////////////////////////////////////////////////////////////////////////////////		  
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
uint8_t TP_Scan(uint8_t tp)
{			   
	if(HAL_GPIO_ReadPin(TPEN_GPIO_Port, TPEN_Pin) == GPIO_PIN_RESET)//有按键按下
	{
		if(tp)TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//读取物理坐标
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//读取屏幕坐标
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//将结果转换为屏幕坐标
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//之前没有被按下
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下  
			tp_dev.x[4]=tp_dev.x[0];//记录第一次按下时的坐标
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}			   
	}
	else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);//标记按键松开	
		}else//之前就没有被按下
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}	  
//////////////////////////////////////////////////////////////////////////	 
//保存在 W25Q16 里面的地址区间基址,占用14个字节(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+13)


#define SAVE_ADDR_BASE 40

//建立一个结构体

union TData
{
   int32_t B32_temp;
   int8_t  B8_Temp[4];
} TData; 

 /**********************************初始化缓冲区为0x0或者0xFF**********************************/ 
 
void Init_Buffer (unsigned char *P,unsigned int Count,unsigned char Type) 

{
  unsigned int i;
  
   if(Type==0)
   {
   
     for(i=0;i<Count;i++)*P++=0x0;
   
   }
   else
   {
     for(i=0;i<Count;i++)*P++=0xFF;

   }

}





//保存校准参数										    
void TP_Save_Adjdata(void)
{
		  
  	uint8_t Flash_Temp[24];
	
    //保存校正结果!
    TData.B32_temp = tp_dev.xfac * 100000000; //保存x校正因素
	  memcpy(Flash_Temp,TData.B8_Temp,4);
	  
	  TData.B32_temp = tp_dev.yfac * 100000000; //保存y校正因素
    memcpy(&Flash_Temp[4],TData.B8_Temp,4);
	 	
    //保存x偏移量
    
	  TData.B32_temp=tp_dev.xoff;//保存x偏移量
	  memcpy(&Flash_Temp[4*2],TData.B8_Temp,4);
	
    //保存y偏移量
    TData.B32_temp=tp_dev.yoff;//保存y偏移量
	  memcpy(&Flash_Temp[4*3],TData.B8_Temp,4);
		
	  tp_dev.touchtype=LCD_DIR_Mode;  //屏幕方向类型
    //保存触屏类型
    Flash_Temp[4*4+1]=tp_dev.touchtype; //保存触屏类型
		
    Flash_Temp[4*4+2]=0X0A;//标记校准过了
		
		W25QXX_Write((uint8_t*)Flash_Temp,0,24);		//从第0个地址处开始,存储校准参数

	
}





//读取 保存在 W25Q16 里面的校准值
//返回值：1，成功获取数据
//        0，获取失败，要重新校准
uint8_t TP_Get_Adjdata(void)
{					  
	int32_t tempfac;
	int32_t Ttype;
	
	uint8_t Flash_Temp[24];
	
	W25QXX_Read(Flash_Temp,0,24);					//从第0个地址处开始,读出校准参数
	 
	Ttype=Flash_Temp[4*4+1]; //读取标记字,屏幕方向和当前记录的参数方向是否一致
	
	tempfac = Flash_Temp[4*4+2]; //读取标记字,看是否校准过！
	
		
	if((tempfac==0X0A)&(Ttype==LCD_DIR_Mode))//触摸屏已经校准过了			   
 	{ 
				memcpy(TData.B8_Temp,Flash_Temp,4);
			  tempfac=TData.B32_temp;
        tp_dev.xfac = (float)tempfac / 100000000; //得到x校准参数
			
       	memcpy(TData.B8_Temp,&Flash_Temp[4],4);
			  tempfac=TData.B32_temp;
			  tp_dev.yfac = (float)tempfac / 100000000; //得到y校准参数
		
        //得到x偏移量
        memcpy(TData.B8_Temp,&Flash_Temp[4*2],4);
			  tempfac=TData.B32_temp;
			  tp_dev.xoff=tempfac;
		
        //得到y偏移量
        
			  memcpy(TData.B8_Temp,&Flash_Temp[4*3],4);
			  tempfac=TData.B32_temp;
			  tp_dev.yoff=tempfac;
		 
		    if(Ttype==0||Ttype==1)tp_dev.touchtype=0;//Flash_Temp[4*4+1];  //读取触屏类型--根据类型，设置触摸方向
		    
				else tp_dev.touchtype=1;
		
				if(tp_dev.touchtype)//X,Y方向与屏幕相反
				{
					CMD_RDX=0X90;
					CMD_RDY=0XD0;	 
				}
				else				   //X,Y方向与屏幕相同
				{
					CMD_RDX=0XD0;
					CMD_RDY=0X90;	 
				}		 
				return 1;	 
			}
			return 0;//未校准，或者参数不正确，则返回0
}



//提示字符串
uint8_t* const TP_MSG="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 


//提示校准结果(各个参数)

void TP_Adj_Info_Show(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3,uint16_t fac)
{	  
	POINT_COLOR=RED;
	LCD_ShowString(40,160,lcddev.width,lcddev.height,16,RED,(uint8_t*)"x1:");
 	LCD_ShowString(40+80,160,lcddev.width,lcddev.height,16,RED,(uint8_t*)"y1:");
 	LCD_ShowString(40,180,lcddev.width,lcddev.height,16,RED,(uint8_t*)"x2:");
 	LCD_ShowString(40+80,180,lcddev.width,lcddev.height,16,RED,(uint8_t*)"y2:");
	LCD_ShowString(40,200,lcddev.width,lcddev.height,16,RED,(uint8_t*)"x3:");
 	LCD_ShowString(40+80,200,lcddev.width,lcddev.height,16,RED,(uint8_t*)"y3:");
	LCD_ShowString(40,220,lcddev.width,lcddev.height,16,RED,(uint8_t*)"x4:");
 	LCD_ShowString(40+80,220,lcddev.width,lcddev.height,16,RED,(uint8_t*)"y4:");  
 	LCD_ShowString(40,240,lcddev.width,lcddev.height,16,RED,(uint8_t*)"fac is:");     
	LCD_ShowNum(40+24,160,x0,4,16,RED);		//显示数值
	LCD_ShowNum(40+24+80,160,y0,4,16,RED);	//显示数值
	LCD_ShowNum(40+24,180,x1,4,16,RED);		//显示数值
	LCD_ShowNum(40+24+80,180,y1,4,16,RED);	//显示数值
	LCD_ShowNum(40+24,200,x2,4,16,RED);		//显示数值
	LCD_ShowNum(40+24+80,200,y2,4,16,RED);	//显示数值
	LCD_ShowNum(40+24,220,x3,4,16,RED);		//显示数值
	LCD_ShowNum(40+24+80,220,y3,4,16,RED);	//显示数值
 	LCD_ShowNum(40+56,240,fac,3,16,RED); 	//显示数值,该数值必须在95~105范围之内.

}
		 
//触摸屏校准代码
//得到四个校准参数
void TP_Adjust(void)
{								 
	uint16_t pos_temp[4][2];//坐标缓存值
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	double fac; 	
	uint16_t outtime=0;
 	cnt=0;			
	
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	
//	LCD_Clear(WHITE);//清屏   
//	POINT_COLOR=RED;//红色 
	LCD_Clear(WHITE);//清屏 	   
	POINT_COLOR=BLACK;
	
	LCD_ShowString(40,40,160,100,16,BLACK,(uint8_t*)TP_MSG);//显示提示信息
	
//	GBK_Show_Str(8,200,240,16,"校准：依次点四角的点",16,RED,WHITE,0);	//用字库方式，显示提示
//	GBK_Show_Str(8,220,240,16,"      进行触摸屏校准",16,RED,WHITE,0);	//用字库方式，显示提示
	
	TP_Drow_Touch_Point(20,20,RED);//画点1 
	
	tp_dev.sta=0;//消除触发信号 
	tp_dev.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	
	while(1)//如果连续10秒钟没有按下,则自动退出
	{
		tp_dev.scan(1);//扫描物理坐标
		
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//按键按下了一次(此时按键松开了.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//标记按键已经被处理过了.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			
			cnt++;	  
			
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,WHITE);				//清除点1 
					TP_Drow_Touch_Point(lcddev.width-20,20,RED);	//画点2
					break;
				case 2:
 					TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);	//清除点2
					TP_Drow_Touch_Point(20,lcddev.height-20,RED);	//画点3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);			//清除点3
 					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//画点4
					break;
				case 4:	 //全部四个点已经得到
	    		       //对边相等
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,2的距离
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到3,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
 						continue;
					}
					
					
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 				TP_Drow_Touch_Point(20,20,RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
					
					//计算结果
					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					tp_dev.xoff=(lcddev.width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					tp_dev.yoff=(lcddev.height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					
					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)//触屏和预设的相反了.
					{
						cnt=0;
						
 				    TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
						
   	 				TP_Drow_Touch_Point(20,20,RED);								//画点1
						
						LCD_ShowString(40,26,lcddev.width,lcddev.height,16,RED,"TP Need readjust!");
						
						tp_dev.touchtype=!tp_dev.touchtype;//修改触屏类型.
						
						if(tp_dev.touchtype)//X,Y方向与屏幕相反
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}else				   //X,Y方向与屏幕相同
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}	
					
					POINT_COLOR=BLUE;
					
					LCD_Clear(WHITE);//清屏
					
					LCD_ShowString(32,110,lcddev.width,lcddev.height,16,RED,"Touch Screen Adjust OK!");//校正完成
					
//					GBK_Show_Str(32,130,240,16,"触摸屏校准完成，OK！！",16,RED,WHITE,0);	//显示内码低字节
					
					HAL_Delay(1000);  //延时显示提示
					
					TP_Save_Adjdata(); //保存校准参数
					
 					LCD_Clear(WHITE);//清屏   
					
					return;//校正完成				 
			}
		}
		
		
		HAL_Delay(10);
		outtime++;
		
		if(outtime>1000)
		{
			
			TP_Get_Adjdata();//获取校准参数
			
			break;
	 	} 
 	}
}	




/********************************************************************************/

//2.4寸/2.8寸/3.2寸 TFT液晶驱动	  
//支持驱动IC型号包括:ILI9341   
//触摸屏驱动(支持ADS7843/XPT2046/TSC2046)代码	   

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/********************************************************************************/

//触摸屏初始化  		    
//返回值:0,没有进行校准
//       1,进行过校准
uint8_t TP_Init(void)
{	

//	  GPIO_InitTypeDef  GPIO_InitStructure;	
//	
//	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOB,C,F时钟

//    //PB1--TPEN,  PB2--T-MISO  初始化设置
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;//PB1/PB2 设置为上拉输入
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
//		
//	  //PB0--TSCK,  初始化设置
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PB0设置为推挽输出
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
//	  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
//		
//	  //PC13--T-CS
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//PC13设置为推挽输出
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
//	  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化	
//		
//		//PF11--T-MOSI
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PF11设置推挽输出
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
//	  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化	
	 			
 
		TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//第一次读取初始化	 
		TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//第一次读取初始化	 
		
		
		if(TP_Get_Adjdata())return 0;//已经校准
		else			  		//未校准?
		{ 										    
			LCD_Clear(WHITE);	//清屏
			TP_Adjust();  		//屏幕校准  
		}	
		
		TP_Get_Adjdata();	

		return 1; 									 
}






