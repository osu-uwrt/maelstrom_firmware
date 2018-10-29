//------------------------------------------------------------------------
// Acoustics.v
//
// This file reads data and stores it in ram, then offloads to computer
//
// Host Interface registers:
// WireIn 0x00 bits
//     0 - Enable reading data off device (0=disabled, 1=enabled)
//     1 - Collect and save ADC data (0=disabled, 1=enabled)
//     2 - Reset
//
// PipeOut 0xA0 - Read port (U11, DDR3)
//
//------------------------------------------------------------------------
// 1. Computer sends signal to clear memory (Bit 2)
// 2. Computer enables reading (Bit 1)
// 3. Collect data and store it in storage FIFO
// 4. Data is pulled out of storage FIFO and put in DDR3
// 5. Computer stops collections and starts reading (Bit 0)
// 6. DDR3 dumps data into recall FIFO
// 7. PC pulls data out of recall FIFO through pipe out
// 8. Repeat...
// -----------------------------------------------------------------------

`timescale 1ns/1ps

// Input and output pins on actual FPGA
module Acoustics (
	// USB Interface
	input  wire [  4: 0]       okUH,
	output wire [  2: 0]       okHU,
	inout  wire [ 31: 0]       okUHU,
	inout  wire                okAA,
	
	// DDR3 Interface
	output wire [ 14: 0]       mem_addr,
	output wire [  2: 0]       mem_ba,
	output wire                mem_cas_n,
	output wire [  0: 0]       mem_cke,
	output wire [  0: 0]       mem_clk,
	output wire [  0: 0]       mem_clk_n,
	output wire [  0: 0]       mem_cs_n,
	output wire [  1: 0]       mem_dm,
	inout  wire [ 15: 0]       mem_dq,
	inout  wire [  1: 0]       mem_dqs,
	inout  wire [  1: 0]       mem_dqs_n,
	output wire [  0: 0]       mem_odt,
	output wire                mem_ras_n,
	output wire                mem_we_n,
	input  wire [  0: 0]       mem_rzqin,
	output wire                mem_reset_n,
	
	// System clock
	input  wire                sys_clk_p,
	input  wire                sys_clk_n,

	// ADC pins
	input  wire      				drdy_in, 	// Goes low when ADC has a data point
	output wire						sclk_out, 	// Used to request a bit. On rising edge, data will be placed on miso
	input  wire						miso_pf_in, // Data from PF ADC
	input  wire						miso_pa_in, // Data from PA ADC
	input  wire						miso_sf_in, // Data from SF ADC
	input  wire						miso_sa_in, // Data from SA ADC
	output wire						start, 		// ADC only collects when high
	output wire						clk_out,		// Master clock for ADC
	
	// Test
	output wire [  3: 0]       led			// Device leds
);

localparam BLOCK_SIZE      = 128;   // How many words the computer reads at a time. 512 bytes / 4 byte per word;
localparam FIFO_SIZE       = 1024;  // Number of 32-bit words stored in input/output FIFOs

// USB signals
wire           okClk;
wire [112:0]   okHE;
wire [64:0]    okEH;

// Wire from computer (computer controls the state of each bit at any point in time)
// 	Bit 0 - Enable reading data off device (0=disabled, 1=enabled)
//    Bit 1 - Collect and save ADC data (0=disabled, 1=enabled)
//		Bit 2 - Reset
wire [31:0]  ep00wire;

// Pipe
reg          pipe_out_ready;  		// Can computer read now?

// Fifo controls
reg 			 store_fifo_write;		// Should storage fifo input data?
wire         store_fifo_read;			// Should storage fifo output data?
wire [127:0] store_fifo_data;			// Storage fifo output to DDR3
wire [7:0]   store_fifo_rd_count;	// How many items are available to read
wire [7:0]   store_fifo_wr_count;	// How many items have been written and still remain
wire         store_fifo_full;			// Is storage fifo full?
wire         store_fifo_empty;		// Is storage fifo empty?

wire			 recall_fifo_write;		// Should storage fifo input data?
wire [127:0] recall_fifo_datain;		// Data from DDR3 to store
wire [10:0]  recall_fifo_rd_count;	// How many items are available to read
wire [8:0]   recall_fifo_wr_count;	// How many items have been written and still remain
wire         recall_fifo_full;		// Is storage fifo full?
wire         recall_fifo_empty;		// Is storage fifo empty?
wire 			 recall_fifo_read;		// Should storage fifo output data?
wire 			 recall_fifo_dataout;	// Data out to PC


// DDR3 signals
wire        	phy_clk;              // 333MHz Memory PHY Clk
reg         	global_reset_n;
wire        	afi_reset_n;
wire           mem_avl_ready;
wire           mem_avl_burstbegin;
wire [24:0]    mem_avl_addr;
wire           mem_avl_rdata_valid;
wire [127:0]   mem_avl_rdata;
wire [127:0]   mem_avl_wdata;
wire [15:0]    mem_avl_be;
wire           mem_avl_read_req;
wire           mem_avl_write_req;
wire [7:0]     mem_avl_size;
wire           mem_avl_clk;
wire           mem_init_done;
wire           mem_cal_success;
wire           mem_cal_fail;
wire           mem_pll_locked;


// Clocks
wire        	sys_clk;
wire 				master_clk;

reg [127:0]		adc_data;  			// Data buffer
reg 				sending_sclk;
reg 				start_sclk;
reg 				reset_sclk;




// Weird code to ensure DDR3 initilizes
reg [3:0] rst_cnt;
initial rst_cnt = 4'b0;
always @(posedge okClk) begin
	if(rst_cnt < 4'b1000) begin
		rst_cnt <= rst_cnt + 1;
		global_reset_n <= 1'b0;
	end
	else begin
		global_reset_n <= 1'b1;
	end
end


// Allow computer to read if we have enough data in memory.
// Computer reads one block at a time. If we have a block of data...
always @(posedge okClk) begin
	if(recall_fifo_rd_count >= BLOCK_SIZE) begin
		pipe_out_ready <= 1'b1;
	end else begin
		pipe_out_ready <= 1'b0;
	end
end


always @(negedge master_clk) begin
	if (start_sclk) begin 								// When time to start requesting bits...
		sending_sclk <= 1'b1;								// Start requesting bits
		reset_sclk <= 1'b1;									// Tell other process to reset start_sclk
		adc_data <= 128'h000001000000000000000000;	// Initialize data with one bit set
	end else if (sending_sclk) begin					// If mid transfer...
		if (adc_data[95]) begin								// If our original 1 has been shifted to the 23rd position, this is our bit
			store_fifo_write <= 1'b1;							// Tell Fifo to store this data on next clock
			sending_sclk <= 1'b0;								// Stop requesting bits
		end
		adc_data <= adc_data << 1;							
		adc_data[72] <= miso_pf_in;						
		adc_data[48] <= miso_pa_in;						
		adc_data[24] <= miso_sf_in;						// Shift data ad insert new bit
		adc_data[0] <= miso_sa_in;	
	end else begin											// If we are not transfering data
		store_fifo_write <= 1'b0; 							// Ensure fifo is not trying to save data
		reset_sclk <= 1'b0;									// Ensure start_sclk is allowed to be asserted
	end
end

// When drdy falls, set start_sclk to tell other process to stop reading.
// reset_sclk will be set by that process when it aknowledged the start flag
always @(negedge drdy_in, posedge reset_sclk) begin
	if (reset_sclk) begin
		start_sclk <= 1'b0;
	end else begin
		start_sclk <= 1'b1;
	end
end


assign sclk_out = (sending_sclk) ? master_clk : 1'b0; // Request bits when sending_sclk (asserted when drdy falls)
assign clk_out = master_clk;									// clk_out always has clock on it
assign start = ep00wire[1];									// Tell ADC to collect data when collect bit is set from PC

assign led[0] = 1'b1;											// Led 0 always on
assign led[1] = (ep00wire[2] == 1'b1) ? 1'b0 : 1'bz;	// Led 1 on when "reset" bit is high from pc
assign led[2] = (start == 1'b1) ? 1'b0 : 1'bz;			// Led 2 on when collecting data from ADC
assign led[3] = (ep00wire[0] == 1'b1) ? 1'b0 : 1'bz;	// Led 3 on when "read" bit is high from pc


// Usb Stuff
// -------------------------------------------------------------------
	wire [65*1-1:0]  okEHx;

	okHost okHI(
		.okUH(okUH),
		.okHU(okHU),
		.okUHU(okUHU),
		.okAA(okAA),
		.okClk(okClk),
		.okHE(okHE),
		.okEH(okEH)
		);

	okWireOR # (.N(1)) wireOR (okEH, okEHx);

	// Connect ep00wire to computer
	okWireIn wi00 (
		.okHE(okHE),
		.ep_addr(8'h00), 
		.ep_dataout(ep00wire)
	);

	// https://docs.opalkelly.com/display/FPSDK/FrontPanel+HDL+-+USB+2.0#FrontPanelHDL-USB2.0-okBTPipeOut
	// Connects a pipe out. This connects directly to DDR3 output fifo.
	okBTPipeOut po0 (
		.okHE(okHE), 
		.okEH(okEHx[ 0*65 +: 65 ]), 
		.ep_addr(8'ha0), 
		.ep_read(recall_fifo_read),   
		.ep_blockstrobe(), 
		.ep_datain(recall_fifo_dataout),   
		.ep_ready(pipe_out_ready)
	);
// --------------------------------------------------------------------

// Generates clock frequency for DDR3
mem_pll mem_pll_inst (
	.refclk   (phy_clk),         //  refclk.clk
	.rst      (~global_reset_n), //   reset.reset
	.outclk_0 (mem_avl_clk),     // outclk0.clk
	.locked   (mem_pll_locked)   //  locked.export
	);

// Generates clock frequency for ADC
master_pll master_pll_inst (
	.refclk	 (sys_clk),
	.outclk_0 (master_clk)
	);
	
// Convert differential clock input to simple clock
alt_inbuf_diff sys_clk_io(.i(sys_clk_p), .ibar(sys_clk_n), .o(sys_clk));

// Define DDR3 module (IDK how this works)
ddr3_interface ddr3_interface_inst (
	.pll_ref_clk                (sys_clk),             //        pll_ref_clk.clk
	.global_reset_n             (global_reset_n),      //       global_reset.reset_n
	.soft_reset_n               (1'b1),                //         soft_reset.reset_n

	.afi_clk                    (phy_clk),             //            afi_clk.clk
	.afi_half_clk               (),                    //       afi_half_clk.clk
	.afi_reset_n                (),                    //          afi_reset.reset_n
	.afi_reset_export_n         (),                    //   afi_reset_export.reset_n

	.mem_a                      (mem_addr[14 : 0]),    //             memory.mem_a
	.mem_ba                     (mem_ba[2 : 0]),       //                   .mem_ba
	.mem_ck                     (mem_clk),             //                   .mem_ck
	.mem_ck_n                   (mem_clk_n),           //                   .mem_ck_n
	.mem_cke                    (mem_cke),             //                   .mem_cke
	.mem_cs_n                   (mem_cs_n),            //                   .mem_cs_n
	.mem_dm                     (mem_dm[1 : 0]),       //                   .mem_dm
	.mem_ras_n                  (mem_ras_n),           //                   .mem_ras_n
	.mem_cas_n                  (mem_cas_n),           //                   .mem_cas_n
	.mem_we_n                   (mem_we_n),            //                   .mem_we_n
	.mem_reset_n                (mem_reset_n),         //                   .mem_reset_n
	.mem_dq                     (mem_dq),              //                   .mem_dq
	.mem_dqs                    (mem_dqs[1 : 0]),      //                   .mem_dqs
	.mem_dqs_n                  (mem_dqs_n[1 : 0]),    //                   .mem_dqs_n
	.mem_odt                    (mem_odt),             //                   .mem_odt

	.avl_ready_0                (mem_avl_ready),       //              avl_0.waitrequest_n
	.avl_burstbegin_0           (mem_avl_burstbegin),  //                   .beginbursttransfer
	.avl_addr_0                 (mem_avl_addr),        //                   .address
	.avl_rdata_valid_0          (mem_avl_rdata_valid), //                   .readdatavalid
	.avl_rdata_0                (mem_avl_rdata),       //                   .readdata
	.avl_wdata_0                (mem_avl_wdata),       //                   .writedata
	.avl_be_0                   (mem_avl_be),          //                   .byteenable
	.avl_read_req_0             (mem_avl_read_req),    //                   .read
	.avl_write_req_0            (mem_avl_write_req),   //                   .write
	.avl_size_0                 (mem_avl_size),        //                   .burstcount

	.mp_cmd_clk_0_clk           (mem_avl_clk),         //       mp_cmd_clk_0.clk
	.mp_cmd_reset_n_0_reset_n   (1'b1),                //   mp_cmd_reset_n_0.reset_n
	.mp_rfifo_clk_0_clk         (mem_avl_clk),         //     mp_rfifo_clk_0.clk
	.mp_rfifo_reset_n_0_reset_n (1'b1),                // mp_rfifo_reset_n_0.reset_n
	.mp_wfifo_clk_0_clk         (mem_avl_clk),         //     mp_wfifo_clk_0.clk
	.mp_wfifo_reset_n_0_reset_n (1'b1),                // mp_wfifo_reset_n_0.reset_n
	.mp_rfifo_clk_1_clk         (mem_avl_clk),         //     mp_rfifo_clk_1.clk
	.mp_rfifo_reset_n_1_reset_n (1'b1),                // mp_rfifo_reset_n_1.reset_n
	.mp_wfifo_clk_1_clk         (mem_avl_clk),         //     mp_wfifo_clk_1.clk
	.mp_wfifo_reset_n_1_reset_n (1'b1),                // mp_wfifo_reset_n_1.reset_n

	.local_init_done            (mem_init_done),       //             status.local_init_done
	.local_cal_success          (mem_cal_success),     //                   .local_cal_success
	.local_cal_fail             (mem_cal_fail),        //                   .local_cal_fail
	.oct_rzqin                  (mem_rzqin),           //                oct.rzqin

	.pll_mem_clk                (),                    //        pll_sharing.pll_mem_clk
	.pll_write_clk              (),                    //                   .pll_write_clk
	.pll_locked                 (),                    //                   .pll_locked
	.pll_write_clk_pre_phy_clk  (),                    //                   .pll_write_clk_pre_phy_clk
	.pll_addr_cmd_clk           (),                    //                   .pll_addr_cmd_clk
	.pll_avl_clk                (),                    //                   .pll_avl_clk
	.pll_config_clk             (),                    //                   .pll_config_clk
	.pll_mem_phy_clk            (),                    //                   .pll_mem_phy_clk
	.afi_phy_clk                (),                    //                   .afi_phy_clk
	.pll_avl_phy_clk            ()                     //                   .pll_avl_phy_clk
	);

	
// Create DDR3 Interface
ddr3_test ddr3_tb (
	.clk             (mem_avl_clk),
	.reset           (ep00wire[2]),  			// Pass in signals from computer to enable writes and reads
	.writes_en       (ep00wire[1]),
	.reads_en        (ep00wire[0]),
	.calib_done      (mem_init_done),

	.ib_re           (store_fifo_read),			// Request to read data from storage fifo
	.ib_data         (store_fifo_data),			// Data from storage fifo
	.ib_count        (store_fifo_rd_count),	// Available words to read from storage fifo
	.ib_empty        (store_fifo_empty),		// Is storage fifo empty?

	.ob_we           (recall_fifo_write),		// Request to write to recall fifo
	.ob_data         (recall_fifo_datain),		// Data to request fifo
	.ob_count        (recall_fifo_wr_count),	// How much data is available for PC to read

	.avl_ready       (mem_avl_ready),
	.avl_burstbegin  (mem_avl_burstbegin),
	.avl_address     (mem_avl_addr),
	.avl_size        (mem_avl_size),

	.avl_read_req    (mem_avl_read_req),
	.avl_rdata_valid (mem_avl_rdata_valid),
	.avl_rdata       (mem_avl_rdata),

	.avl_write_req   (mem_avl_write_req),
	.avl_wdata       (mem_avl_wdata),
	.avl_be          (mem_avl_be)
);

// Storage fifo
fifo_w128_256 okPipeIn_fifo (
	.aclr      (ep00wire[2]),				// Clear fifo
	.wrclk     (master_clk),				// Clock to synchronize writes to
	.rdclk     (mem_avl_clk),				// Clock to synchronize reads to
	.data      (adc_data),     			// Store adc_data into fifo
	.wrreq     (store_fifo_write),		// When done collecting from ADC, request to store in fifo
	.rdreq     (store_fifo_read),			// DDR3 interface asking for data to save
	.q         (store_fifo_data),       // Output to DDR3 interface
	.wrfull    (store_fifo_full),			// Am I full?
	.rdempty   (store_fifo_empty),		// Am I empty?
	.rdusedw   (store_fifo_rd_count),   // How many readings can you take from me?
	.wrusedw   (store_fifo_wr_count)    // How many data points have been written and remain?
);


// Recall fifo. Note: takes 128 bit words from DDR3 but 
// outputs 32 bit words for pipe to pc
fifo_w128_256_r32_1024 okPipeOut_fifo (
	.aclr      (ep00wire[2]),				// Clear fifo
	.wrclk     (mem_avl_clk),				// Clock to synchronize writes to
	.rdclk     (okClk),						// Clock to synchronize reads to
	.data      (recall_fifo_datain),    // Data to store from DDR3
	.wrreq     (recall_fifo_write),		// Should I save on this clock cycle?
	.rdreq     (recall_fifo_read),		// Is PC asking for data?
	.q         (recall_fifo_dataout),   // Output to PC
	.wrfull    (recall_fifo_full),		// Am I Full?
	.rdempty   (recall_fifo_empty),		// Am I empty?
	.rdusedw   (recall_fifo_rd_count),  // How many readings can you take from me?
	.wrusedw   (recall_fifo_wr_count)   // How many data points have been written and remain?
);

endmodule
