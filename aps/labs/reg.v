module register(input [4:0] A1, A2, A3, input [31:0] WD3, input WE3, CLK,
				output reg [31:0] RD1, RD2);
	
	// saved data - registers
	reg [31:0] data [31:0];
	
	// read from registers
	always@(A1) begin
		if(A1 == 0) RD1 <= 0;
		else RD1 <= data[A1];
	end
	always@(A2) begin
		if(A2 == 0) RD2 <= 0;
		else RD2 <= data[A2];
	end
	
	// write to registers
	always@(posedge CLK) begin
		if(WE3) begin
			data[A3] <= WD3;
		end
	end

endmodule
