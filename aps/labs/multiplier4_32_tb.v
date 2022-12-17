module test();

    reg[31:0] a;
    wire[31:0] b;

    multiplier mult(a,b);

    initial begin
        $dumpfile("test");
        $dumpvars;
        a = 4;
        #2;
        $display("a=%b, b=%b",a,b);
        #2;
        a = ~a;
        #2;
        $display("a=%b, b=%b",a,b);
        #2;
        $finish;
    end

endmodule