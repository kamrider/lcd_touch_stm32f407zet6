#include "w25qxx.h" 

/**
  * @brief  ?? SPI ????????????? (HAL???)
  * @param  TxData: ??????
  * @retval uint8_t: ??????
  */
uint8_t W25QXX_SPI_ReadWriteByte(uint8_t TxData)
{
    uint8_t RxData; // ??????????
    HAL_StatusTypeDef status;

    // ?? HAL ?????????
    // - &hspi1:        ???? SPI1 ??
    // - &TxData:       ???????? (????? TxData)
    // - &RxData:       ?????????? (????? RxData ??)
    // - 1:             ???,???????/?? 1 ???
    // - HAL_MAX_DELAY: ????,?????????? (??????????)
    //                  ??????????????,? 100
    status = HAL_SPI_TransmitReceive(&hspi1, &TxData, &RxData, 1, HAL_MAX_DELAY);

    // (?????) ?? HAL ??????
    if (status != HAL_OK)
    {
        // ??????,?????????
        // ??:????????????????????????? (? 0xFF)
        // ?????? HAL ??????? (??? main.c ???????)
        Error_Handler();
    }

    return RxData; // ????????
}
 
uint16_t W25QXX_TYPE=W25Q16;	//Ĭ����W25Q16

//4KbytesΪһ��Sector
//16������Ϊ1��Block
													 
//��ʼ��SPI FLASH��IO��
void W25QXX_Init(void)
{ 
//  GPIO_InitTypeDef  GPIO_InitStructure;
// 
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOGʱ��

//	  //PB14--CS
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//PB14
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
//  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��


//	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET); =1;			//SPI FLASH��ѡ��
//	SPI1_Init();		   			//��ʼ��SPI
//	SPI1_SetSpeed(SPI_BaudRatePrescaler_2);		//����Ϊ42Mʱ��,����ģʽ 
	W25QXX_TYPE=W25QXX_ReadID();	//��ȡFLASH ID.
	
	
}  


/************************************************************************/

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/


uint8_t W25QXX_ReadSR(void)   
{  
	uint8_t byte=0;   
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET); //ʹ������                            
	W25QXX_SPI_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������
	byte=W25QXX_SPI_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
 	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);  //ȡ��Ƭѡ     
	return byte;   
} 


/************************************************************************/

//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/


void W25QXX_Write_SR(uint8_t sr)   
{   
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);                            //ʹ������   
	W25QXX_SPI_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	W25QXX_SPI_ReadWriteByte(sr);               //д��һ���ֽ�  
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);                            //ȡ��Ƭѡ     	      
}  


/************************************************************************/

//W25QXXдʹ��	
//��WEL��λ   
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/


void W25QXX_Write_Enable(void)   
{
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);                          //ʹ������   
    W25QXX_SPI_ReadWriteByte(W25X_WriteEnable);      //����дʹ��  
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);                             //ȡ��Ƭѡ     	      
}

/************************************************************************/


//W25QXXд��ֹ	
//��WEL����  
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/

void W25QXX_Write_Disable(void)   
{  
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);                            //ʹ������   
    W25QXX_SPI_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);                             //ȡ��Ƭѡ     	      
} 	


/************************************************************************/

//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/

  
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;	  
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET); 				    
	W25QXX_SPI_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	W25QXX_SPI_ReadWriteByte(0x00); 	    
	W25QXX_SPI_ReadWriteByte(0x00); 	    
	W25QXX_SPI_ReadWriteByte(0x00); 	 			   
	Temp|=W25QXX_SPI_ReadWriteByte(0xFF)<<8;  
	Temp|=W25QXX_SPI_ReadWriteByte(0xFF);	 
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET); 				    
	return Temp;
} 



/************************************************************************/

//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)


//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/


void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   										    
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);                             //ʹ������   
    W25QXX_SPI_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
    W25QXX_SPI_ReadWriteByte((uint8_t)((ReadAddr)>>16));  //����24bit��ַ    
    W25QXX_SPI_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    W25QXX_SPI_ReadWriteByte((uint8_t)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=W25QXX_SPI_ReadWriteByte(0XFF);   //ѭ������  
    }
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);  				    	      
}  



/************************************************************************/

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/

 
void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    W25QXX_Write_Enable();                  //SET WEL 
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);                             //ʹ������   
    W25QXX_SPI_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
    W25QXX_SPI_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //����24bit��ַ    
    W25QXX_SPI_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    W25QXX_SPI_ReadWriteByte((uint8_t)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)W25QXX_SPI_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);                             //ȡ��Ƭѡ 
	W25QXX_Wait_Busy();					   //�ȴ�д�����
} 




/************************************************************************/

//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/


void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 


/************************************************************************/

//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/


uint8_t W25QXX_BUFFER[4096];		 
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 
}



/************************************************************************/

//��������оƬ		  
//�ȴ�ʱ�䳬��...
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/


void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                  //SET WEL 
    W25QXX_Wait_Busy();   
  	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);                             //ʹ������   
    W25QXX_SPI_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);                             //ȡ��Ƭѡ     	      
	W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
}


/************************************************************************/

//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/


void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
//	//����falsh�������,������   
// 	printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
    W25QXX_Write_Enable();                  //SET WEL 	 
    W25QXX_Wait_Busy();   
  	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);                            //ʹ������   
    W25QXX_SPI_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
    W25QXX_SPI_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //����24bit��ַ    
    W25QXX_SPI_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    W25QXX_SPI_ReadWriteByte((uint8_t)Dst_Addr);  
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);                            //ȡ��Ƭѡ     	      
    W25QXX_Wait_Busy();   				   //�ȴ��������
} 



/************************************************************************/

//�ȴ�����
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/


void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
}


/************************************************************************/

//�������ģʽ
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/


void W25QXX_PowerDown(void)   
{ 
  	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);                             //ʹ������   
    W25QXX_SPI_ReadWriteByte(W25X_PowerDown);        //���͵�������  
	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);                             //ȡ��Ƭѡ     	      
	HAL_Delay(1);

}  


/************************************************************************/


//����
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/

void W25QXX_WAKEUP(void)   
{  
  	HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_RESET);                             //ʹ������   
    W25QXX_SPI_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	  HAL_GPIO_WritePin(W25QXX_CS_GPIO_Port, W25QXX_CS_Pin, GPIO_PIN_SET);                             //ȡ��Ƭѡ     	      
  	HAL_Delay(1);                             //�ȴ�TRES1
}   
















