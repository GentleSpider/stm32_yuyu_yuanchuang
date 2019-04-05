/*******************************************************************************
  测试平台：STM32F103ZET6最小系统
*******************************************************************************/
static void i2cDelay()
{
    volatile int i = 7;
    while (i)
    i--;
}

// SCL高电平期间，SDA出现下降沿为起始信号
static bool i2cStart()
{
    SDA_OUT;
    SCL_H;
    SDA_H;
    i2cDelay();
    if (!sdaRead)  // 如果SDA为低电平，则总线忙，退出
        return false;
    SDA_L;
    if (sdaRead)  // 如果SDA为高电平，则总线忙，退出
        return false;
    SDA_L;
    return true;
}

// SCL高电平期间，SDA出现上升沿为停止信号
static void i2cStop(void)
{
    SDA_OUT;
    SCL_L; 
    SDA_L;
    i2cDelay();  // STOP:when CLK is high DATA from low to high 
    SCL_H;
    SDA_H;  
    i2cDelay();
}

static void i2cAck(void)
{
    SDA_OUT;
    SCL_L;
    i2cDelay();
    SDA_L;
    i2cDelay();
    SCL_H;
    i2cDelay();
    SCL_L;
}

static void i2cNoAck(void)
{
    SDA_OUT;
    SCL_L;
    i2cDelay();
    SDA_H;
    i2cDelay();
    SCL_H;
    i2cDelay();
    SCL_L;
}

// SCL高电平期间，SDA电平被从设备拉低表示应答
static bool i2cWaitAck(void)
{
    uint8_t errTimes = 0;

    SDA_IN;
    SDA_H;
    i2cDelay();
    SCL_H;
    i2cDelay();
    while (sdaRead) {
        if (errTimes++ > 20) {
            SCL_L;
            return false;
        }           
        i2cDelay();
    }
    SCL_L;
    return true;
}

// 发送数据，数据从高位到低位传输  
static void i2cSendByte(uint8_t byte)  
{
    uint8_t i = 8;

    SDA_OUT;
    while (i--) {      
        SCL_L;  // 时钟信号为低电平期间，允许数据线电平变化
        i2cDelay();
        if (byte & 0x80)
            SDA_H;
        else
            SDA_L; 
        byte <<= 1; 
        i2cDelay();
        SCL_H;
        i2cDelay();
    }
    SCL_L;
}

static uint8_t i2cReceiveByte()  
{
    uint8_t i = 8;
    uint8_t byte = 0;

    SDA_IN;
    SDA_H;
    while (i--) {
        byte <<= 1;
        SCL_H;
        i2cDelay();
        if (sdaRead) {
            byte |= 0x01;
        }
        SCL_L;
        i2cDelay();
    }
    SCL_L;
    return byte; 
}


void i2cInit()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIOB clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* Configure GPIOB.6 & GPIOB.7 as open-drain output */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
 * 通过I2C总线写一字节数据
 * @param[in] dev:设备I2C地址
 * @param[in] reg:寄存器地址
 * @param[in] data:要写入的数据
 */
bool i2cWriteOneByte(uint8_t dev, uint8_t reg, uint8_t data)
{
    if (!i2cStart())        
        return false;
    i2cSendByte(dev << 1);  // 从机地址由高7位+读写位构成   
    if (!i2cWaitAck()) {     
        i2cStop();
        return false;
    }
    i2cSendByte(reg);       
    i2cWaitAck();
    i2cSendByte(data);     
    i2cWaitAck();
    return true;
}

/**
 *  
 * @param[in] dev:设备I2C地址
 * @param[in] reg:寄存器地址
 * @param[in] len:字节数 
 * @param[in] data:待写入的数据 
 */
bool i2cWriteBytes(uint8_t dev, uint8_t reg, uint8_t len, uint8_t *data)
{
    uint8_t i;

    if (!i2cStart())        
        return false;
    i2cSendByte(dev << 1);          
    if (!i2cWaitAck()) {     
        i2cStop();
        return false;
    }
    i2cSendByte(dev);   
    i2cWaitAck();
    for (i = 0; i < len; i++) {
        i2cSendByte(data[i]);
        if (!i2cWaitAck()) {
            i2cStop();
            return false;
        }
    }
    i2cStop();
    return true;
}


/**
 * 从I2C设备中读取数据
 * @param[in] dev:设备I2C地址
 * @param[in] reg:寄存器地址
 * @param[in] len:数据字节数
 * @param[out] data:读出的数据
 */
bool i2cReadBytes(uint8_t dev, uint8_t reg, uint8_t len, uint8_t *data)
{
    if (!i2cStart())        
        return false;
    i2cSendByte(dev << 1);      
    if (!i2cWaitAck()) {     
        i2cStop();
        return false;
    }
    i2cSendByte(reg);     
    i2cWaitAck();
    i2cStart();           
    i2cSendByte((dev << 1) | 0x01);  // 器件地址+读命令    
    i2cWaitAck();
    while (len) {
        *data = i2cReceiveByte();
        if (len == 1)
            i2cNoAck();  // 最后一个字节不应答
        else
            i2cAck();
        data++;
        len--;
    }
    i2cStop();
    return true;
}