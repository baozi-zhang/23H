#include "AD9959.h"

// 寄存器长度数组，表示每个寄存器对应的字节数
u8 Reg_Len[25] = {1, 3, 2, 3, 4, 2, 3, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

/**
 * @brief 启动AD9959设备
 */
void AD9959_Start(void)
{
    AD9959_CS_H(); // 取消片选
    HAL_Delay(1);
    AD9959_CS_L(); // 使能片选
    HAL_Delay(1);
}

/**
 * @brief 复位AD9959设备
 */
void AD9959_Reset(void)
{
    AD9959_RST_L(); // 拉低复位引脚
    HAL_Delay(1);
    AD9959_RST_H(); // 拉高复位引脚
    HAL_Delay(1);
    AD9959_RST_L(); // 再次拉低复位引脚
}

/**
 * @brief 更新AD9959寄存器
 * @details 每次对寄存器操作后必须调用此函数将数据从缓冲区写入寄存器
 */
void AD9959_IO_UpDate(void)
{
    AD9959_UP_L(); // 拉低更新引脚
    HAL_Delay(1);
    AD9959_UP_H(); // 拉高更新引脚
    HAL_Delay(1);
    AD9959_UP_L(); // 再次拉低更新引脚
    HAL_Delay(1);
    AD9959_UP_H(); // 再次拉高更新引脚
}

/**
 * @brief 初始化AD9959设备
 */
void AD9959_Init(void)
{
    AD9959_PDC_L();             // 拉低省电模式引脚
    AD9959_Start();             // 启动设备
    AD9959_Reset();             // 复位设备
    AD9959_WRrg(CSR, 0x00);     // 配置CSR寄存器：通道选择、串行通信模式、MSB优先
    AD9959_WRrg(FR1, 0xD00000); // 配置FR1寄存器：系统时钟设置，PLL倍频设置等
    AD9959_WRrg(FR2, 0x0000);   // 配置FR2寄存器
    AD9959_WRrg(CFR, 0x000300); // 配置CFR寄存器：设置输出电流等
    AD9959_IO_UpDate();         // 更新寄存器以使配置生效
}

/**
 * @brief 写入一个字节到AD9959
 * @param byte 要写入的字节数据
 */
void AD9959_WByte(unsigned char byte)
{
    u8 bit_ctr;
    for (bit_ctr = 0; bit_ctr < 8; bit_ctr++) // 循环输出8位数据
    {
        if (byte & 0x80) // 判断当前位是否为1
            AD9959_SDIO0_H();
        else
            AD9959_SDIO0_L();
        byte = (byte << 1); // 左移一位，准备发送下一位
        AD9959_SCK_H();     // 拉高时钟引脚
        HAL_Delay(1);
        AD9959_SCK_L(); // 拉低时钟引脚
    }
}

/**
 * @brief 向寄存器写入数据
 * @param reg 寄存器地址
 * @param _data 要写入的数据
 */
void AD9959_WRrg(u8 reg, u32 _data)
{
    u8 i, nbyte;
    nbyte = Reg_Len[reg];       // 获取寄存器所需字节数
    AD9959_SDIO3_L();           // 配置为写模式
    AD9959_CS_L();              // 使能片选
    AD9959_WByte(reg);          // 写入寄存器地址
    for (i = 0; i < nbyte; i++) // 写入数据
        AD9959_WByte((_data >> ((nbyte - 1 - i) * 8)) & 0xFF);
    AD9959_CS_H();    // 取消片选
    AD9959_SDIO3_H(); // 配置为读模式
}

/**
 * @brief 设置输出频率
 * @param Fout 目标频率值
 */
void AD9959_Set_Fre(u32 Fout)
{
    u32 FTW;
    FTW = (u32)(Fout * FRE_REF); // 计算频率控制字
    AD9959_WRrg(0x04, FTW);      // 写入频率控制寄存器
    AD9959_IO_UpDate();          // 更新寄存器
}

/**
 * @brief 设置输出相位
 * @param Pout 相位值（单位：度）
 */
void AD9959_Set_Pha(float Pout)
{
    int POW;
    POW = (int)(Pout * (float)POW_REF); // 计算相位控制字
    AD9959_WRrg(0x05, POW);             // 写入相位控制寄存器
    AD9959_IO_UpDate();                 // 更新寄存器
}

/**
 * @brief 设置输出幅度
 * @param amp 幅度值（0-1023）
 */
void AD9959_Set_Amp(u16 amp)
{
    u32 dat;
    dat = 0x00001000 + amp; // 打开幅度倍增使能
    AD9959_WRrg(0x06, dat); // 写入幅度控制寄存器
    AD9959_IO_UpDate();     // 更新寄存器
}

/**
 * @brief 选择输出通道
 * @param Channel 通道编号（0-3）
 */
void AD9959_Ch(u8 Channel)
{
    switch (Channel)
    {
    case 0:
        AD9959_WRrg(0x00, 0x10); // 通道0
        break;
    case 1:
        AD9959_WRrg(0x00, 0x20); // 通道1
        break;
    case 2:
        AD9959_WRrg(0x00, 0x40); // 通道2
        break;
    case 3:
        AD9959_WRrg(0x00, 0x80); // 通道3
        break;
    case 4:
        AD9959_WRrg(0x00, 0xF0); // 全部通道
        break;
    default:
        break;
    }
}

/**
 * @brief 单通道波形输出
 * @param Channel 通道编号
 * @param Fout 输出频率
 * @param Pout 输出相位
 * @param amp 输出幅度
 */
void AD9959_Single_Output(u8 Channel, u32 Fout, float Pout, u16 amp)
{
    AD9959_Ch(Channel);   // 设置通道
    AD9959_Set_Fre(Fout); // 设置频率
    AD9959_Set_Pha(Pout); // 设置相位
    AD9959_Set_Amp(amp);  // 设置幅度
}

/**
 * @brief 设置通道的频率扫描
 * @param Channel 通道编号
 * @param FreS 扫频起始频率
 * @param FreE 扫频结束频率
 * @param FTstep 下降扫频步进时间
 * @param RTstep 上升扫频步进时间
 * @param FFstep 下降扫频步进频率
 * @param RFstep 上升扫频步进频率
 * @param DWELL 是否使能暂停（1使能，0不使能）
 */
void AD9959__Sweep_Fre(u8 Channel, u32 FreS, u32 FreE, float FTstep, float RTstep, u32 FFstep, u32 RFstep, u8 DWELL)
{
    u32 FTW0, CW_1, RDW0, FDW0;
    u16 RSRR0, FSRR0;
    FTW0 = (u32)(FreS * FRE_REF);   // 计算起始频率控制字
    CW_1 = (u32)(FreE * FRE_REF);   // 计算结束频率控制字
    RDW0 = (u32)(RFstep * FRE_REF); // 计算上升扫频步进频率控制字
    FDW0 = (u32)(FFstep * FRE_REF); // 计算下降扫频步进频率控制字
    RSRR0 = (u16)(RTstep * 0.4f);   // 计算上升步进时间
    FSRR0 = (u16)(FTstep * 0.4f);   // 计算下降步进时间
    AD9959_Ch(Channel);             // 选择通道
    AD9959_WRrg(0x06, 0x0003ff);    // 设置幅度为最大值
    if (DWELL)
        AD9959_WRrg(0x03, 0x80c314); // 频率扫描模式，自动清空累加器，DWELL使能
    else
        AD9959_WRrg(0x03, 0x804314);        // 频率扫描模式，DWELL禁用
    AD9959_WRrg(0x01, 0xD00000);            // 配置FR1寄存器
    AD9959_WRrg(0x04, FTW0);                // 设置起始频率
    AD9959_WRrg(0x0A, CW_1);                // 设置结束频率
    AD9959_WRrg(0x08, RDW0);                // 设置上升步进频率
    AD9959_WRrg(0x09, FDW0);                // 设置下降步进频率
    AD9959_WRrg(0x07, FSRR0 * 256 + RSRR0); // 配置步进时间
    AD9959_IO_UpDate();                     // 更新寄存器
}

/**
 * @brief 设置通道的幅度扫描
 * @param Channel 通道编号
 * @param ampS 扫幅起始值
 * @param ampE 扫幅结束值
 * @param FTstep 下降扫幅步进时间
 * @param RTstep 上升扫幅步进时间
 * @param FFstep 下降扫幅步进值
 * @param RFstep 上升扫幅步进值
 * @param DWELL 是否使能暂停（1使能，0不使能）
 */
void AD9959__Sweep_Amp(u8 Channel, u16 ampS, u32 ampE, float FTstep, float RTstep, u32 FFstep, u32 RFstep, u8 DWELL)
{
    u32 RDW0, FDW0;
    u32 SDW, EDW;
    u16 RSRR0, FSRR0;
    SDW = ampS;                   // 起始幅度
    EDW = ampE << 22;             // 结束幅度
    RDW0 = (u32)RFstep;           // 上升步进幅度
    FDW0 = (u32)FFstep;           // 下降步进幅度
    RSRR0 = (u16)(RTstep * 0.4f); // 上升步进时间
    FSRR0 = (u16)(FTstep * 0.4f); // 下降步进时间
    AD9959_Ch(Channel);           // 选择通道
    AD9959_WRrg(0x06, 0x0003ff);  // 设置最大幅度
    if (DWELL)
        AD9959_WRrg(0x03, 0x40c314); // 幅度扫描模式，自动清空累加器，DWELL使能
    else
        AD9959_WRrg(0x03, 0x404314);        // 幅度扫描模式，DWELL禁用
    AD9959_WRrg(0x01, 0xD00000);            // 配置FR1寄存器
    AD9959_WRrg(0x06, SDW);                 // 设置起始幅度
    AD9959_WRrg(0x0A, EDW);                 // 设置结束幅度
    AD9959_WRrg(0x08, RDW0);                // 设置上升步进幅度
    AD9959_WRrg(0x09, FDW0);                // 设置下降步进幅度
    AD9959_WRrg(0x07, FSRR0 * 256 + RSRR0); // 配置步进时间
}

/**
 * @brief 设置通道的相位扫描
 * @param Channel 通道编号
 * @param phaseS 扫相起始值
 * @param phaseE 扫相结束值
 * @param FTstep 下降扫相步进时间
 * @param RTstep 上升扫相步进时间
 * @param FFstep 下降扫相步进值
 * @param RFstep 上升扫相步进值
 * @param DWELL 是否使能暂停（1使能，0不使能）
 */
void AD9959_Sweep_Phase(u8 Channel, u16 phaseS, u32 phaseE, float FTstep, float RTstep, u32 FFstep, u32 RFstep, u8 DWELL)
{
    u32 RDW0, FDW0;
    u32 SDW, EDW;
    u16 RSRR0, FSRR0;
    SDW = phaseS;                   // 起始相位
    EDW = phaseE << 18;             // 结束相位
    RDW0 = (u32)(RFstep * POW_REF); // 上升步进相位值
    FDW0 = (u32)(FFstep * POW_REF); // 下降步进相位值
    RSRR0 = (u16)(RTstep * 0.4f);   // 上升步进时间
    FSRR0 = (u16)(FTstep * 0.4f);   // 下降步进时间
    AD9959_Ch(Channel);             // 选择通道
    AD9959_WRrg(0x06, 0x0003ff);    // 设置最大幅度
    if (DWELL)
        AD9959_WRrg(0x03, 0xc0c314); // 相位扫描模式，自动清空累加器，DWELL使能
    else
        AD9959_WRrg(0x03, 0xc04314);        // 相位扫描模式，DWELL禁用
    AD9959_WRrg(0x01, 0xD00000);            // 配置FR1寄存器
    AD9959_WRrg(0x05, SDW);                 // 设置起始相位
    AD9959_WRrg(0x0A, EDW);                 // 设置结束相位
    AD9959_WRrg(0x08, RDW0);                // 设置上升步进相位
    AD9959_WRrg(0x09, FDW0);                // 设置下降步进相位
    AD9959_WRrg(0x07, FSRR0 * 256 + RSRR0); // 配置步进时间
}

/**
 * @brief 触发扫频
 * @param Channel 通道编号
 */
void AD9959__Sweep_Trigger(u8 Channel)
{
    switch (Channel)
    {
    case 0:
        AD9959_P0_L(); // 拉低通道0引脚
        HAL_Delay(1);
        AD9959_P0_H(); // 拉高通道0引脚
        HAL_Delay(1);
        break;
    case 1:
        AD9959_P0_L();
        HAL_Delay(1);
        AD9959_P0_H();
        HAL_Delay(1);
        break;
    case 2:
        AD9959_P0_L();
        HAL_Delay(1);
        AD9959_P0_H();
        HAL_Delay(1);
        break;
    case 3:
        AD9959_P0_L();
        HAL_Delay(1);
        AD9959_P0_H();
        HAL_Delay(1);
        break;
    default:
        break;
    }
}
/**
 * @brief 拉高通道0的GPIO引脚
 */
void AD9959_P0_H(void)
{
    HAL_GPIO_WritePin(AD9959_P0_GPIO_Port, AD9959_P0_Pin, GPIO_PIN_SET); // 设置通道0引脚为高电平
}

/**
 * @brief 拉低通道0的GPIO引脚
 */
void AD9959_P0_L(void)
{
    HAL_GPIO_WritePin(AD9959_P0_GPIO_Port, AD9959_P0_Pin, GPIO_PIN_RESET); // 设置通道0引脚为低电平
}

/**
 * @brief 拉高通道1的GPIO引脚
 */
void AD9959_P1_H(void)
{
    HAL_GPIO_WritePin(AD9959_P1_GPIO_Port, AD9959_P1_Pin, GPIO_PIN_SET); // 设置通道1引脚为高电平
}

/**
 * @brief 拉低通道1的GPIO引脚
 */
void AD9959_P1_L(void)
{
    HAL_GPIO_WritePin(AD9959_P1_GPIO_Port, AD9959_P1_Pin, GPIO_PIN_RESET); // 设置通道1引脚为低电平
}

/**
 * @brief 拉高更新引脚（UP）
 */
void AD9959_UP_H(void)
{
    HAL_GPIO_WritePin(AD9959_UP_GPIO_Port, AD9959_UP_Pin, GPIO_PIN_SET); // 设置更新引脚为高电平
}

/**
 * @brief 拉低更新引脚（UP）
 */
void AD9959_UP_L(void)
{
    HAL_GPIO_WritePin(AD9959_UP_GPIO_Port, AD9959_UP_Pin, GPIO_PIN_RESET); // 设置更新引脚为低电平
}

/**
 * @brief 拉高省电模式引脚（PDC）
 */
void AD9959_PDC_H(void)
{
    HAL_GPIO_WritePin(AD9959_PDC_GPIO_Port, AD9959_PDC_Pin, GPIO_PIN_SET); // 设置省电模式引脚为高电平
}

/**
 * @brief 拉低省电模式引脚（PDC）
 */
void AD9959_PDC_L(void)
{
    HAL_GPIO_WritePin(AD9959_PDC_GPIO_Port, AD9959_PDC_Pin, GPIO_PIN_RESET); // 设置省电模式引脚为低电平
}

/**
 * @brief 拉高数据引脚（SDIO0）
 */
void AD9959_SDIO0_H(void)
{
    HAL_GPIO_WritePin(AD9959_SDIO0_GPIO_Port, AD9959_SDIO0_Pin, GPIO_PIN_SET); // 设置数据引脚SDIO0为高电平
}

/**
 * @brief 拉低数据引脚（SDIO0）
 */
void AD9959_SDIO0_L(void)
{
    HAL_GPIO_WritePin(AD9959_SDIO0_GPIO_Port, AD9959_SDIO0_Pin, GPIO_PIN_RESET); // 设置数据引脚SDIO0为低电平
}

/**
 * @brief 拉高通道2的GPIO引脚
 */
void AD9959_P2_H(void)
{
    HAL_GPIO_WritePin(AD9959_P2_GPIO_Port, AD9959_P2_Pin, GPIO_PIN_SET); // 设置通道2引脚为高电平
}

/**
 * @brief 拉低通道2的GPIO引脚
 */
void AD9959_P2_L(void)
{
    HAL_GPIO_WritePin(AD9959_P2_GPIO_Port, AD9959_P2_Pin, GPIO_PIN_RESET); // 设置通道2引脚为低电平
}

/**
 * @brief 拉高时钟引脚（SCK）
 */
void AD9959_SCK_H(void)
{
    HAL_GPIO_WritePin(AD9959_SCK_GPIO_Port, AD9959_SCK_Pin, GPIO_PIN_SET); // 设置时钟引脚为高电平
}

/**
 * @brief 拉低时钟引脚（SCK）
 */
void AD9959_SCK_L(void)
{
    HAL_GPIO_WritePin(AD9959_SCK_GPIO_Port, AD9959_SCK_Pin, GPIO_PIN_RESET); // 设置时钟引脚为低电平
}

/**
 * @brief 拉高片选引脚（CS）
 */
void AD9959_CS_H(void)
{
    HAL_GPIO_WritePin(AD9959_CS_GPIO_Port, AD9959_CS_Pin, GPIO_PIN_SET); // 设置片选引脚为高电平
}

/**
 * @brief 拉低片选引脚（CS）
 */
void AD9959_CS_L(void)
{
    HAL_GPIO_WritePin(AD9959_CS_GPIO_Port, AD9959_CS_Pin, GPIO_PIN_RESET); // 设置片选引脚为低电平
}

/**
 * @brief 拉高复位引脚（RST）
 */
void AD9959_RST_H(void)
{
    HAL_GPIO_WritePin(AD9959_RST_GPIO_Port, AD9959_RST_Pin, GPIO_PIN_SET); // 设置复位引脚为高电平
}

/**
 * @brief 拉低复位引脚（RST）
 */
void AD9959_RST_L(void)
{
    HAL_GPIO_WritePin(AD9959_RST_GPIO_Port, AD9959_RST_Pin, GPIO_PIN_RESET); // 设置复位引脚为低电平
}

/**
 * @brief 拉高数据引脚（SDIO1）
 */
void AD9959_SDIO1_H(void)
{
    HAL_GPIO_WritePin(AD9959_SDIO1_GPIO_Port, AD9959_SDIO1_Pin, GPIO_PIN_SET); // 设置数据引脚SDIO1为高电平
}

/**
 * @brief 拉低数据引脚（SDIO1）
 */
void AD9959_SDIO1_L(void)
{
    HAL_GPIO_WritePin(AD9959_SDIO1_GPIO_Port, AD9959_SDIO1_Pin, GPIO_PIN_RESET); // 设置数据引脚SDIO1为低电平
}

/**
 * @brief 拉高通道3的GPIO引脚
 */
void AD9959_P3_H(void)
{
    HAL_GPIO_WritePin(AD9959_P3_GPIO_Port, AD9959_P3_Pin, GPIO_PIN_SET); // 设置通道3引脚为高电平
}

/**
 * @brief 拉低通道3的GPIO引脚
 */
void AD9959_P3_L(void)
{
    HAL_GPIO_WritePin(AD9959_P3_GPIO_Port, AD9959_P3_Pin, GPIO_PIN_RESET); // 设置通道3引脚为低电平
}

/**
 * @brief 拉高数据引脚（SDIO2）
 */
void AD9959_SDIO2_H(void)
{
    HAL_GPIO_WritePin(AD9959_SDIO2_GPIO_Port, AD9959_SDIO2_Pin, GPIO_PIN_SET); // 设置数据引脚SDIO2为高电平
}

/**
 * @brief 拉低数据引脚（SDIO2）
 */
void AD9959_SDIO2_L(void)
{
    HAL_GPIO_WritePin(AD9959_SDIO2_GPIO_Port, AD9959_SDIO2_Pin, GPIO_PIN_RESET); // 设置数据引脚SDIO2为低电平
}

/**
 * @brief 拉高数据引脚（SDIO3）
 */
void AD9959_SDIO3_H(void)
{
    HAL_GPIO_WritePin(AD9959_SDIO3_GPIO_Port, AD9959_SDIO3_Pin, GPIO_PIN_SET); // 设置数据引脚SDIO3为高电平
}

/**
 * @brief 拉低数据引脚（SDIO3）
 */
void AD9959_SDIO3_L(void)
{ 
    HAL_GPIO_WritePin(AD9959_SDIO3_GPIO_Port, AD9959_SDIO3_Pin, GPIO_PIN_RESET); // 设置数据引脚SDIO3为低电平
}


u32 Carrier[11]={0,55,105,165,215,275,325,385,435,485,555};//直达载波
u8 Carrier_indx=7;
u32 val=600;
u32 pid_vin=38;
u32 Modulated_wave=400;//直达调制
u32 Carrier1;//多径载波
u32 Modulated_wave1;//多径调制
u8 mdoe_flag=0;//CW 0 AM 1
u16 Phase = 0;
void AD9959_proc(void)
{
	  
	// 这里进行幅度的信息输入，下面函数参数为
    // 通道，频率，初始相位，幅度                      
    AD9959_Single_Output(0, 2000000,Phase ,pid_vin);//100mv-80,1v-830
    if(mdoe_flag==0)
		{
			
			AD9959_Single_Output(1, 2000, 0, 0);
		}			
    else
		{
			AD9959_Single_Output(1, 2000, 0, 200);
		}
			
		
		
    //AD9959_Single_Output(2, 2000000, 0, 1023);
   // AD9959_Single_Output(3, 100000, 0, 1023);
    AD9959_IO_UpDate(); // 这段代码在你每次进行赋值修改后都要打过去
	  HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_2);
		//my_printf(&huart1,"%d\r\n",uwTick);
	  
	
}



