`default_nettype none
module processor( input         clk, reset,
                  output reg [31:0] PC,
                  input  [31:0] instruction,
                  output reg       WE,
                  output reg [31:0] address_to_mem,
                  output reg [31:0] data_to_mem,
                  input  [31:0] data_from_mem
                );
    //... write your code here ...
	
  reg signed [31:0] r [31:0]; // 32 bit registers (32 of them)
  integer i;
  // Control Unit
  always@(posedge clk) begin
    $display(" ");
    //$display("===== CPU ha isogashi desu =======");
    $display("PC before instruction = %h", PC);
    //$display("Instruction = %b", instruction);
    WE = 0;
    r[0] = 0;
    if(reset) begin
      PC = 0;
      //$display("reset = %b, PC = %h", reset, PC);
    end
    else begin
    //$display("Instruction[6:0] = %b", instruction[6:0]);
      case(instruction[6:0])
        'b0110011: // add, and, sub, sll, srl, sra
          begin
            //$display("Instruction[14:12] = %b, rd = %d, rs1 = %d, rs2 = %d", instruction[14:12],  instruction[11:7], instruction[19:15], instruction[24:20]);
            //$display("BEFORE: rd = %d, rs1 = %d, rs2 = %d", r[instruction[11:7]], r[instruction[19:15]], r[instruction[24:20]]);
          case(instruction[14:12])
            'b000: // add, sub
              case(instruction[31:25])
                'b0000000: // add: rd = rs1 + rs2
                  r[instruction[11:7]] = r[instruction[19:15]] + r[instruction[24:20]];
                'b0100000: // sub: rd = rs1 - rs2
                  r[instruction[11:7]] = r[instruction[19:15]] - r[instruction[24:20]];
              endcase
            'b111: // and
              r[instruction[11:7]] = r[instruction[19:15]] & r[instruction[24:20]];
            'b010: // slt: rd = (rs1 < rs2)
              r[instruction[11:7]] = (r[instruction[19:15]] < r[instruction[24:20]]);
            'b100: // div: rd = rs1 / rs2
              r[instruction[11:7]] = r[instruction[19:15]] / r[instruction[24:20]];
            'b110: // rem: rd = rs1 % rs2
              r[instruction[11:7]] = r[instruction[19:15]] % r[instruction[24:20]];
            'b001: // sll: rd = rs1 << rs2
              r[instruction[11:7]] = r[instruction[19:15]] << r[instruction[24:20]];
            'b101: // srl, sra
              case(instruction[31:25])
                'b0000000: // srl: rd = (unsigned)rs1 >> rs2 (logical) 
                  r[instruction[11:7]] = r[instruction[19:15]] >> r[instruction[24:20]];
                'b0100000: // sra: rd = (signed)rs1 >>rs2 (arithmetic)
                  begin
            //        $display("SRA: r%d = %d >>> %d", instruction[19:15], r[instruction[19:15]], r[instruction[24:20]]);
                    r[instruction[11:7]] = (r[instruction[19:15]] >>> r[instruction[24:20]]);
                  end
              endcase
          endcase
            //$display("AFTER: rd = %d, rs1 = %d, rs2 = %d", r[instruction[11:7]], r[instruction[19:15]], r[instruction[24:20]]);
          end
        'b0010011: // addi: rd = rd1 + imm12
          begin
            //$display("rd = %d, rs1 = %d, imm = %d",  instruction[11:7], instruction[19:15], instruction[31:20]);
            //$display("BEFORE: rd = %d, rs1 = %d", r[instruction[11:7]], r[instruction[19:15]]);
          r[instruction[11:7]] = r[instruction[19:15]] + {{20{instruction[31]}}, instruction[31:20]};
            //$display("AFTER: rd = %d, rs1 = %d", r[instruction[11:7]], r[instruction[19:15]]);
          end
        'b1100011: // beq, blt
          begin
            //$display("Instruction[14:12] = %b, rs%d = %d, rs%d = %d", instruction[14:12], instruction[19:15], r[instruction[19:15]], instruction[24:20], r[instruction[24:20]]);
          case(instruction[14:12])
            'b000: // beq: if [rs1] == [rs2] go to [PC]+{imm12:1,'0'}; else go to [PC]+4;	
              if(r[instruction[19:15]] == r[instruction[24:20]]) PC = PC + {{19{instruction[31]}}, instruction[31], instruction[7], instruction[30:25], instruction[11:8], 1'b0} - 4;
              //else PC = PC + 4;
            'b001: //bne: if rs1 != rs2 go to PC+imm*2; else go to PC+4
              if(r[instruction[19:15]] != r[instruction[24:20]]) PC = PC + {{19{instruction[31]}},instruction[31], instruction[7], instruction[30:25], instruction[11:8], 1'b0} - 4;
              //else PC = PC + 4;
            'b100: //blt: if [rs1] < [rs2] go to [PC]+{imm12:1,'0'}; else go to [PC]+4;	
              if(r[instruction[19:15]] < r[instruction[24:20]]) PC = PC + {{19{instruction[31]}}, instruction[31], instruction[7], instruction[30:25], instruction[11:8], 1'b0} - 4;
              //else PC = PC + 4;
          endcase
            //$display("PC = %d", PC);
          end
        'b0000011: // lw: rd ← Memory[[rs1] + imm11:0]
          begin
            $display("lw: r%d <- mem[r%d[%d] + imm[%d]",  instruction[11:7], instruction[19:15], r[instruction[19:15]], {{20{instruction[31]}}, instruction[31:20]});
            address_to_mem = r[instruction[19:15]] + {{20{instruction[31]}}, instruction[31:20]};
            r[0] = 0;
            r[instruction[11:7]] = data_from_mem;
            $display("lw: r%d <- %d",  instruction[11:7], data_from_mem);
          end
        'b0100011: // sw: Memory[[rs1] + imm11:0] ← [rs2];
          begin
            WE = 1;
            address_to_mem = r[instruction[19:15]] + {{20{instruction[31]}}, {instruction[31:25], instruction[11:7]}};
            data_to_mem = r[instruction[24:20]];
            $display("sw: WE = %d, addr = %d, data = %d", WE, address_to_mem, data_to_mem);
            $display("sw: r%d, %d(r%d)", instruction[24:20], {{20{instruction[31]}}, {instruction[31:25], instruction[11:7]}}, instruction[19:15]);
          end
        'b0110111: // lui: rd ← {imm31:12,'0000 0000 0000'};
          begin
            //$display("lui ");
          r[instruction[11:7]] = {instruction[31:12], 12'b0000_0000_0000};
          //$display("lui: ");
          end
        'b1101111: // jal: rd ← [PC]+4; go to [PC] +{imm20:1,'0'};
          begin
            //$display("jal r%d, label", instruction[11:7]);
            r[instruction[11:7]] = PC + 4;
            PC = PC + {{11{instruction[31]}}, instruction[31], instruction[19:12], instruction[20], instruction[30:21], 1'b0} - 4;
            //$display("jal: PC = %d", PC);
          end
        'b1100111: // jalr: rd ← [PC]+4; go to [rs1]+imm11:0;
          begin
            //$display("jalr r%d, label", instruction[11:7]);
            r[instruction[11:7]] = PC + 4;
            PC = r[instruction[19:15]] + {{20{instruction[31]}}, instruction[31:20]} - 4;
            //$display("jalr: PC = %d", PC);
          end
        'b0010111: // auipc: rd ← [PC] + {imm31:12,'0000 0000 0000'};
          begin
            //$display("auipc: r%d = %d", instruction[11:7], PC + {instruction[31:12], 12'b0000_0000_0000});
          r[instruction[11:7]] = PC + {instruction[31:12], 12'b0000_0000_0000};
          end
      endcase
      r[0] = 0; // verify that r0 is zero
      // increment PC
      PC = PC + 4;
    end
    for(i = 0; i < 32; i= i+1)
      $display("r[%d] = %d", i, r[i]);
    $display("PC after instruction = %h", PC);
  end
endmodule

//... add new modules here ...

`default_nettype wire