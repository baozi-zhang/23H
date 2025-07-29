//&----------------------------------------------------------------------------------------
//& 模块名: DA_WAVEFORM_B
//& 文件名: DA_WAVEFORM_B.v
//& 作  者: 左岚
//& 日  期: 2025-07-18
//&
//& 功  能: DA波形发生器模块 (B通道)。此模块功能与A通道类似，但它使用写入到
//&         地址ADDR12的数据总线的**高8位**(`WAVEFORM[15:8]`)作为波形选择码。
//&         它从四个不同的波形ROM中选择并输出一种波形数据。
//&
//& 设计警告:
//& 本模块中用于写入波形选择控制字`WAVEFORM_B`的 `always @(*)` 块
//& 会综合成一个锁存器(Latch)，这在同步设计中通常是不推荐的。
//& 建议使用同步时钟逻辑来更新该控制寄存器。
//&----------------------------------------------------------------------------------------

module DA_WAVEFORM_B #(
    parameter ADDR12 = 16'h000C  // 波形选择控制字的写入地址
) (
    // --- 端口定义 ---
    input         CLK,       // 系统主时钟
    input  [15:0] WAVEFORM,  // 从总线输入的16位数据
    input  [ 9:0] addr_b,    // 10位的ROM地址输入 (B通道相位)
    // -- 总线写控制
    input         CS,        // 片选信号，低电平有效
    input         WR_EN,     // 写使能信号，高电平有效
    input  [15:0] ADDR,      // 16位地址总线
    // -- 输出
    output [13:0] POW_B      // B通道的14位波形数据输出
);


  // --- 内部信号定义 ---
  wire [13:0] sin_data;  // 来自正弦波ROM的数据
  wire [13:0] tri_data;  // 来自三角波ROM的数据
  wire [13:0] squ_data;  // 来自方波ROM的数据
  wire [13:0] swt_data;  // 来自锯齿波ROM的数据

  // 波形选择控制寄存器 (会综合成锁存器)
  // 0: 正弦波, 1: 方波, 2: 三角波, 3: 锯齿波
  reg  [ 7:0] WAVEFORM_B;

  // 两级流水线寄存器，用于缓冲输出数据，改善时序
  reg  [13:0] WAVE_DATA_B;  // 流水线第一级
  reg  [13:0] WAVE_DATA_B_reg;  // 流水线第二级

  // --- 波形选择控制字写入逻辑 (会生成锁存器) ---
  always @(*) begin
    if (!CS && WR_EN) begin
      case (ADDR)
        // 当地址匹配时，锁存总线数据的高8位作为B通道的波形选择码
        ADDR12: WAVEFORM_B <= WAVEFORM[15:8];
      endcase
    end
  end

  // --- 波形ROM实例化 ---
  // 每个ROM根据相同的地址(相位)输入，输出对应波形的采样点值

  // 正弦波ROM Sine wave ROM
  sinromvpp sinromvpp_inst (
      .address(addr_b),
      .clock  (CLK),
      .q      (sin_data)
  );

  // 方波ROM Square wave ROM
  sqaure_rom sqaure_rom_inst (
      .address(addr_b),
      .clock  (CLK),
      .q      (squ_data)
  );

  // 三角波ROM Triangle wave ROM
  triangle_rom triangle_rom_inst (
      .address(addr_b),
      .clock  (CLK),
      .q      (tri_data)
  );

  // 锯齿波ROM Sawtooth wave ROM
  sawtooth_rom sawtooth_rom_inst (
      .address(addr_b),
      .clock  (CLK),
      .q      (swt_data)
  );

  // --- 波形选择和流水线寄存器逻辑 ---
  // 这个时序逻辑块实现了一个多路选择器，根据WAVEFORM_B的值选择一个波形数据
  // 并通过两级流水线寄存器输出。
  always @(posedge CLK) begin
    case (WAVEFORM_B)
      8'd0: begin  // 选择正弦波
        WAVE_DATA_B     <= sin_data;
        WAVE_DATA_B_reg <= WAVE_DATA_B;  // 第二级流水线寄存器锁存第一级的值
      end
      8'd1: begin  // 选择方波
        WAVE_DATA_B     <= squ_data;
        WAVE_DATA_B_reg <= WAVE_DATA_B;
      end
      8'd2: begin  // 选择三角波
        WAVE_DATA_B     <= tri_data;
        WAVE_DATA_B_reg <= WAVE_DATA_B;
      end
      8'd3: begin  // 选择锯齿波
        WAVE_DATA_B     <= swt_data;
        WAVE_DATA_B_reg <= WAVE_DATA_B;
      end
      default: begin  // 默认选择正弦波
        WAVE_DATA_B     <= sin_data;
        WAVE_DATA_B_reg <= WAVE_DATA_B;
      end
    endcase
  end

  // --- 最终输出赋值 ---
  // 将第二级流水线寄存器的值赋给模块输出
  assign POW_B = WAVE_DATA_B_reg;

endmodule
