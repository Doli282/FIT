module comp(input [31:0] a, b, output out);
    assign out = (a == b);
endmodule

module comparator (input [31:0] a, b, output out);
    wire [31:0] y;
    assign y = a^b;
    assign out = ~|y;
endmodule