library ieee;
library clockPLL;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.FRONTPANEL.all;
use ieee.std_logic_unsigned.all;

entity Acoustics is
	port(
	
		okUH      	: in     STD_LOGIC_VECTOR(4 downto 0);
		okHU      	: out    STD_LOGIC_VECTOR(2 downto 0);
		okUHU     	: inout  STD_LOGIC_VECTOR(31 downto 0);
		okAA      	: inout  STD_LOGIC;
		
		sys_clk_n 	: in 	 	 STD_LOGIC;
		sys_clk_p 	: in		 STD_LOGIC;
			
		drdy_in		: in		 STD_LOGIC; --A3
		sclk_out	 	: out	 	 STD_LOGIC; --A1
		miso_pf_in	: in		 STD_LOGIC; --A2
		miso_pa_in	: in		 STD_LOGIC; --BP6
		miso_sf_in	: in		 STD_LOGIC; --BN6
		miso_sa_in	: in		 STD_LOGIC; --BP8
		start		 	: out	 	 STD_LOGIC; --BP1
		clk_out	 	: out 	 STD_LOGIC;	--BP0
		test_out		: out 	 STD_LOGIC;	--BN2
		
		
		led       : out    STD_LOGIC_VECTOR(3 downto 0)
	);
end Acoustics;

architecture arch of Acoustics is

-- Clocks
	signal master_clk				: STD_LOGIC := '0';
	signal sys_clk    			: STD_LOGIC;
	
-- USB
	signal okClk      			: STD_LOGIC;
	signal okHE       			: STD_LOGIC_VECTOR(112 downto 0);
	signal okEH       			: STD_LOGIC_VECTOR(64 downto 0);
	signal okEHx      			: STD_LOGIC_VECTOR(1*65-1 downto 0);
	
-- Pipe out signals
	signal bs_out					: STD_LOGIC := '0';  		
	signal pipe_out_ready		: STD_LOGIC := '1';										-- Allows computer to read when high. When low, data is being collected
	signal pipe_out_read  		: STD_LOGIC;												-- Computer requesting data
	signal pipe_out_data  		: STD_LOGIC_VECTOR(31 downto 0);						-- Data to be sent to computer. 32 bit version of data_to_send
	
-- ADC Interaction
	signal data_pf   				: STD_LOGIC_VECTOR(23 downto 0) := x"000001";  	-- Data buffer
	signal data_pa					: STD_LOGIC_VECTOR(23 downto 0) := x"000001";  	-- Data buffer
	signal data_sf					: STD_LOGIC_VECTOR(23 downto 0) := x"000001";  	-- Data buffer
	signal data_sa					: STD_LOGIC_VECTOR(23 downto 0) := x"000001";  	-- Data buffer
	
-- FIFO
	signal data_out_pf   		: STD_LOGIC_VECTOR(23 downto 0);  	
	signal data_out_pa			: STD_LOGIC_VECTOR(23 downto 0);  	
	signal data_out_sf			: STD_LOGIC_VECTOR(23 downto 0);  	
	signal data_out_sa			: STD_LOGIC_VECTOR(23 downto 0);  	
	signal write_request			: STD_LOGIC := '0';										-- When high, data is written to fifo
	signal full_pf					: STD_LOGIC := '0';										-- Fifo full
	signal full_pa					: STD_LOGIC := '0';										-- Fifo full
	signal full_sf					: STD_LOGIC := '0';										-- Fifo full
	signal full_sa					: STD_LOGIC := '0';										-- Fifo full
	signal empty_pf				: STD_LOGIC := '0';										-- Fifo empty
	signal empty_pa				: STD_LOGIC := '0';										-- Fifo empty
	signal empty_sf				: STD_LOGIC := '0';										-- Fifo empty
	signal empty_sa				: STD_LOGIC := '0';										-- Fifo empty
	signal read_pf					: STD_LOGIC := '0';										-- Fifo read
	signal read_pa					: STD_LOGIC := '0';										-- Fifo read
	signal read_sf					: STD_LOGIC := '0';										-- Fifo read
	signal read_sa					: STD_LOGIC := '0';										-- Fifo read
	
-- LEDs
	signal led_state 				: STD_LOGIC_VECTOR(3 downto 0);
	
	
-- Triggers and Resets
	signal sending_sclk 			: STD_LOGIC := '0';										-- Currently talking to adc
	signal start_sclk 			: STD_LOGIC := '0';
	signal reset_sclk 			: STD_LOGIC := '0';										-- Assert to deassert start_sclk
	signal start_collections	: STD_LOGIC_VECTOR(31 downto 0);	
	
	
	
	
	-- Bipolar input to clock
	component alt_inbuf_diff
	PORT
	(
		i    : IN STD_LOGIC;
		ibar : IN STD_LOGIC;
		o    : OUT STD_LOGIC
	);
	end component;
	
	-- 15MHz pll
	component master_pll
	port (
		refclk   : in  std_logic := '0'; --  refclk.clk
		rst      : in  std_logic := '0'; --   reset.reset
		outclk_0 : out std_logic        -- outclk0.clk
	);
	end component;
	
	-- Storage fifo
	component fifo 
	port (
		data		: IN STD_LOGIC_VECTOR (23 DOWNTO 0);
		rdclk		: IN STD_LOGIC ;
		rdreq		: IN STD_LOGIC ;
		wrclk		: IN STD_LOGIC ;
		wrreq		: IN STD_LOGIC ;
		q		: OUT STD_LOGIC_VECTOR (23 DOWNTO 0);
		rdempty		: OUT STD_LOGIC ;
		wrfull		: OUT STD_LOGIC 
	);
	end component;

begin



		-- Mapping differential inputs to clock
		alt_inbuf_diff_inst: alt_inbuf_diff PORT MAP (
			i    => sys_clk_p,
			ibar => sys_clk_n,
			o    => sys_clk
		);
		
		
		-- Generating 15MHz clock
		pll: master_pll PORT MAP (
			refclk 	=> sys_clk,
			outclk_0 => master_clk
		);
		
		
		
		-- fifo
		fifo_pf: fifo PORT MAP (
			rdclk => okClk,				-- Reads are sunchronous to usb clock
			data => data_pf,				-- Input data is data
			rdreq => read_pf,				-- Output data when computer reading
			wrclk => master_clk,			-- Writes are synchronous to adc clock
			wrreq => write_request,		-- Write when write requested
			q => data_out_pf,			-- Load data into data_to_send
			rdempty => empty_pf,				
			wrfull => full_pf
		);
		
		-- fifo
		fifo_pa: fifo PORT MAP (
			rdclk => okClk,				-- Reads are sunchronous to usb clock
			data => data_pa,				-- Input data is data
			rdreq => read_pa,				-- Output data when computer reading
			wrclk => master_clk,			-- Writes are synchronous to adc clock
			wrreq => write_request,		-- Write when write requested
			q => data_out_pa,			-- Load data into data_to_send
			rdempty => empty_pa,				
			wrfull => full_pa
		);
		
		-- fifo
		fifo_sf: fifo PORT MAP (
			rdclk => okClk,				-- Reads are sunchronous to usb clock
			data => data_sf,				-- Input data is data
			rdreq => read_sf,				-- Output data when computer reading
			wrclk => master_clk,			-- Writes are synchronous to adc clock
			wrreq => write_request,		-- Write when write requested
			q => data_out_sf,			-- Load data into data_to_send
			rdempty => empty_sf,				
			wrfull => full_sf
		);
		
		-- fifoipc
		fifo_sa: fifo PORT MAP (
			rdclk => okClk,				-- Reads are sunchronous to usb clock
			data => data_sa,				-- Input data is data
			rdreq => read_sa,				-- Output data when computer reading
			wrclk => master_clk,			-- Writes are synchronous to adc clock
			wrreq => write_request,		-- Write when write requested
			q => data_out_sa,			-- Load data into data_to_send
			rdempty => empty_sa,				
			wrfull => full_sa
		);
		

		
		
		
		
	-- Start	
		process (master_clk) begin
			if falling_edge(master_clk) then
				if start_sclk = '1' then
					reset_sclk <= '1';
					sending_sclk <= '1';
				elsif sending_sclk = '1' then				-- On falling edge, if transfer is going...
					if data_pf(23) = '1' then					-- If our original 1 has been shifted to the 23rd position, this is our last pulse
						sending_sclk <= '0';					-- Stop clock and load data into fifo
						write_request <= '1';
					end if;
					data_pf <= STD_LOGIC_VECTOR(UNSIGNED(data_pf) sll 1);
					data_pa <= STD_LOGIC_VECTOR(UNSIGNED(data_pa) sll 1);
					data_sf <= STD_LOGIC_VECTOR(UNSIGNED(data_sf) sll 1);
					data_sa <= STD_LOGIC_VECTOR(UNSIGNED(data_sa) sll 1);
					data_pf(0) <= miso_pf_in;						
					data_pa(0) <= miso_pa_in;						
					data_sf(0) <= miso_sf_in;						-- Shift data ad insert new bit
					data_sa(0) <= miso_sa_in;						
				else
					data_pf <= x"000001";						-- If not sending, reset flags and load our 1 into data
					write_request <= '0';
					reset_sclk <= '0';
				end if;
			end if;
		end process;
		
		
		
		process (drdy_in, reset_sclk) begin
			if reset_sclk = '1' then
				start_sclk <= '0';
			elsif falling_edge(drdy_in) then
				if pipe_out_ready = '0' then
					start_sclk <= '1';
				end if;
			end if;
		end process;	
		
		
		process (start_collections(0), full_pf) begin
			if full_pf = '1' then
				pipe_out_ready <= '1';
			elsif rising_edge(start_collections(0)) then
				pipe_out_ready <= '0';
			end if;
		end process;
		
		
		read_pf <= pipe_out_read when empty_pf = '0' else '0';
		read_pa <= pipe_out_read when empty_pf = '1' and empty_pa = '0' else '0';
		read_sf <= pipe_out_read when empty_pa = '1' and empty_sf = '0' else '0';
		read_sa <= pipe_out_read when empty_sf = '1' else '0';
		
		pipe_out_data(23 downto 0) <= data_out_pf when empty_pf = '0' else
												data_out_pa when empty_pa = '0' else
												data_out_sf when empty_sf = '0' else
												data_out_sa;
		
		sclk_out <= master_clk when sending_sclk = '1' else '0';
		
		clk_out <= master_clk;
		start <= not pipe_out_ready;
		test_out <= not pipe_out_ready;
		
		led_state(0) <= '1';
		led_state(1) <= not pipe_out_ready;
		led_state(3) <= drdy_in;
		
		
				
	-- Instantiate the okHost and connect endpoints
		okHI : okHost port map (
			okUH=>okUH, 
			okHU=>okHU, 
			okUHU=>okUHU, 
			okAA=>okAA,
			okClk=>okClk, 
			okHE=>okHE, 
			okEH=>okEH
		);
		
		okWO : okWireOR     generic map (N=>1) port map (okEH=>okEH, okEHx=>okEHx);

		trigger_in : okTriggerIn   port map (okHE=>okHE, ep_addr=>x"40", ep_clk => okclk, ep_trigger => start_collections);
		
		pipe_out : okBTPipeOut    	port map (okHE=>okHE, okEH=>okEHx( 1*65-1 downto 0*65 ), ep_addr=>x"A0", 
										ep_read=>pipe_out_read, ep_datain=>pipe_out_data, ep_ready=>pipe_out_ready, ep_blockstrobe=>bs_out);

		
		led(0) <= 'Z' when (led_state(0)='0') else '0';
		led(1) <= 'Z' when (led_state(1)='0') else '0';
		led(2) <= 'Z' when (led_state(2)='0') else '0';
		led(3) <= 'Z' when (led_state(3)='0') else '0';
		
		
		
		
		
end arch;
