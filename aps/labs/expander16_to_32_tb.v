module test();

    reg [15:0] a;
    wire [31:0] b;

    expander exp(a,b);

    initial begin
        $dumpfile("test");
        $dumpvars;
        a = 30000;
        #2;
        $display("a=%b, b=%b", a,b);
        #2;
        a = 60000;
        #2;
        $display("a=%b, b=%b", a,b);
        #2;
        $finish;
    end

endmodule