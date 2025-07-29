//&----------------------------------------------------------------------------------------
//& 模块名: CNT32
//& 文件名: CNT32.v
//& 作  者: 左岚
//& 日  期: 2025-07-18
//&
//& 功  能: 32位计数器模块。本模块内部集成了两个独立的32位同步加法计数器。
//&         每个计数器都有自己的时钟和使能信号，但共享同一个异步复位信号。
//&----------------------------------------------------------------------------------------

module CNT32 (
    // --- 端口定义 ---
    input         CLR,        // 异步复位信号，低电平有效，同时复位两个计数器
    input         CLK,        // 计数器1的时钟信号
    input         CLKBASE,    // 计数器2 (基准计数器) 的时钟信号
    input         CLKEN,      // 计数器1的同步使能信号，高电平有效
    input         CLKBASEEN,  // 计数器2的同步使能信号，高电平有效
    output [31:0] Q,          // 计数器1的32位计数值输出
    output [31:0] QBASE       // 计数器2的32位计数值输出
);

  // --- 内部信号定义 ---
  reg [31:0] Q1, Q1BASE;  // 两个计数器的内部寄存器

  // --- 第一个计数器 (Q) 的逻辑实现 ---
  // 时序逻辑块，在CLK的上升沿或CLR的下降沿触发
  always @(posedge CLK or negedge CLR) begin
    // 异步复位逻辑：当CLR为低电平时，立即将计数器清零
    if (!CLR) begin
      Q1 <= 32'b0;
    end  // 同步计数逻辑：如果未复位，在CLK上升沿判断使能信号
    else if (CLKEN) begin
      Q1 <= Q1 + 1;  // 当CLKEN为高时，计数器加1
    end
  end

  // 将内部寄存器Q1的值持续赋给输出端口Q
  assign Q = Q1;

  // --- 第二个计数器 (QBASE) 的逻辑实现 ---
  // 时序逻辑块，在CLKBASE的上升沿或CLR的下降沿触发
  always @(posedge CLKBASE or negedge CLR) begin
    // 异步复位逻辑：当CLR为低电平时，立即将计数器清零
    if (!CLR) begin
      Q1BASE <= 32'b0;
    end  // 同步计数逻辑：如果未复位，在CLKBASE上升沿判断使能信号
    else if (CLKBASEEN) begin
      Q1BASE <= Q1BASE + 1;  // 当CLKBASEEN为高时，计数器加1
    end
  end

  // 将内部寄存器Q1BASE的值持续赋给输出端口QBASE
  assign QBASE = Q1BASE;

endmodule
