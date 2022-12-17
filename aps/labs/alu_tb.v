module test();
	
	reg [31:0] SrcA, SrcB;
	reg [1:0] ALUControl;
	wire [31:0] ALUResult;
	wire Zero;
	
	alu al1(SrcA, SrcB, ALUControl, Zero, ALUResult);
	
	initial begin
		$dumpfile("test");
		$dumpvars;
// ADD
		$display("ADD:");
		ALUControl = 2'b00;
		SrcA = 32'h0000_3333;
		SrcB = 32'h0000_1111;
		#2;
		$display("a=%h, b=%h, Con=%b, Res=%h, Zero=%h", SrcA, SrcB, ALUControl, ALUResult, Zero);
		#2;
		SrcA = 32'hFFFF_FFFF;
		SrcB = 32'h0000_1111;
		#2;
		$display("a=%h, b=%h, Con=%b, Res=%h, Zero=%h", SrcA, SrcB, ALUControl, ALUResult, Zero);
		#2;
		SrcA = 32'h0000_0000;
		SrcB = 32'h0000_0000;
		#2;
		$display("a=%h, b=%h, Con=%b, Res=%h, Zero=%h", SrcA, SrcB, ALUControl, ALUResult, Zero);
		#2;
		SrcA = 32'hFFFF_FFFF;
		SrcB = 32'h0000_0001;
		#2;
		$display("a=%h, b=%h, Con=%b, Res=%h, Zero=%h", SrcA, SrcB, ALUControl, ALUResult, Zero);
		#2;
// SUB
		$display("SUB:");
		ALUControl = 2'b01;
		SrcA = 32'h0000_1111;
		SrcB = 32'h0010_1111;
		#2;
		$display("a=%h, b=%h, Con=%b, Res=%h, Zero=%h", SrcA, SrcB, ALUControl, ALUResult, Zero);
		#2;
		SrcA = 32'hFFFF_FFFF;
		SrcB = 32'h0000_1111;
		#2;
		$display("a=%h, b=%h, Con=%b, Res=%h, Zero=%h", SrcA, SrcB, ALUControl, ALUResult, Zero);
		#2;
		SrcA = 32'h0000_0010;
		SrcB = 32'h0000_0010;
		#2;
		$display("a=%h, b=%h, Con=%b, Res=%h, Zero=%h", SrcA, SrcB, ALUControl, ALUResult, Zero);
		#2;
// XOR		
		$display("XOR:");
		ALUControl = 2'b10;
		SrcA = 32'h0000_3333;
		SrcB = 32'h0000_1111;
		#2;
		$display("a=%h, b=%h, Con=%b, Res=%h, Zero=%h", SrcA, SrcB, ALUControl, ALUResult, Zero);
		#2;
		SrcA = 32'h1111_1111;
		SrcB = 32'h0000_1111;
		#2;
		$display("a=%h, b=%h, Con=%b, Res=%h, Zero=%h", SrcA, SrcB, ALUControl, ALUResult, Zero);
		#2;
		SrcA = 32'h0000_0000;
		SrcB = 32'h0000_0000;
		#2;
		$display("a=%h, b=%h, Con=%b, Res=%h, Zero=%h", SrcA, SrcB, ALUControl, ALUResult, Zero);
		#2;
		SrcA = 32'h010F_0000;
		SrcB = 32'h010F_0000;
		#2;
		$display("a=%h, b=%h, Con=%b, Res=%h, Zero=%h", SrcA, SrcB, ALUControl, ALUResult, Zero);
		#2;
		
		$finish;
	end



endmodule
