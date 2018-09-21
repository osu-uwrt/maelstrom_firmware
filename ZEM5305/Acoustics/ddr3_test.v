
`timescale 1ns/1ps
//`default_nettype none

module ddr3_test (
	input  wire          clk,
	input  wire          reset,
	input  wire          writes_en,
	input  wire          reads_en,
	input  wire          calib_done,

	//DDR Input Buffer (ib_)
	output reg           ib_re,
	input  wire [127:0]  ib_data,
	input  wire [8:0]   ib_count,
	input  wire          ib_empty,

	//DDR Output Buffer (ob_)
	output reg           ob_we,
	output reg  [127:0]  ob_data,
	input  wire [8:0]   ob_count,

	input  wire          avl_ready,
	output reg           avl_burstbegin,
	output reg  [24:0]   avl_address,
	output wire [7:0]    avl_size,

	output reg           avl_read_req,
	input  wire          avl_rdata_valid,
	input  wire [127:0]  avl_rdata,

	output reg           avl_write_req,
	output wire [127:0]  avl_wdata,
	output wire [15:0]   avl_be
	);

localparam FIFO_SIZE      = 256; // Number of 128-bit words in input/output FIFOs
localparam BURST_LEN      = 16;  // Number of 128bit user words per DRAM command (Must be Multiple of 2)

reg  [24:0] avl_addr_wr, avl_addr_rd;
reg  [7:0]  burst_cnt;

reg         write_mode;
reg         read_mode;
reg         reset_d;
reg         avl_rdata_was_valid;


assign avl_size  = BURST_LEN;
assign avl_be    = 16'hFFFF;
assign avl_wdata = ib_data;

always @(posedge clk) write_mode <= writes_en;
always @(posedge clk) read_mode <= reads_en;
always @(posedge clk) reset_d <= reset;


reg [31:0] state;
localparam s_idle   = 0,
           s_write0 = 10,
           s_write1 = 11,
           s_write2 = 12,
           s_read1  = 20,
           s_read2  = 21,
           s_read3  = 22,
           s_read4  = 23;
always @(posedge clk) begin
	if (reset_d) begin
		state          <= s_idle;
		burst_cnt      <= 3'b000;
		avl_addr_wr    <= 0;
		avl_addr_rd    <= 0;
		avl_address    <= 25'b0;
		avl_read_req    <= 1'b0;
		avl_write_req   <= 1'b0;
		avl_rdata_was_valid <= 1'b0;
	end else begin
		avl_burstbegin  <= 1'b0;
		avl_write_req   <= 1'b0;
		ib_re           <= 1'b0;
		avl_read_req    <= 1'b0;
		ob_we           <= 1'b0;


		case (state)
			s_idle: begin
				burst_cnt <= BURST_LEN;

				// only start writing when initialization done
				if (calib_done==1 && write_mode==1 && (ib_count >= BURST_LEN) && (avl_ready == 1'b1) ) begin
					burst_cnt     <= BURST_LEN - 2;
					avl_address <= avl_addr_wr;
					avl_addr_wr <= avl_addr_wr + BURST_LEN;
					state <= s_write0;
				end else if (calib_done==1 && read_mode==1 && (ob_count<(FIFO_SIZE-1-BURST_LEN) ) && (avl_ready == 1'b1) ) begin
					burst_cnt     <= BURST_LEN - 1;
					avl_address <= avl_addr_rd;
					avl_addr_rd <= avl_addr_rd + BURST_LEN;
					state <= s_read1;
				end
			end

			s_write0: begin
				if (avl_ready == 1'b1) begin
					ib_re <= 1'b1;
					state <= s_write1;
				end
			end

			s_write1: begin
				if (avl_ready == 1'b1) begin
					avl_burstbegin  <= 1'b1;
					avl_write_req   <= 1'b1;
					ib_re <= 1'b1;
					state <= s_write2;
				end
			end

			s_write2: begin
				avl_write_req   <= 1'b1;
				// Keep write request and data present until
				// controller is ready.
				if (avl_ready == 1'b1) begin
					if (burst_cnt == 3'd0) begin
						state <= s_idle;
					end else begin
						ib_re <= 1'b1;
						burst_cnt <= burst_cnt - 1'b1;
					end
				end
			end

			s_read1: begin
				avl_burstbegin <= 1'b1;
				avl_read_req   <= 1'b1;
				state          <= s_read2;
			end

			s_read2: begin
				if (avl_rdata_valid == 1'b1) begin
					ob_data <= avl_rdata;
					ob_we <= 1'b1;
					if (burst_cnt == 3'd0) begin
						state <= s_idle;
					end else begin
						burst_cnt <= burst_cnt - 1'b1;
					end
				end
			end
		endcase
	end
end


endmodule
