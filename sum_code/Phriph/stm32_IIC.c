define BufferSize 4

define I2C2_SLAVE_ADDRESS7 0x30

/* Private macro ————————————————————-*/ 
/* Private variables ———————————————————*/

vu8 I2C1_Buffer_Tx[BufferSize] = {1, 2, 3, 4}; /* I2C1待发送字节数组 */ 
vu8 I2C2_Buffer_Rx[BufferSize] = {0, 0, 0, 0}; /* I2C2待接收字节缓冲 */ 
vu8 Tx_Idx = 0; /* I2C1数据发送计数变量 */ 
vu8 Rx_Idx = 0; /* I2C2数据接收计数变量 */

/* 自定义参数宏 ——————————————————-*/

define I2C1_SLAVE_ADDRESS7 0x30 /* 定义 I2C1 本地地址为0x30 */

define I2C2_SLAVE_ADDRESS7 0x30 /* 定义 I2C2 本地地址为0x30 */

define BufferSize 4

/* 自定义函数宏 ——————————————————-*/

/* 自定义变量 ——————————————————-*/

/* 用户函数声明 ———————————————————*/

void RCC_Configuration(void); 
void GPIO_Configuration(void); 
void NVIC_Configuration(void); 
void USART_Configuration(void); 
void I2c_Configuration(void);

int main(void) 
{ 
/* 设置系统时钟 */ 
RCC_Configuration(); 
/* 设置 NVIC */ 
NVIC_Configuration(); 
/* 设置GPIO端口 */ 
GPIO_Configuration(); 
/* 设置USART */ 
USART_Configuration(); 
/* 设置 IIC */ 
I2c_Configuration();

/* I2C1产生开始条件 */
I2C_GenerateSTART(I2C1, ENABLE);

while(1);

}

void RCC_Configuration(void) 
{ 
ErrorStatus HSEStartUpStatus;

RCC_DeInit();
RCC_HSEConfig(RCC_HSE_ON);
HSEStartUpStatus = RCC_WaitForHSEStartUp();
if(HSEStartUpStatus == SUCCESS)
{
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);    
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08);
}

/* 开启 I2C1、I2C2 设备时钟 */
RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 | RCC_APB1Periph_I2C2, ENABLE);
/* 开启 GPIOA、GPIOB和 USART 设备时钟 */
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

}

void GPIO_Configuration(void) 
{ 
/* 定义 GPIO 初始化结构体 GPIO_InitStructure */ 
GPIO_InitTypeDef GPIO_InitStructure;

/* 配置 I2C1 设备的引脚为复用开漏模式 */
GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
GPIO_Init(GPIOB, &GPIO_InitStructure);

/* 配置 I2C2 设备的引脚为复用开漏模式 */
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
GPIO_Init(GPIOB, &GPIO_InitStructure);

/* 配置 USART 设备引脚 */ 
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStructure);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void I2c_Configuration(void) 
{ 
/* 定义 I2C 初始化结构体 I2C_InitStructure */ 
I2C_InitTypeDef I2C_InitStructure;

/*  
*   I2C模式;
*   占空比50%;
*   本地地址（前面宏定义定义为0x30）
*   使能应答;
*   应答7位地址;
*   速率200K;
*/
I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//设置I2C接口为I2C模式
I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;  //设置占空比为2/1
I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;     //设置第一个设备自身地址可以是7位也可以是10位
I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//使能ACk应答
I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //设置应答7位地址
I2C_InitStructure.I2C_ClockSpeed = 200000;  //
I2C_Init(I2C1, &I2C_InitStructure);

I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
I2C_InitStructure.I2C_OwnAddress1 = I2C2_SLAVE_ADDRESS7;
I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
I2C_InitStructure.I2C_ClockSpeed = 200000;
I2C_Init(I2C2, &I2C_InitStructure);

/* 开启I2C1、I2C2 的事件、缓存中断 */
I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF, ENABLE);

/* 使能I2C1、I2C2 接口 */
I2C_Cmd(I2C1, ENABLE);
I2C_Cmd(I2C2, ENABLE);

}

void NVIC_Configuration(void) 
{ 
/* 定义 NVIC 初始化结构体 NVIC_InitStructure */ 
NVIC_InitTypeDef NVIC_InitStructure;

/* 选择 NVIC 优先级分组1 */
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

/* 设置并使能 I2C1 中断 */
NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQChannel;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);

/* 设置并使能 I2C2 中断 */  
NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQChannel;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);

}

void USART_Configuration(void) 
{ 
/* 定义USART初始化结构体 USART_InitStructure */ 
USART_InitTypeDef USART_InitStructure; 
/* 定义USART初始化结构体 USART_ClockInitStructure */ 
USART_ClockInitTypeDef USART_ClockInitStructure;

/*  
*   波特率为115200bps;
*   8位数据长度;
*   1个停止位，无校验;
*   禁用硬件流控制;
*   禁止USART时钟;
*   时钟极性低;
*   在第2个边沿捕获数据
*   最后一位数据的时钟脉冲不从 SCLK 输出； 
*/
USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
USART_ClockInit(USART1 , &USART_ClockInitStructure);

USART_InitStructure.USART_BaudRate = 115200;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_InitStructure.USART_StopBits = USART_StopBits_1;
USART_InitStructure.USART_Parity = USART_Parity_No ;
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
USART_Init(USART1 , &USART_InitStructure);

/* 使能USART1 */
USART_Cmd(USART1 , ENABLE);

}

void I2C1_EV_IRQHandler(void) 
{ 
switch (I2C_GetLastEvent(I2C1)) 
{ 
case I2C_EVENT_MASTER_MODE_SELECT: /* 已发送起始条件 */ 
{ 
/* 发送从机地址 */ 
I2C_Send7bitAddress(I2C1, I2C2_SLAVE_ADDRESS7, I2C_Direction_Transmitter);

        break;
    }

case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:        /* 从机地址已发送 */
    {
        /* 发送第一个数据 */
        printf("\r\n The I2C1 has send data 0x0%x\r\n", I2C1_Buffer_Tx[Rx_Idx]);
        I2C_SendData(I2C1, I2C1_Buffer_Tx[Tx_Idx++]);  

        break;
    }


case I2C_EVENT_MASTER_BYTE_TRANSMITTED:                 /* 第一个数据已发送 */
    {
        if(Tx_Idx < BufferSize)
        {
            /* 继续发送剩余数据... ... */
            printf("\r\n The I2C1 has send data 0x0%x\r\n", I2C1_Buffer_Tx[Rx_Idx]);
            I2C_SendData(I2C1, I2C1_Buffer_Tx[Tx_Idx++]);
        }
        else
        {
            /* 剩余数据发送完毕，发送结束条件 */
            I2C_GenerateSTOP(I2C1, ENABLE);
            /* 禁止 I2C1 中断*/
            I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
        }

        break;
    }


default:
    {
        break;
    }
}

}

/********************************************************************* 
* Function Name : I2C1_ER_IRQHandler 
* Description : This function handles I2C1 Error interrupt request. 
* Input : None 
* Output : None 
* Return : None 
*********************************************************************/ 
void I2C1_ER_IRQHandler(void) 
{ 
}

/********************************************************************* 
* Function Name : I2C2_EV_IRQHandler 
* Description : This function handles I2C2 Event interrupt request. 
* Input : None 
* Output : None 
* Return : None 
*********************************************************************/ 
void I2C2_EV_IRQHandler(void) 
{ 
switch (I2C_GetLastEvent(I2C2)) 
{ 
case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED: /* 收到匹配的地址数据 */ 
{ 
break; 
}

case I2C_EVENT_SLAVE_BYTE_RECEIVED:             /* 收到数据 */
    {
        if (Rx_Idx < BufferSize)
        {
            I2C2_Buffer_Rx[Rx_Idx] = I2C_ReceiveData(I2C2);
            printf("\r\n The I2C2 has received data 0x%x\r\n", I2C2_Buffer_Rx[Rx_Idx++]);
        }

        break;
    }


case I2C_EVENT_SLAVE_STOP_DETECTED:             /* 收到结束条件 */
    {       
        I2C_ClearFlag(I2C2, I2C_FLAG_STOPF);
        I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF, DISABLE);

        break;
    }

default:
    {
        break;
    }
}

}

int fputc(int ch, FILE *f) 
{ 
USART_SendData(USART1, (u8) ch); 
while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
return ch; 
}