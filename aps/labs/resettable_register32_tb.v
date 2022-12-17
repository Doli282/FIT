module test();

    reg [31:0] in;
    wire [31:0] out;
    reg clk, reset;

    register32 register(in, clk, reset, out);

    initial begin
        $dumpfile("test");
        $dumpvars;
        in = 1;
        clk = 1;
        reset = 0;
        #2;
        $display("in=%b, clk=%b, reset=%b, out=%b", in, clk, reset, out);
        #2;
        in = 2;
        clk = 0;
        reset = 0;
        #2;
        $display("in=%b, clk=%b, reset=%b, out=%b", in, clk, reset, out);
        #2;
        in = 4;
        clk = 1;
        reset = 1;
        #2;
        $display("in=%b, clk=%b, reset=%b, out=%b", in, clk, reset, out);
        #2;
        in = 1;
        clk = 0;
        reset = 0;
        #2;
        $display("in=%b, clk=%b, reset=%b, out=%b", in, clk, reset, out);
        #2;
        in = 1;
        clk = 1;
        reset = 0;
        #2;
        $display("in=%b, clk=%b, reset=%b, out=%b", in, clk, reset, out);
        #2;
        in = 8;
        clk = 0;
        reset = 1;
        #2;
        $display("in=%b, clk=%b, reset=%b, out=%b", in, clk, reset, out);
        $finish;
    end
endmodule