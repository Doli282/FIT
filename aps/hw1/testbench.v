module testbench();
	reg         clk;
	reg         reset;
	wire [31:0] data_to_mem, address_to_mem;
	wire        write_enable;

	top simulated_system (clk, reset, data_to_mem, address_to_mem, write_enable);

	integer i;
	initial	begin
		$dumpfile("test");
		$dumpvars;
		reset<=1; # 2; reset<=0;
		#2000;
		$writememh ("memfile_data_after_simulation.hex",simulated_system.dmem.RAM,0,63);
		$display("succesful test: %d", simulated_system.CPU.r[8]);
		$display("success / fail: %d", simulated_system.CPU.r[9]);
		$finish;
	end

	// generate clock
	always	begin
		clk<=1; # 1; clk<=0; # 1;
	end
endmodule

// TEST MODULES BELOW
module top (	input         clk, reset,
		output [31:0] data_to_mem, address_to_mem,
		output        write_enable);

	wire [31:0] pc, instruction, data_from_mem;

//	inst_mem  imem(pc[7:2], instruction);
	inst_mem  imem(pc[8:2], instruction);
	data_mem  dmem(clk, write_enable, address_to_mem, data_to_mem, data_from_mem);
	always@(posedge clk)
	begin
	end
	processor CPU(clk, reset, pc, instruction, write_enable, address_to_mem, data_to_mem, data_from_mem);
endmodule

//-------------------------------------------------------------------
module data_mem (input clk, we,
		 input  [31:0] address, wd,
		 output [31:0] rd);

//	reg [31:0] RAM[63:0];
	reg [31:0] RAM[127:0];

	initial begin
		$readmemh ("memfile_data.hex",RAM,0,127);
	end

	assign rd=RAM[address[31:2]]; // word aligned

	always @ (posedge clk)
		if (we)
			RAM[address[31:2]]<=wd;
endmodule

//-------------------------------------------------------------------
//module inst_mem (input  [5:0]  address,
module inst_mem (input  [6:0]  address,
		 output [31:0] rd);

//	reg [31:0] RAM[63:0];
	reg [31:0] RAM[127:0];
	initial begin
		$readmemh ("Dolansky_Lukas_prog1.hex",RAM,0,127);
//		$readmemh ("memfile_inst.hex",RAM,0,127);
	end
	assign rd=RAM[address]; // word aligned
endmodule