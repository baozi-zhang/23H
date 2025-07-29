#include "cmd_to_fun.h"
/**
 * @brief 初始化控制器
 *
 * 该函数用于初始化控制器，将控制器数据设置为0。
 */
void CTRL_INIT(void)
{
    CTRL_DATA = 0; // 初始化控制器
}
//------- DA系列 -------

/**
 * @brief 启动DA波形发生器
 * @details 通过对 `CTRL_DATA` 寄存器执行按位或操作，将 DA_FREQ_EN 位置为1。
 * 这会使能FPGA内部的DA模块，开始生成并输出波形，而不影响其他控制位的状态。
 */
void DA_FPGA_START()
{
    CTRL_DATA = CTRL_DATA | DA_FREQ_EN; // 置位 DA 使能位，启动DA输出
}

/**
 * @brief 停止DA波形发生器
 * @details 通过对 `CTRL_DATA` 寄存器执行按位与非操作，将 DA_FREQ_EN 位清零。
 * 这会禁用FPGA内部的DA模块，停止输出波形。
 */
void DA_FPGA_STOP()
{
    CTRL_DATA = CTRL_DATA & (~DA_FREQ_EN); // 清零 DA 使能位，停止DA输出
}
//----- AD测频系列 ------
/**
 * @brief 启用AD测频清除
 *
 * 根据指定的通道号，启用相应的AD测频清除功能。
 *
 * @param ch 通道号，1代表AD1，2代表AD2
 */
void AD_FREQ_CLR_ENABLE(int ch)
{
    switch (ch) {
    case 1:
        CTRL_DATA = CTRL_DATA & (~AD1_FREQ_CLR); // 清除AD1测频的数据
        break;
    case 2:
        CTRL_DATA = CTRL_DATA & (~AD2_FREQ_CLR); // 清除AD2测频的数据
        break;
    }
}
/**
 * @brief 禁用AD测频数据清除
 *
 * 根据给定的通道号，禁用对AD测频数据的清除操作。
 *
 * @param ch 通道号，取值范围为1或2
 */
void AD_FREQ_CLR_DISABLE(int ch)
{
    switch (ch) {
    case 1:
        CTRL_DATA = CTRL_DATA | AD1_FREQ_CLR; // 不清除AD1测频的数据
        break;
    case 2:
        CTRL_DATA = CTRL_DATA | AD2_FREQ_CLR; // 不清除AD2测频的数据
        break;
    }
}
/**
 * @brief 开始测频
 *
 * 根据给定的通道号启动对应的模数转换器（AD）进行频率测量。
 *
 * @param ch 通道号，取值范围为1或2
 *           - 1：启动AD1测频
 *           - 2：启动AD2测频
 */
void AD_FREQ_START(int ch)
{
    switch (ch) {
    case 1:
        CTRL_DATA = CTRL_DATA | AD1_FREQ_START; // 启动AD1测频
        break;
    case 2:
        CTRL_DATA = CTRL_DATA | AD2_FREQ_START; // 启动AD2测频
        break;
    }
}
/**
 * @brief 停止AD测频
 *
 * 根据指定的通道号，停止对应通道的AD测频。
 *
 * @param ch 通道号，取值范围：1-2
 *        - 1：关闭AD1测频
 *        - 2：关闭AD2测频
 */
void AD_FREQ_STOP(int ch)
{
    switch (ch) {
    case 1:
        CTRL_DATA = CTRL_DATA & (~AD1_FREQ_START); // 关闭AD1测频
        break;
    case 2:
        CTRL_DATA = CTRL_DATA & (~AD2_FREQ_START); // 关闭AD2测频
        break;
    }
}
//------- AD系列 -------
/**
 * @brief 设置AD采样频率
 *
 * 根据给定的通道号，设置对应的AD采样频率生成。
 *
 * @param ch 通道号，1表示AD1，2表示AD2
 */
void AD_FREQ_SET(int ch)
{
    switch (ch) {
    case 1:
        CTRL_DATA = CTRL_DATA | AD1_FREQ_EN; // 保持原本控制不变，开启AD1的采样频率生成
        break;
    case 2:
        CTRL_DATA = CTRL_DATA | AD2_FREQ_EN; // 保持原本控制不变，开启AD2的采样频率生成
        break;
    case 3:
        CTRL_DATA = CTRL_DATA | AD1_FREQ_EN | AD2_FREQ_EN; // 保持原本控制不变，开启AD1的采样频率生成
        break;
    }
}
/**
 * @brief 设置AD采样频率
 *
 * 根据给定的通道号，设置对应的AD采样频率生成。
 *
 * @param ch 通道号，1表示AD1，2表示AD2
 */
void AD_FIFO_WRITE_ENABLE(int ch)
{
    switch (ch) {
    case 1:
        CTRL_DATA = CTRL_DATA | AD1_FIFO_WR; // 开启AD1的FIFO写入
        break;
    case 2:
        CTRL_DATA = CTRL_DATA | AD2_FIFO_WR; // 开启AD2的FIFO写入
        break;
    case 3:
        CTRL_DATA = CTRL_DATA | AD1_FIFO_WR | AD2_FIFO_WR; // 开启AD2的FIFO写入
        break;
    }
}
/**
 * @brief 禁用AD FIFO写入
 *
 * 根据给定的通道号禁用AD FIFO的写入功能。
 *
 * @param ch 通道号
 *          - 1: 表示AD1通道
 *          - 2: 表示AD2通道
 */
void AD_FIFO_WRITE_DISABLE(int ch)
{
    switch (ch) {
    case 1:
        CTRL_DATA = CTRL_DATA & (~AD1_FIFO_WR); // 关闭AD1的FIFO写入
        break;
    case 2:
        CTRL_DATA = CTRL_DATA & (~AD2_FIFO_WR); // 关闭AD2的FIFO写入
        break;
    }
}
/**
 * @brief 启用AD FIFO读取
 *
 * 根据给定的通道号，启用对应AD的FIFO读取功能。
 *
 * @param ch 通道号，取值范围为1或2
 */
void AD_FIFO_READ_ENABLE(int ch)
{
    switch (ch) {
    case 1:
        CTRL_DATA = CTRL_DATA | AD1_FIFO_RD; // 开启AD1的FIFO读取
        break;
    case 2:
        CTRL_DATA = CTRL_DATA | AD2_FIFO_RD; // 开启AD2的FIFO读取
        break;
    }
}
/**
 * @brief 禁用AD FIFO读取
 *
 * 根据给定的通道号，禁用对应的AD FIFO读取功能。
 *
 * @param ch 通道号
 *        - 1: 禁用AD1的FIFO读取
 *        - 2: 禁用AD2的FIFO读取
 */
void AD_FIFO_READ_DISABLE(int ch)
{
    switch (ch) {
    case 1:
        CTRL_DATA = CTRL_DATA & (~AD1_FIFO_RD); // 关闭AD1的FIFO读取
        break;
    case 2:
        CTRL_DATA = CTRL_DATA & (~AD2_FIFO_RD); // 关闭AD2的FIFO读取
        break;
    }
}
