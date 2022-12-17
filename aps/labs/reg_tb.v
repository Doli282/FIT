module test();

	wire [31:0] RD1, RD2;
	reg [31:0] WD3;
	reg [4:0] A1, A2, A3;
	reg CLK, WE3;
	
	register r(A1, A2, A3, WD3, WE3, CLK, RD1, RD2);
	
	initial begin
		$dumpfile("test");
		$dumpvars;
		CLK = 0;
		WE3 = 0;
		WD3 = 10;
		A1 = 0;
		A2 = 0;
		A3 = 0;
		
		//write in memory
		#6;
		WE3 = 1;
		A3 = 10;
		#8;
		WD3 = 12;
		A3 = 12;
		#8;
		WD3 = 8;
		A3 = 16;
		#8;
		WD3 = 16;
		#8;
		// read
		
		A1 = 10;
		#4;
		A2 = 10;
		#4;
		A2 = 12;
		#4;
		A3 = 12;
		#4;
		A2 = 12;

		
		
		#60 $finish;
	end

	always #4 CLK = ~CLK;
	
endmodule
