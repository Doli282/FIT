module test();

    reg [31:0] a, b;
    reg in;
    wire [31:0] sum;
    wire out;

    //full_adder add(a,b,in,sum,out);
    adder_32 adder(a, b, in, sum, out);


    initial begin
        $dumpfile("test");
        $dumpvars;
        a = 0;
        b = 0;
        in = 0;
        #100000 $finish;
    end

    always #4 a = a+1;
    always #8 b = b+1;
    always #2 in = ~in;

    always@(in) #2 $display("t=%d, a=%d, b=%d, in=%d, sum=%d, out=%d", $time, a,b,in,sum,out);

endmodule