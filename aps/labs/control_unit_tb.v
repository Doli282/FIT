module test();

    reg [2:0] Opcode;
    wire [1:0] ALUControl;
    wire ALUSrc, RegWrite, MemWrite, Branch;

    controlUnit CU(Opcode, ALUSrc, RegWrite, ALUControl, MemWrite, Branch);

    initial begin
        $dumpfile("test");
        $dumpvars;
        Opcode = -1;
        #30 $finish;
    end

    always begin
        #2 Opcode += 1;
        #2 $display("Opcode=%b, ALUSrc=%b, RegWrite=%b, ALUControl=%b, MemWrite=%b, Branch=%b", Opcode, ALUSrc, RegWrite, ALUControl, MemWrite, Branch);
    end

endmodule