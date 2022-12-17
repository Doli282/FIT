module mux_2_1_32 (input [31:0] d0, d1, input select, output [31:0] y);
    assign y = select ? d1 : d0;
endmodule