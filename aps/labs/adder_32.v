module full_adder (input a, b, cin, output sum, cout);
    assign sum = a^b^cin;
    assign cout = a&b | a&cin | b&cin;
endmodule
/* a|b|c||s|o
   ----------
   0|0|0||0|0
   0|0|1||1|0
   0|1|0||1|0
   0|1|1||0|1
   1|0|0||1|0
   1|0|1||0|1
   1|1|0||0|1
   1|1|1||1|1
*/

module adder_32 (input [31:0] a, b , input c0, output [31:0] sum, output c32);
    wire [32:0] c;
    assign c[0] = c0;
    assign c32 = c[32];
    genvar i;
    for (i=0; i<32; i=i+1) begin
      full_adder adder(a[i], b[i], c[i], sum[i], c[i+1]);
    end
endmodule