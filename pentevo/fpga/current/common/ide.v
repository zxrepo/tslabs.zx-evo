
// IDE interface

// PIO4 mode
// (assuming that delay from internal FPGA's logics and sinals at HDD's edge connector is ~35ns, e.g. 28MHz clk period)
// clk              |誉_覧__慾誉_慾誉_覧__慾誉_覧__慾誉_慾誉_覧__慾誉_覧__慾
// st[n]            |<--><00>|<01>|<02><03>|<04><00>|<01>|<02><03>|<04><-->|
// go               |覧覧____|____|________|覧覧____|____|________|________|
// rdy              |覧覧____|____|________|覧覧____|____|________|____覧覧|
// din_stb          |________|____|________|覧覧____|____|________|________|
// ide_out          |xxxxxxxx|xxxx|xxxxxxxx|WWWWWWWW|WWWW|WWWWWWWW|WWWWWWWW|
// ide_d            |xxxxxxxx|xxxx|xxxxxxxx|xxRRWWWW|WWWW|WWWWWWWW|zzzzzzzz|
// ide_a            |xxxxAAAA|AAAA|AAAAAAAA|AAAAaaaa|aaaa|aaaaaaaa|aaaaxxxx|
// ide_dir (rnw)    |覧覧覧覧|覧覧|覧覧覧覧|覧覧____|____|________|覧覧覧覧|
// ide_cs_n         |覧覧____|____|________|覧覧____|____|________|覧覧覧覧|
// ide_rd_n         |覧覧覧覧|____|____覧覧|覧覧覧覧|覧覧|覧覧覧覧|覧覧覧覧|
// ide_wr_n         |覧覧覧覧|覧覧|覧覧覧覧|覧覧覧覧|____|____覧覧|覧覧覧覧|
//
//                  | t1 |   t2   |  teoc  | t1 |   t2   |  teoc  |
// ide_d (hdd)      |zzzzzzzz|zzzz|zzzzzzRR|RRRRzzzz|WWWW|WWWWWWWW|WWWWzzzz|
// ide_a (hdd)      |xxxxxxxx|AAAA|AAAAAAAA|AAAAAAAA|aaaa|aaaaaaaa|aaaaaaaa|
// ide_cs_n  (hdd)  |覧覧覧覧|____|________|____覧覧|____|________|____覧覧|
// ide_rd_n  (hdd)  |覧覧覧覧|覧覧|________|覧覧覧覧|覧覧|覧覧覧覧|覧覧覧覧|
// ide_wr_n  (hdd)  |覧覧覧覧|覧覧|覧覧覧覧|覧覧覧覧|覧覧|________|覧覧覧覧|

`include "tune.v"

module ide
(
  // clocks
  input wire         clk,

  // controls
  input  wire        reset,
  output wire        rdy_stb,
  output wire        rdy,
  // output wire next,

  // IDE interface
  output  wire [15:0] ide_out,
  output wire [ 2:0] ide_a,
  output reg         ide_dir,     // rnw
  output reg         ide_cs0_n,
  output reg         ide_cs1_n,
  output reg         ide_rd_n,
  output reg         ide_wr_n,

  // DMA interface
  input wire [15:0]  dma_out,
  input wire         dma_req,
  input wire         dma_rnw,

  // Z80 interface
  input wire [15:0]  z80_out,
  input wire [ 2:0]  z80_a,
  input wire         z80_cs0_n,
  input wire         z80_cs1_n,
  input wire         z80_req,
  input wire         z80_rnw
);

  reg [4:0] st;
  
  assign ide_out = dma_req ? dma_out : z80_out;
  assign ide_a = dma_req ? 3'b0 : z80_a;
  wire dir = dma_req ? dma_rnw : z80_rnw;
  wire cs0_n = dma_req ? 1'b0 : z80_cs0_n;
  wire cs1_n = dma_req ? 1'b1 : z80_cs1_n;
  wire rd_n = dma_req ? ~dma_rnw : ~z80_rnw;
  wire wr_n = dma_req ? dma_rnw : z80_rnw;

  wire go = (dma_req || z80_req) && rdy;
  assign rdy_stb = st[4];
  assign rdy = ~|st;

  // states clocking
  always @(posedge clk)
    if (reset)
      st <= 5'b0;
    else
      st <= {st[3:0], go};

  // states processing
  always @(posedge clk)
    if (reset)
    begin
      ide_dir   <= 1'b1;
      ide_cs0_n <= 1'b1;
      ide_cs1_n <= 1'b1;
      ide_rd_n  <= 1'b1;
      ide_wr_n  <= 1'b1;
    end

    else if (go)
    begin
      ide_dir   <= dir;
      ide_cs0_n <= cs0_n;
      ide_cs1_n <= cs1_n;
    end

    else if (st[0])
    begin
      ide_rd_n  <= rd_n;
      ide_wr_n  <= wr_n;
    end

    else if (st[2])
    begin
      ide_rd_n  <= 1'b1;
      ide_wr_n  <= 1'b1;
    end

    else if (st[3])
    begin
      ide_dir   <= 1'b1;
      ide_cs0_n <= 1'b1;
      ide_cs1_n <= 1'b1;
    end

endmodule
