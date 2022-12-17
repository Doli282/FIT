module test();
    reg [31:0] a, b;
    reg select;
    wire [31:0] y;

    mux_2_1_32 mux(a, b, select, y);

    initial begin
        $dumpfile("test");
        $dumpvars;
        a = 0;
        b = 2;
        select = 0;
        #80 $finish;
    end

    always #10 select = ~select;
    always #20 a = ~a;
    always #40 b = ~b;

    always@(y) #1 $display("Time=%d, a=%b, b=%b, selest=%b, y=%b", $time, a,b,select,y);

endmodule