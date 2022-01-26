/*
 * @Author: chilkings
 * @Date: 2022-01-25 11:28:44
 * @LastEditTime: 2022-01-26 22:41:56
 * @Description: 模拟IIC头文件
 * @mail:chilkings.mail@gmail.com
 */
#include"stm32f4xx_hal.h"


#define delayCount 5

typedef enum 
{
    SFIIC_OK       = 0x00U,
    SFIIC_ERROR    = 0x01U,
    SFIIC_BUSY     = 0x02U,
    SFIIC_TIMEOUT  = 0x03U
} SFIIC_StatusTypeDef;

typedef struct classSoftI2c SFIIC_HandleTypeDef;
struct classSoftI2c
{
		GPIO_TypeDef *SDAPort;
    uint32_t SDAPin;
    GPIO_TypeDef *SCLPort;
    uint32_t SCLPin;
	
	  GPIO_InitTypeDef SDAPinInit;  
		GPIO_InitTypeDef SCLPinInit;  
	
	  void (*Init)(SFIIC_HandleTypeDef *hsfi2c); //构造函数

    SFIIC_StatusTypeDef (*Master_Receive)(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    SFIIC_StatusTypeDef (*Master_Transmit)(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    SFIIC_StatusTypeDef (*Mem_Read)(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    SFIIC_StatusTypeDef (*Mem_Write)(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
};



void softI2cInit(SFIIC_HandleTypeDef *hsfi2c);
SFIIC_StatusTypeDef SFIIC_Master_Receive(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
SFIIC_StatusTypeDef SFIIC_Master_Transmit(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
SFIIC_StatusTypeDef SFIIC_Mem_Read(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
SFIIC_StatusTypeDef SFIIC_Mem_Write(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
void testOutput(SFIIC_HandleTypeDef *hsfi2c);


