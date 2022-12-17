module register32 (input [31:0] datain, input clk, reset, output reg[31:0] dataout);
    always@(posedge clk) begin
        if(reset) dataout <= 0;
        else dataout <= datain;
    end
endmodule