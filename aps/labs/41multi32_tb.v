module test();

    reg [31:0] a, b, c, d;
    reg [1:0] s;
    wire [31:0] y;

    mux_4_1_32 mux(a,b,c,d,s,y);

    initial begin
        $dumpfile("test");
        $dumpvars;
        a = 0;
        b = 2; 
        c = 4;
        d = 8;
        s = 0;
        #640 $finish;
    end

    always #10 s[0] = ~s[0];
    always #20 s[1] = ~s[1];
    always #40 a = ~a;
    always #80 b = ~b;
    always #160 c = ~c;
    always #320 d = ~d;

    always@(s[0]) #2 $display("time=%d, a=%d, b=%d, c=%d, d=%d, s=%b, y=%d", $time, a,b,c,d,s,y);

endmodule