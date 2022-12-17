module test();

    reg [31:0] a,b;
    wire out;

//    comp cmp(a,b,y);
    comparator compar(a,b,out);


    initial begin
        $dumpfile("test");
        $dumpvars;
        a = 32;
        b = 34;
        #2;
        $display("a=%d, b=%d, out=%d", a,b,out);
        #2;
        b = 32; 
        #2;
        $display("a=%d, b=%d, out=%d", a,b,out);
        #2;
        b = 0;
        #2;
        $display("a=%d, b=%d, out=%d", a,b,out);
        #2;
        $finish;
    end

endmodule