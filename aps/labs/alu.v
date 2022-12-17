module alu(input [31:0] SrcA, SrcB, input [1:0] ALUControl,
	 output reg Zero, output reg [31:0] ALUResult);
	reg [32:0] c;
	always@(*) begin
		Zero = 0;
		case (ALUControl)
			0: begin 
				ALUResult = SrcA + SrcB;
				//c = SrcA + SrcB;
				//ALUResult = c[31:0];
				//Zero = c[32];
			   end
			1: ALUResult = SrcA - SrcB;
			2: begin 
				ALUResult = SrcA ^ SrcB;
				Zero = 0;
			   end
		endcase
		if(ALUResult == 0) Zero = 1;
	end
	
endmodule
