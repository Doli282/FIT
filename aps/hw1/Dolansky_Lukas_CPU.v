`default_nettype none
module processor( input         clk, reset,
                  output reg [31:0] PC,
                  input  [31:0] instruction,
                  output        WE,
                  output reg [31:0] address_to_mem,
                  output reg [31:0] data_to_mem,
                  input  [31:0] data_from_mem
                );
    //... write your code here ...
	
  reg[31:0] r [4:0]; // 32 bit registers (32 of them)

  // Control Unit
  always@(PC) begin
    if(reset) 
      PC <= 0;
    else begin
      case(instruction[6:0])
        'b0110011: // add, and, sub, sll, srl, sra
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
                  r[instruction[11:7]] = r[instruction[19:15]] >>> r[instruction[24:20]];
              endcase
          endcase
        'b0010011: // addi: rd = rd1 + imm12
           r[instruction[11:7]] = r[instruction[19:15]] + {{20{instruction[31]}}, instruction[31:20]};
        'b1100011: // beq, blt
          case(instruction[14:12])
            'b000: // beq: if [rs1] == [rs2] go to [PC]+{imm12:1,'0'}; else go to [PC]+4;	
              if(r[instruction[19:15]] == r[instruction[24:20]]) PC <= PC + {instruction[31], instruction[7], instruction[30:25], instruction[11:8], 1'b0};
              else PC <= PC + 4;
            'b100: //blt: if [rs1] < [rs2] go to [PC]+{imm12:1,'0'}; else go to [PC]+4;	
              if(r[instruction[19:15]] < r[instruction[24:20]]) PC <= PC + {instruction[31], instruction[7], instruction[30:25], instruction[11:8], 1'b0};
              else PC <= PC + 4;
          endcase
        'b0000011: // lw: rd ← Memory[[rs1] + imm11:0]
          r[instruction[11:7]] = data_from_mem[r[instruction[19:15]] + {{20{instruction[31]}}, instruction[31:20]}];
        'b0100011: // sw: Memory[[rs1] + imm11:0] ← [rs2];
          begin
            address_to_mem <= r[instruction[19:15]] + {{20{instruction[31]}}, {instruction[31:20], instruction[11:7]}};
            data_to_mem <= r[instruction[24:20]];
          end
        'b0110111: // lui: rd ← {imm31:12,'0000 0000 0000'};
          r[instruction[11:7]] = {instruction[31:12], 12'b0000_0000_0000};
        'b1101111: // jal: rd ← [PC]+4; go to [PC] +{imm20:1,'0'};
          begin
            r[instruction[11:7]] = PC + 4;
            PC <= PC + {{11{instruction[31]}}, instruction[31], instruction[19:12], instruction[20], instruction[30:21], 1'b0};
          end
        'b1100111: // jarl: rd ← [PC]+4; go to [rs1]+imm11:0;
          begin
            r[instruction[11:7]] = PC + 4;
            PC <= r[instruction[19:15]] + {{20{instruction[31]}}, instruction[31:20]};
          end
        'b0010111: // auipc: rd ← [PC] + {imm31:12,'0000 0000 0000'};
          r[instruction[11:7]] = PC + {instruction[31:12], 12'b0000_0000_0000}; 
      endcase
      // increment PC
      PC <= PC + 4;
    end
  end
endmodule

//... add new modules here ...

`default_nettype wire