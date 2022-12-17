module test();

    reg [31:0] in;
    wire [31:0] out;
    reg clk;

    register32 regs(in, clk, out);

    initial begin
        $dumpfile("test");
        $dumpvars;
        in = 28;
        clk = 1;
        #2;
        $display("in=%b, clk=%b out=%b", in, clk, out);
        #2;
        in = 2;
        clk = 0;
        #2;
        $display("in=%b, clk=%b out=%b", in, clk, out);
        #2;
        in = 91;
        clk = 1;
        #2;
        $display("in=%b, clk=%b out=%b", in, clk, out);
        $finish;
    end

endmodule