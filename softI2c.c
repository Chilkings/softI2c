/*
 * @Author: chilkings
 * @Date: 2022-01-25 11:28:31
 * @LastEditTime: 2022-01-26 22:38:55
 * @Description: 模拟IIC源文件
 * @mail:chilkings.mail@gmail.com
 */
#include "softI2c.h"
#include "delay.h"

/*定义用于初始化IO口的结构体*/
//GPIO_InitTypeDef SDAPinInit = {0};  //SDA OUT
//GPIO_InitTypeDef SCLPinInit = {0};  //SCL OUT
//GPIO_InitTypeDef SDAPinInput = {0}; //SDA IN

/**
 * @description: 模拟IIC IO口初始化
 * @param {SFIIC_HandleTypeDef*} *hsfi2c 储存IIC IO口结构体的指针
 * @return None
 * @note 建议在cubemx中直接配置好IO口为推挽输出，拉高
 */
void softI2cInit(SFIIC_HandleTypeDef *hsfi2c)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;

		hsfi2c->Master_Receive = SFIIC_Master_Receive;
	  hsfi2c->Master_Transmit = SFIIC_Master_Transmit;
	  hsfi2c->Mem_Read = SFIIC_Mem_Read;
	  hsfi2c->Mem_Write = SFIIC_Mem_Write;
	
    if (SDA == GPIOA || SCL == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (SDA == GPIOB || SCL == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (SDA == GPIOC || SCL == GPIOC)
        __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (SDA == GPIOD || SCL == GPIOD)
        __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (SDA == GPIOE || SCL == GPIOE)
        __HAL_RCC_GPIOE_CLK_ENABLE();

//    HAL_GPIO_WritePin(SDA, SDAP, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(SCL, SCLP, GPIO_PIN_RESET);

    hsfi2c->SDAPinInit.Pin = SDAP;
    hsfi2c->SDAPinInit.Mode = GPIO_MODE_OUTPUT_PP;
    hsfi2c->SDAPinInit.Pull = GPIO_NOPULL;
    hsfi2c->SDAPinInit.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SDA, &(hsfi2c->SDAPinInit));
    HAL_GPIO_WritePin(SDA, SDAP, GPIO_PIN_SET);


    hsfi2c->SCLPinInit.Pin = SCLP;
    hsfi2c->SCLPinInit.Mode = GPIO_MODE_OUTPUT_PP;
    hsfi2c->SCLPinInit.Pull = GPIO_NOPULL;
    hsfi2c->SCLPinInit.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SCL, &(hsfi2c->SCLPinInit));
    HAL_GPIO_WritePin(SCL, SCLP, GPIO_PIN_SET);
		
}

/**
 * @description: 微妙级延时函数，可以自定义
 */
void softI2cDelay()
{
//    for (int i = 0; i < delayCount; i++)
//        ;
		delay_us(1);
}

/**
 * @description: 测试IO口工作是否正常
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @return None
 */
void testOutput(SFIIC_HandleTypeDef *hsfi2c)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;

    SCL->BSRR = SCLP;
    SDA->BSRR = SDAP;

    HAL_Delay(1000);

    SDA->BSRR = (uint32_t)SDAP << 16U;
    SCL->BSRR = (uint32_t)SCLP << 16U;

    HAL_Delay(1000);
}

/**
 * @description: 修改SDA IO口输入输出方向
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @param {int} selection selection 0输入 1输出
 * @return None
 */
void changeSDAState(SFIIC_HandleTypeDef *hsfi2c, int selection)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;

    if (selection == 0)
    {
        hsfi2c->SDAPinInit.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(SDA, &(hsfi2c->SDAPinInit));
    }
    else if (selection == 1)
    {
        SCL->BSRR = (uint32_t)SCLP << 16U ;
        hsfi2c->SDAPinInit.Mode = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(SDA, &(hsfi2c->SDAPinInit));
        SDA->BSRR = (uint32_t)SDAP << 16U ;
        SCL->BSRR = (uint32_t)SCLP << 16U ;
    }

    for(int i=0;i<5;i++)
    {
        softI2cDelay();
    }
}

/**
 * @description: 复位IIC 
 * @param None
 * @return None
 */
void resetIO(SFIIC_HandleTypeDef *hsfi2c)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;

    changeSDAState(hsfi2c, 1); //sda输出
    SCL->BSRR = SCLP;          //scl拉高
    SDA->BSRR = SDAP;          //sda拉高
}

/**
 * @description: 产生启动信号
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @return {*}
 */
void startCondition(SFIIC_HandleTypeDef *hsfi2c)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;
	
	SDA->BSRR = (uint32_t)SDAP << 16U ;
	softI2cDelay();
	SCL->BSRR = (uint32_t)SCLP << 16U ;
}

/**
 * @description: 产生停止信号
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @return {*}
 */
void stopCondition(SFIIC_HandleTypeDef *hsfi2c)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;

	
    changeSDAState(hsfi2c, 1); 
	SCL->BSRR = SCLP;          //拉高scl
	softI2cDelay();
    SDA->BSRR = SDAP;         //拉高sda
    softI2cDelay();
}

/**
 * @description: 发送ack信号
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @param {int} select 1发送ack 0发送nack
 * @return {*}
 */
void sendACK(SFIIC_HandleTypeDef *hsfi2c, int select)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;

    changeSDAState(hsfi2c, 1);

    if (select == 1)
        SDA->BSRR = (uint32_t)SDAP << 16U;
    else
        SDA->BSRR = SDAP;

    softI2cDelay();
    SCL->BSRR = SCLP;
    softI2cDelay();
    SCL->BSRR = (uint32_t)SCLP << 16U;
}

/**
 * @description: 重复启动信号
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @return {*}
 */
void repeatedStartCondition(SFIIC_HandleTypeDef *hsfi2c)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;

    changeSDAState(hsfi2c, 1);
    SDA->BSRR = SDAP;
    softI2cDelay();
    SCL->BSRR = SCLP;
    softI2cDelay();
    SDA->BSRR = (uint32_t)SDAP << 16U;
}

/**
 * @description: 等待ACK信号
 * @param {uint32_t} timeOut 超时时间
 * @return {*}
 */
SFIIC_StatusTypeDef waitACK(SFIIC_HandleTypeDef *hsfi2c, uint32_t timeOut)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;

    changeSDAState(hsfi2c, 0);
    int ACK = 0;

    SCL->BSRR = SCLP;
    softI2cDelay();
    if (!(HAL_GPIO_ReadPin(SDA, SDAP)))
    {
        ACK = 1;
        changeSDAState(hsfi2c, 1);
    }

    SCL->BSRR = (uint32_t)SCLP << 16U;
    changeSDAState(hsfi2c, 1);

    return ACK == 1 ? SFIIC_OK : SFIIC_ERROR;
}

/**
 * @description: 写入器件地址
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @param {uint8_t} address
 * @param {bool} selection 1写操作 0读操作
 * @param {int32_t} timeOut
 * @return {*}
 */
SFIIC_StatusTypeDef writeReadBitSet(SFIIC_HandleTypeDef *hsfi2c, uint8_t address, int selection, int32_t timeOut)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;

    int successfullACK = 0;
    int writeAddress = 0;
    if (selection)
        writeAddress = address & 0xFE; //读地址
    else
        writeAddress = address | 0x01; //写地址

    startCondition(hsfi2c);

    for (int i = 7; i >= 0; i--)
    {
        if ((writeAddress >> i) & 0x01)
            SDA->BSRR = SDAP;
        else
            SDA->BSRR = (uint32_t)SDAP << 16U;

        softI2cDelay();
        SCL->BSRR = SCLP;
        softI2cDelay();
        SCL->BSRR = (uint32_t)SCLP << 16U;
    }

    if (waitACK(hsfi2c, timeOut) == SFIIC_OK)
        successfullACK = 1;
    if(selection == 1)
    	changeSDAState(hsfi2c,1);
    return successfullACK == 1 ? SFIIC_OK : SFIIC_ERROR;
}

/**
 * @description: 写入一个字节
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @param {uint8_t} value 写入值
 * @param {int32_t} timeOut
 * @return {*}
 */
SFIIC_StatusTypeDef writeBytes(SFIIC_HandleTypeDef *hsfi2c, uint8_t value, int32_t timeOut)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;

    int successfullACK = 0;

    SCL->BSRR = (uint32_t)(SCLP << 16U);

    for (int i = 7; i >= 0; i--)
    {
        if ((value >> i) & 0x01)
            SDA->BSRR = SDAP;
        else
            SDA->BSRR = (uint32_t)(SDAP << 16U);

        softI2cDelay();
        SCL->BSRR = SCLP;
        softI2cDelay();
        SCL->BSRR = (uint32_t)(SCLP << 16U);
    }

    if (waitACK(hsfi2c, timeOut) == SFIIC_OK)
        successfullACK = 1;

    //changeSDAState(1);
    return successfullACK == 1 ? SFIIC_OK : SFIIC_ERROR;
}

/**
 * @description: 读入一个字节
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @return {*} 返回读入的值
 */
uint8_t readBytes(SFIIC_HandleTypeDef *hsfi2c)
{
    GPIO_TypeDef *SDA = hsfi2c->SDAPort;
    uint32_t SDAP = hsfi2c->SDAPin;
    GPIO_TypeDef *SCL = hsfi2c->SCLPort;
    uint32_t SCLP = hsfi2c->SCLPin;

    changeSDAState(hsfi2c, 0);
    uint8_t data = 0;
    uint8_t counter = 0;
    while (1)
    {
        SCL->BSRR = SCLP;
        softI2cDelay();
        data |= HAL_GPIO_ReadPin(SDA, SDAP);
        counter++;

        SCL->BSRR = (uint32_t)SCLP << 16U;
        softI2cDelay();

        if (counter == 8)
            return data;

        data = data << 1;
    }
}

/**
 * @description: 向iic器件的寄存器写入数据
 * @param {SFIIC_HandleTypeDef} *hsfi2c 
 * @param {uint16_t} DevAddress 器件地址
 * @param {uint16_t} MemAddress 寄存器地址
 * @param {uint16_t} MemAddSize 寄存器大小 1一个字节 2两个字节 TODO:实现两个字节
 * @param {uint8_t} *pData 写入数据地址指针
 * @param {uint16_t} Size  写入数据大小
 * @param {uint32_t} Timeout 超时时间 TODO:待实现
 * @return {*}
 */
SFIIC_StatusTypeDef SFIIC_Mem_Write(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if (!(writeReadBitSet(hsfi2c, DevAddress, 1, Timeout) == SFIIC_OK)) //写入器件地址
    {
        resetIO(hsfi2c);
        return SFIIC_BUSY;
    }

    if (MemAddSize == 1) //写入寄存器地址 地址长度为8位
    {
        if (!(writeBytes(hsfi2c, MemAddress, Timeout) == SFIIC_OK)) //写入寄存器地址
        {
            resetIO(hsfi2c);
            return SFIIC_ERROR;
        }
    }
    else //写入寄存器地址 地址长度为16位    TODO:待实现
    {
        if (!(writeBytes(hsfi2c, MemAddress, Timeout) == SFIIC_OK)) //写入寄存器地址
        {
            resetIO(hsfi2c);
            return SFIIC_ERROR;
        }
    }

    for (int i = 0; i < Size; i++) //写入要发送的数据
    {
        if (!(writeBytes(hsfi2c, pData[i], Timeout) == SFIIC_OK))
        {
            resetIO(hsfi2c);
            return SFIIC_ERROR;
        }
    }

    stopCondition(hsfi2c); //发送停止信号
    return SFIIC_OK;
}

/**
 * @description: 向IIC器件寄存器读出数据
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @param {uint16_t} DevAddress 器件地址
 * @param {uint16_t} MemAddress 寄存器地址
 * @param {uint16_t} MemAddSize 寄存器大小 1一个字节 2两个字节 TODO:实现两个字节
 * @param {uint8_t} *pData 存放读出数据地址指针
 * @param {uint16_t} Size  读出数据大小
 * @param {uint32_t} Timeout 超时时间 TODO:待实现
 * @return {*}
 */
SFIIC_StatusTypeDef SFIIC_Mem_Read(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if (!(writeReadBitSet(hsfi2c, DevAddress, 1, Timeout) == SFIIC_OK))
    {
        resetIO(hsfi2c);
        return SFIIC_BUSY;
    }

    if (MemAddSize == 1) //写入寄存器地址 地址长度为8位
    {
        if (!(writeBytes(hsfi2c, MemAddress, Timeout) == SFIIC_OK)) //写入寄存器地址
        {
            resetIO(hsfi2c);
            return SFIIC_ERROR;
        }
    }
    else //写入寄存器地址 地址长度为16位    TODO:待实现
    {
        if (!(writeBytes(hsfi2c, MemAddress, Timeout) == SFIIC_OK)) //写入寄存器地址
        {
            resetIO(hsfi2c);
            return SFIIC_ERROR;
        }
    }

    repeatedStartCondition(hsfi2c);

    if (!(writeReadBitSet(hsfi2c, DevAddress, 0, Timeout) == SFIIC_OK))
    {
        resetIO(hsfi2c);
        return SFIIC_ERROR;
    }

    for (int i = 0; i < Size; i++)
    {
        pData[i] = readBytes(hsfi2c);
        if (i != Size - 1)
        {
            sendACK(hsfi2c, 1);
            changeSDAState(hsfi2c, 0);
        }

    }
	  sendACK(hsfi2c, 0);
    stopCondition(hsfi2c);
    return SFIIC_OK;
}

/**
 * @description: 向IIC器件发送数据
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @param {uint16_t} DevAddress 器件地址
 * @param {uint8_t} *pData 发送数据指针
 * @param {uint16_t} Size 发送数据大小
 * @param {uint32_t} Timeout 超时时间 Todo:待实现
 * @return {*}
 */
SFIIC_StatusTypeDef SFIIC_Master_Transmit(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if (!(writeReadBitSet(hsfi2c, DevAddress, 1, Timeout) == SFIIC_OK))
    {
        resetIO(hsfi2c);
        return SFIIC_BUSY;
    }

    for (int i = 0; i < Size; i++) //写入要发送的数据
    {
        if (!(writeBytes(hsfi2c, pData[i], Timeout) == SFIIC_OK))
        {
            resetIO(hsfi2c);
            return SFIIC_ERROR;
        }
    }

    stopCondition(hsfi2c); //发送停止信号
    return SFIIC_OK;
}

/**
 * @description: 向IIC器件读入数据
 * @param {SFIIC_HandleTypeDef} *hsfi2c
 * @param {uint16_t} DevAddress 器件地址
 * @param {uint8_t} *pData 存放读入数据指针
 * @param {uint16_t} Size 读入数据大小
 * @param {uint32_t} Timeout 超时时间 Todo:待实现
 * @return {*}
 */
SFIIC_StatusTypeDef SFIIC_Master_Receive(SFIIC_HandleTypeDef *hsfi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if (!(writeReadBitSet(hsfi2c, DevAddress, 0, Timeout) == SFIIC_OK))
    {
        resetIO(hsfi2c);
        return SFIIC_BUSY;
    }

    for (int i = 0; i < Size; i++)
    {
        pData[i] = readBytes(hsfi2c);
        if (i != Size - 1)
        {
            sendACK(hsfi2c, 1);
            changeSDAState(hsfi2c, 0);
        }
    }
		sendACK(hsfi2c, 0);
    stopCondition(hsfi2c);
    return SFIIC_OK;
}
