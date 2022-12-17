module controlUnit(input [2:0] Opcode, output reg ALUSrc, RegWrite, output reg [1:0] ALUControl, output reg MemWrite, Branch);
    always@(*) begin
        case(Opcode)
            0: // add
                begin
                ALUSrc <= 0;
                RegWrite <= 1;
                ALUControl <= 2'b00;
                MemWrite <= 0;
                Branch <= 0;
                end
            1: // addi
                begin
                ALUSrc <= 1;
                RegWrite <= 1;
                ALUControl <= 2'b00;
                MemWrite <= 0;
                Branch <= 0;
                end
            2: // sub
                begin
                ALUSrc <= 0;
                RegWrite <= 1;
                ALUControl <= 2'b01;
                MemWrite <= 0;
                Branch <= 0;
                end
            3: // lw
                begin
                ALUSrc <= 1;
                RegWrite <= 1;
                ALUControl <= 2'b00;
                MemWrite <= 0;
                Branch <= 0;
                end
            4: // sw
                begin
                ALUSrc <= 1;
                RegWrite <= 0;
                ALUControl <= 2'b00;
                MemWrite <= 1;
                Branch <= 0;
                end
            5: // beq
                begin
                ALUSrc <= 0;
                RegWrite <= 0;
                ALUControl <= 2'b01;
                MemWrite <= 0;
                Branch <= 1;
                end
        endcase
    end
endmodule