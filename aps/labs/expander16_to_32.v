module expander (input [15:0] in, output [31:0] out);
    genvar i;
    for (i=0; i < 16; i=i+1) begin
        assign out[i] = in[i];
    end
    for (i=16; i < 32; i=i+1) begin
        assign out[i] = in[15];
    end
endmodule