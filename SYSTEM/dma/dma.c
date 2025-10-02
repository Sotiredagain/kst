#include "stm32f10x.h"  
#include "delay.h"

void MyDMA_Init(uint32_t AddrA, uint32_t AddrB,uint16_t Size)
{
    
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef  DMA_InitStructure;
	
	DMA_DeInit(DMA1_Channel3);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr=AddrA;//�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr=AddrB;//�ڴ��ַ
	DMA_InitStructure.DMA_DIR =DMA_DIR_PeripheralDST;//�ڴ浽����ģʽ
	DMA_InitStructure.DMA_BufferSize =Size;//�����С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݸ�ʽ
	DMA_InitStructure.DMA_MemoryDataSize =DMA_MemoryDataSize_Byte ;//�ڴ����ݸ�ʽ
	DMA_InitStructure.DMA_Mode =DMA_Mode_Normal ;//dma����ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//dma���ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//��ΪҪ��dma��spiͨ������������ʧ��
	
	DMA_Init(DMA1_Channel3,&DMA_InitStructure);
	
	
	
//			SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
	
		DMA_Cmd(DMA1_Channel3,ENABLE);
	

}
void DMA_Enable(uint16_t Size)
{
	DMA_Cmd(DMA1_Channel3,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel3,Size);
	DMA_Cmd(DMA1_Channel3,ENABLE);
}

