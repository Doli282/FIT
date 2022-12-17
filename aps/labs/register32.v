module register32 (input [31:0] datain, input clk, output reg[31:0] dataout);
    always@(posedge clk) begin
        dataout <= datain;
    end
endmodule