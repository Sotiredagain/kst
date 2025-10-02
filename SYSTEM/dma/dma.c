#include "stm32f10x.h"  
#include "delay.h"

void MyDMA_Init(uint32_t AddrA, uint32_t AddrB,uint16_t Size)
{
    
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef  DMA_InitStructure;
	
	DMA_DeInit(DMA1_Channel3);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr=AddrA;//外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr=AddrB;//内存地址
	DMA_InitStructure.DMA_DIR =DMA_DIR_PeripheralDST;//内存到外设模式
	DMA_InitStructure.DMA_BufferSize =Size;//传输大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据格式
	DMA_InitStructure.DMA_MemoryDataSize =DMA_MemoryDataSize_Byte ;//内存数据格式
	DMA_InitStructure.DMA_Mode =DMA_Mode_Normal ;//dma传输模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//dma优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//因为要用dma的spi通道触发，所以失能
	
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

