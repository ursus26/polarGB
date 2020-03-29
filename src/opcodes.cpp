/**
 * Copyright (C) 2018 Bart de Haan
 *
 * polarGB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * polarGB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <fmt/format.h>
#include "polarGB/cpu.h"
#include "polarGB/log.h"


void Cpu::decodeOpcode(instruction_t *instr, u8 opcode)
{
    /* Initialising the instruction struct variables. */
    instr->instructionLength = 0;
    strcpy(instr->mnemonic, "UNSUPPORTED");
    instr->operandSrc.type = OP_NONE;
    instr->operandSrc.reg = RegID_NONE;
    instr->operandSrc.memPtr = RegID_NONE;
    instr->operandSrc.immediate = 0;
    instr->operandDst.type = OP_NONE;
    instr->operandDst.reg = RegID_NONE;
    instr->operandDst.memPtr = RegID_NONE;
    instr->operandDst.immediate = 0;
    instr->cycleCost = 0;
    instr->extraInfo = 0;
    instr->executionFunction = nullptr;


    switch(opcode)
    {
        case 0x0: /* NOP */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeNOP;
            strcpy(instr->mnemonic, "NOP");
            break;
        case 0x1: /* LD BC, d16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_BC;
            instr->cycleCost = 3;
            instr->executionFunction = &Cpu::executeLD16;
            strcpy(instr->mnemonic, "LD BC, d16");
            break;
        case 0x2: /* LD (BC), A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_BC;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (BC), A");
            break;
        case 0x3: /* INC BC */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_BC;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeINC16;
            strcpy(instr->mnemonic, "INC BC");
            break;
        case 0x4: /* INC B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeINC8;
            strcpy(instr->mnemonic, "INC B");
            break;
        case 0x5: /* DEC B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeDEC8;
            strcpy(instr->mnemonic, "DEC B");
            break;
        case 0x6: /* LD B, d8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD B, d8");
            break;
        // case 0x7: /* RLCA */
        //     Log::printVerbose("RLCA");
        //     executeRLCA();
        //     cycles = 1;
        //     break;
        // case 0x8: /* LD a16, SP */
        //     Log::printVerbose("LD (a16), SP");
        //     src16 = reg.getStackPointer();
        //     dest = fetchNext16Bits();
        //     mmu->write(dest, (u8) (src16 & 0xff));
        //     dest++;
        //     mmu->write(dest, (u8) (src16 >> 8));
        //     cycles = 5;
        //     break;
        // case 0x9: /* ADD HL, BC */
        //     Log::printVerbose("ADD HL, BC");
        //     src16 = reg.read16(RegID_BC);
        //     executeADD16(RegID_HL, src16);
        //     cycles = 2;
        //     break;
        case 0xA: /* LD A, (BC) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_BC;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, (BC)");
            break;
        case 0xB: /* DEC BC */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_BC;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeDEC16;
            strcpy(instr->mnemonic, "DEC BC");
            break;
        case 0xC: /* INC C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeINC8;
            strcpy(instr->mnemonic, "INC C");
            break;
        case 0xD: /* DEC C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeDEC8;
            strcpy(instr->mnemonic, "DEC C");
            break;
        case 0xE: /* LD C, d8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD C, d8");
            break;
        // case 0xF: /* RRCA */
        //     Log::printVerbose("RRCA");
        //     executeRRCA();
        //     cycles = 1;
        //     break;
        // case 0x10:
            // break;
        case 0x11: /* LD DE, d16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_DE;
            instr->cycleCost = 3;
            instr->executionFunction = &Cpu::executeLD16;
            strcpy(instr->mnemonic, "LD DE, d16");
            break;
        case 0x12: /* LD (DE), A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_DE;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (DE), A");
            break;
        case 0x13: /* INC DE */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_DE;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeINC16;
            strcpy(instr->mnemonic, "INC DE");
            break;
        case 0x14: /* INC D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeINC8;
            strcpy(instr->mnemonic, "INC D");
            break;
        case 0x15: /* DEC D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeDEC8;
            strcpy(instr->mnemonic, "DEC D");
            break;
        case 0x16: /* LD D, d8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD D, d8");
            break;
        // case 0x17: /* RLA */
        //     Log::printVerbose("RLA");
        //     executeRLA();
        //     cycles = 1;
        //     break;
        case 0x18: /* JR r8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_PC;
            instr->cycleCost = 3;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeJR;
            strcpy(instr->mnemonic, "JR r8");
            break;
        case 0x19: /* ADD HL, DE */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_DE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeADD16;
            strcpy(instr->mnemonic, "ADD HL, DE");
            break;
        case 0x1A: /* LD A, (DE) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_DE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, (DE)");
            break;
        case 0x1B: /* DEC DE */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_DE;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeDEC16;
            strcpy(instr->mnemonic, "DEC DE");
            break;
        case 0x1C: /* INC E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeINC8;
            strcpy(instr->mnemonic, "INC E");
            break;
        case 0x1D: /* DEC E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeDEC8;
            strcpy(instr->mnemonic, "DEC E");
            break;
        case 0x1E: /* LD E, d8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD E, d8");
            break;
        // case 0x1F: /* RRA */
        //     Log::printVerbose("RRA");
        //     executeRRA();
        //     cycles = 1;
        //     break;
        case 0x20: /* JR NZ, r8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_PC;
            instr->cycleCost = 3;
            instr->extraInfo = COND_NZ;
            instr->executionFunction = &Cpu::executeJR;
            strcpy(instr->mnemonic, "JR NZ, r8");
            break;
        case 0x21: /* LD HL, d16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_HL;
            instr->cycleCost = 3;
            instr->executionFunction = &Cpu::executeLD16;
            strcpy(instr->mnemonic, "LD HL, d16");
            break;
        case 0x22: /* LD (HL+), A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8Inc;
            strcpy(instr->mnemonic, "LD (HLI), A");
            break;
        case 0x23: /* INC HL */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeINC16;
            strcpy(instr->mnemonic, "INC HL");
            break;
        case 0x24: /* INC H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeINC8;
            strcpy(instr->mnemonic, "INC H");
            break;
        case 0x25: /* DEC H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeDEC8;
            strcpy(instr->mnemonic, "DEC H");
            break;
        case 0x26: /* LD H, d8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD H, d8");
            break;
        // case 0x27:
        //     break;
        case 0x28: /* JR Z, r8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_PC;
            instr->cycleCost = 3;
            instr->extraInfo = COND_Z;
            instr->executionFunction = &Cpu::executeJR;
            strcpy(instr->mnemonic, "JR Z, r8");
            break;
        // case 0x29: /* ADD HL, HL */
        //     Log::printVerbose("ADD HL, HL");
        //     src16 = reg.read16(RegID_HL);
        //     executeADD16(RegID_HL, src16);
        //     cycles = 2;
        //     break;
        case 0x2A: /* LD A, (HL+) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8Inc;
            strcpy(instr->mnemonic, "LD A, (HLI)");
            break;
        case 0x2B: /* DEC HL */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeDEC16;
            strcpy(instr->mnemonic, "DEC HL");
            break;
        case 0x2C: /* INC L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeINC8;
            strcpy(instr->mnemonic, "INC L");
            break;
        case 0x2D: /* DEC L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeDEC8;
            strcpy(instr->mnemonic, "DEC L");
            break;
        case 0x2E: /* LD L, d8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD L, d8");
            break;
        case 0x2F: /* CPL */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeCPL;
            strcpy(instr->mnemonic, "CPL");
            break;
        case 0x30: /* JR NC, r8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_PC;
            instr->cycleCost = 3;
            instr->extraInfo = COND_NC;
            instr->executionFunction = &Cpu::executeJR;
            strcpy(instr->mnemonic, "JR NC, r8");
            break;
        case 0x31: /* LD SP, d16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_SP;
            instr->cycleCost = 3;
            instr->executionFunction = &Cpu::executeLD16;
            strcpy(instr->mnemonic, "LD SP, d16");
            break;
        case 0x32: /* LD (HL-), A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8Dec;
            strcpy(instr->mnemonic, "LD (HLD), A");
            break;
        case 0x33: /* INC SP */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_SP;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeINC16;
            strcpy(instr->mnemonic, "INC SP");
            break;
        case 0x34: /* INC (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 3;
            instr->executionFunction = &Cpu::executeINC8;
            strcpy(instr->mnemonic, "INC (HL)");
            break;
        case 0x35: /* DEC (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.reg = RegID_HL;
            instr->cycleCost = 3;
            instr->executionFunction = &Cpu::executeDEC8;
            strcpy(instr->mnemonic, "DEC (HL)");
            break;
        case 0x36: /* LD (HL), d8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 3;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (HL), d8");
            break;
        // case 0x37: /* SCF */
        //     Log::printVerbose("SCF");
        //     reg.setFlagCarry(true);
        //     cycles = 1;
        //     break;
        case 0x38: /* JR C, r8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_PC;
            instr->cycleCost = 3;
            instr->extraInfo = COND_C;
            instr->executionFunction = &Cpu::executeJR;
            strcpy(instr->mnemonic, "JR C, r8");
            break;
        // case 0x39: /* ADD HL, SP */
        //     Log::printVerbose("ADD HL, SP");
        //     src16 = reg.read16(RegID_SP);
        //     executeADD16(RegID_HL, src16);
        //     cycles = 2;
        //     break;
        case 0x3A: /* LD A, (HL-) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8Dec;
            strcpy(instr->mnemonic, "LD A, (HLD)");
            break;
        case 0x3B: /* DEC SP */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_SP;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeDEC16;
            strcpy(instr->mnemonic, "DEC SP");
            break;
        case 0x3C: /* INC A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeINC8;
            strcpy(instr->mnemonic, "INC A");
            break;
        case 0x3D: /* DEC A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_NONE;
            instr->operandSrc.memPtr = RegID_NONE;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeDEC8;
            strcpy(instr->mnemonic, "DEC A");
            break;
        case 0x3E: /* LD A, d8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, d8");
            break;
        // case 0x3F: /* CCF */
        //     Log::printVerbose("CCF");
        //     reg.setFlagCarry(reg.getFlagCarry() ^ 0x1);
        //     cycles = 1;
        //     break;
        case 0x40: /* LD B, B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD B, B");
            break;
        case 0x41: /* LD B, C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD B, C");
            break;
        case 0x42: /* LD B, D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD B, D");
            break;
        case 0x43: /* LD B, E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD B, E");
            break;
        case 0x44: /* LD B, H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD B, H");
            break;
        case 0x45: /* LD B, L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD B, L");
            break;
        case 0x46: /* LD B, (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD B, (HL)");
            break;
        case 0x47: /* LD B, A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD B, A");
            break;
        case 0x48: /* LD C, B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD C, B");
            break;
        case 0x49: /* LD C, C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD C, C");
            break;
        case 0x4A: /* LD C, D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD C, D");
            break;
        case 0x4B: /* LD C, E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD C, E");
            break;
        case 0x4C: /* LD C, H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD C, H");
            break;
        case 0x4D: /* LD C, L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD C, L");
            break;
        case 0x4E: /* LD C, (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD C, (HL)");
            break;
        case 0x4F: /* LD C, A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD C, A");
            break;
        case 0x50: /* LD D, B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD D, B");
            break;
        case 0x51: /* LD D, C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD D, C");
            break;
        case 0x52: /* LD D, D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD D, D");
            break;
        case 0x53: /* LD D, E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD D, E");
            break;
        case 0x54: /* LD D, H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD D, H");
            break;
        case 0x55: /* LD D, L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD D, L");
            break;
        case 0x56: /* LD D, (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD D, (HL)");
            break;
        case 0x57: /* LD D, A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD D, A");
            break;
        case 0x58: /* LD E, B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD E, B");
            break;
        case 0x59: /* LD E, C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD E, C");
            break;
        case 0x5A: /* LD E, D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD E, D");
            break;
        case 0x5B: /* LD E, E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD E, E");
            break;
        case 0x5C: /* LD E, H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD E, H");
            break;
        case 0x5D: /* LD E, L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD E, L");
            break;
        case 0x5E: /* LD E, (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD E, (HL)");
            break;
        case 0x5F: /* LD E, A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD E, A");
            break;
        case 0x60: /* LD H, B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD H, B");
            break;
        case 0x61: /* LD H, C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD H, C");
            break;
        case 0x62: /* LD H, D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD H, D");
            break;
        case 0x63: /* LD H, E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD H, E");
            break;
        case 0x64: /* LD H, H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD H, H");
            break;
        case 0x65: /* LD H, L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD H, L");
            break;
        case 0x66: /* LD H, (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD H, (HL)");
            break;
        case 0x67: /* LD H, A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD H, A");
            break;
        case 0x68: /* LD L, B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD L, B");
            break;
        case 0x69: /* LD L, C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD L, C");
            break;
        case 0x6A: /* LD L, D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD L, D");
            break;
        case 0x6B: /* LD L, E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD L, E");
            break;
        case 0x6C: /* LD L, H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD L, H");
            break;
        case 0x6D: /* LD L, L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD L, L");
            break;
        case 0x6E: /* LD L, (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD L, (HL)");
            break;
        case 0x6F: /* LD L, A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD L, A");
            break;
        case 0x70: /* LD (HL), B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (HL), B");
            break;
        case 0x71: /* LD (HL), C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (HL), C");
            break;
        case 0x72: /* LD (HL), D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (HL), D");
            break;
        case 0x73: /* LD (HL), E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (HL), E");
            break;
        case 0x74: /* LD (HL), H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (HL), H");
            break;
        case 0x75: /* LD (HL), L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (HL), L");
            break;
        // case 0x76: /* HALT */
        //     /* TODO */
        //     break;
        case 0x77: /* LD (HL), A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (HL), A");
            break;
        case 0x78: /* LD A, B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, B");
            break;
        case 0x79: /* LD A, C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, C");
            break;
        case 0x7A: /* LD A, D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, D");
            break;
        case 0x7B: /* LD A, E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, E");
            break;
        case 0x7C: /* LD A, H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, H");
            break;
        case 0x7D: /* LD A, L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, L");
            break;
        case 0x7E: /* LD A, (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, (HL)");
            break;
        case 0x7F: /* LD A, A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, A");
            break;
        case 0x80: /* ADD A, B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADD8;
            strcpy(instr->mnemonic, "ADD A, B");
            break;
        case 0x81: /* ADD A, C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADD8;
            strcpy(instr->mnemonic, "ADD A, C");
            break;
        case 0x82: /* ADD A, D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADD8;
            strcpy(instr->mnemonic, "ADD A, D");
            break;
        case 0x83: /* ADD A, E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADD8;
            strcpy(instr->mnemonic, "ADD A, E");
            break;
        case 0x84: /* ADD A, H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADD8;
            strcpy(instr->mnemonic, "ADD A, H");
            break;
        case 0x85: /* ADD A, L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADD8;
            strcpy(instr->mnemonic, "ADD A, L");
            break;
        case 0x86: /* ADD A, (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeADD8;
            strcpy(instr->mnemonic, "ADD A, (HL)");
            break;
        case 0x87: /* ADD A, A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADD8;
            strcpy(instr->mnemonic, "ADD A, A");
            break;
        case 0x88: /* ADC A, B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADC;
            strcpy(instr->mnemonic, "ADC A, B");
            break;
        case 0x89: /* ADC A, C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADC;
            strcpy(instr->mnemonic, "ADC A, C");
            break;
        case 0x8A: /* ADC A, D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADC;
            strcpy(instr->mnemonic, "ADC A, D");
            break;
        case 0x8B: /* ADC A, E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADC;
            strcpy(instr->mnemonic, "ADC A, E");
            break;
        case 0x8C: /* ADC A, H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADC;
            strcpy(instr->mnemonic, "ADC A, H");
            break;
        case 0x8D: /* ADC A, L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADC;
            strcpy(instr->mnemonic, "ADC A, L");
            break;
        case 0x8E: /* ADC A, (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeADC;
            strcpy(instr->mnemonic, "ADC A, (HL)");
            break;
        case 0x8F: /* ADC A, A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeADC;
            strcpy(instr->mnemonic, "ADC A, A");
            break;
        case 0x90: /* SUB B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSUB;
            strcpy(instr->mnemonic, "SUB B");
            break;
        case 0x91: /* SUB C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSUB;
            strcpy(instr->mnemonic, "SUB C");
            break;
        case 0x92: /* SUB D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSUB;
            strcpy(instr->mnemonic, "SUB D");
            break;
        case 0x93: /* SUB E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSUB;
            strcpy(instr->mnemonic, "SUB E");
            break;
        case 0x94: /* SUB H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSUB;
            strcpy(instr->mnemonic, "SUB H");
            break;
        case 0x95: /* SUB L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSUB;
            strcpy(instr->mnemonic, "SUB L");
            break;
        case 0x96: /* SUB (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSUB;
            strcpy(instr->mnemonic, "SUB B");
            break;
        case 0x97: /* SUB A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSUB;
            strcpy(instr->mnemonic, "SUB A");
            break;
        case 0x98: /* SBC B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSBC;
            strcpy(instr->mnemonic, "SBC A, B");
            break;
        case 0x99: /* SBC C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSBC;
            strcpy(instr->mnemonic, "SBC A, C");
            break;
        case 0x9A: /* SBC D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSBC;
            strcpy(instr->mnemonic, "SBC A, D");
            break;
        case 0x9B: /* SBC E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSBC;
            strcpy(instr->mnemonic, "SBC A, E");
            break;
        case 0x9C: /* SBC H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSBC;
            strcpy(instr->mnemonic, "SBC A, H");
            break;
        case 0x9D: /* SBC L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSBC;
            strcpy(instr->mnemonic, "SBC A, L");
            break;
        case 0x9E: /* SBC (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSBC;
            strcpy(instr->mnemonic, "SBC A, (HL)");
            break;
        case 0x9F: /* SBC A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeSBC;
            strcpy(instr->mnemonic, "SBC A, A");
            break;
        case 0xA0: /* AND B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeAND;
            strcpy(instr->mnemonic, "AND B");
            break;
        case 0xA1: /* AND C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeAND;
            strcpy(instr->mnemonic, "AND C");
            break;
        case 0xA2: /* AND D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeAND;
            strcpy(instr->mnemonic, "AND D");
            break;
        case 0xA3: /* AND E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeAND;
            strcpy(instr->mnemonic, "AND E");
            break;
        case 0xA4: /* AND H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeAND;
            strcpy(instr->mnemonic, "AND H");
            break;
        case 0xA5: /* AND L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeAND;
            strcpy(instr->mnemonic, "AND L");
            break;
        case 0xA6: /* AND (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeAND;
            strcpy(instr->mnemonic, "AND (HL)");
            break;
        case 0xA7: /* AND A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeAND;
            strcpy(instr->mnemonic, "AND A");
            break;
        case 0xA8: /* XOR B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeXOR;
            strcpy(instr->mnemonic, "XOR B");
            break;
        case 0xA9: /* XOR C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeXOR;
            strcpy(instr->mnemonic, "XOR C");
            break;
        case 0xAA: /* XOR D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeXOR;
            strcpy(instr->mnemonic, "XOR D");
            break;
        case 0xAB: /* XOR E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeXOR;
            strcpy(instr->mnemonic, "XOR E");
            break;
        case 0xAC: /* XOR H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeXOR;
            strcpy(instr->mnemonic, "XOR H");
            break;
        case 0xAD: /* XOR L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeXOR;
            strcpy(instr->mnemonic, "XOR L");
            break;
        case 0xAE: /* XOR (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.reg = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeXOR;
            strcpy(instr->mnemonic, "XOR (HL)");
            break;
        case 0xAF: /* XOR A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeXOR;
            strcpy(instr->mnemonic, "XOR A");
            break;
        case 0xB0: /* OR B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeOR;
            strcpy(instr->mnemonic, "OR B");
            break;
        case 0xB1: /* OR C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeOR;
            strcpy(instr->mnemonic, "OR C");
            break;
        case 0xB2: /* OR D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeOR;
            strcpy(instr->mnemonic, "OR D");
            break;
        case 0xB3: /* OR E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeOR;
            strcpy(instr->mnemonic, "OR E");
            break;
        case 0xB4: /* OR H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeOR;
            strcpy(instr->mnemonic, "OR H");
            break;
        case 0xB5: /* OR L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeOR;
            strcpy(instr->mnemonic, "OR L");
            break;
        case 0xB6: /* OR (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeOR;
            strcpy(instr->mnemonic, "OR (HL)");
            break;
        case 0xB7: /* OR A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeOR;
            strcpy(instr->mnemonic, "OR A");
            break;
        case 0xB8: /* CP B */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeCP;
            strcpy(instr->mnemonic, "CP B");
            break;
        case 0xB9: /* CP C */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeCP;
            strcpy(instr->mnemonic, "CP C");
            break;
        case 0xBA: /* CP D */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeCP;
            strcpy(instr->mnemonic, "CP D");
            break;
        case 0xBB: /* CP E */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeCP;
            strcpy(instr->mnemonic, "CP E");
            break;
        case 0xBC: /* CP H */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeCP;
            strcpy(instr->mnemonic, "CP H");
            break;
        case 0xBD: /* CP L */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeCP;
            strcpy(instr->mnemonic, "CP L");
            break;
        case 0xBE: /* CP (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeCP;
            strcpy(instr->mnemonic, "CP (HL)");
            break;
        case 0xBF: /* CP A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeCP;
            strcpy(instr->mnemonic, "CP A");
            break;
        case 0xC0: /* RET NZ */
            instr->instructionLength = 1;
            instr->cycleCost = 5;
            instr->executionFunction = &Cpu::executeRET;
            instr->extraInfo = COND_NZ;
            strcpy(instr->mnemonic, "RET NZ");
            break;
        case 0xC1: /* POP BC */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_SP;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_BC;
            instr->cycleCost = 3;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executePOP;
            strcpy(instr->mnemonic, "POP BC");
            break;
        case 0xC2: /* JP NZ, a16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->cycleCost = 4;
            instr->extraInfo = COND_NZ;
            instr->executionFunction = &Cpu::executeJP;
            strcpy(instr->mnemonic, "JP NZ, a16");
            break;
        case 0xC3: /* JP a16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeJP;
            strcpy(instr->mnemonic, "JP a16");
            break;
        case 0xC4: /* CALL NZ, a16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 6;
            instr->extraInfo = COND_NZ;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "CALL NZ, a16");
            break;
        case 0xC5: /* PUSH BC */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_BC;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_SP;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executePUSH;
            strcpy(instr->mnemonic, "PUSH BC");
            break;
        // case 0xC6: /* ADD A, d8 */
        //     Log::printVerbose("ADD d8");
        //     executeADD8(fetchNextInstruction());
        //     cycles = 2;
        //     break;
        case 0xC7: /* RST 0x00 */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = 0x0000;
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "RST 0x00");
            break;
        case 0xC8: /* RET Z */
            instr->instructionLength = 1;
            instr->cycleCost = 5;
            instr->executionFunction = &Cpu::executeRET;
            instr->extraInfo = COND_Z;
            strcpy(instr->mnemonic, "RET Z");
            break;
        case 0xC9: /* RET */
            instr->instructionLength = 1;
            instr->cycleCost = 4;
            instr->executionFunction = &Cpu::executeRET;
            strcpy(instr->mnemonic, "RET");
            break;
        case 0xCA: /* JP Z, a16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->cycleCost = 4;
            instr->extraInfo = COND_Z;
            instr->executionFunction = &Cpu::executeJP;
            strcpy(instr->mnemonic, "JP Z, a16");
            break;
        case 0xCB: /* PREFIX CB */
            decodePrefixedOpcode(instr);
            break;
        case 0xCC: /* CALL Z, a16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 6;
            instr->extraInfo = COND_Z;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "CALL Z, a16");
            break;
        case 0xCD: /* CALL a16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 6;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "CALL a16");
            break;
        // case 0xCE: /* ADC A, d8 */
        //     Log::printVerbose("ADC d8");
        //     executeADC(fetchNextInstruction());
        //     cycles = 2;
        //     break;
        case 0xCF: /* RST 0x08 */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = 0x0008;
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "RST 0x08");
            break;
        case 0xD0: /* RET NC */
            instr->instructionLength = 1;
            instr->cycleCost = 5;
            instr->executionFunction = &Cpu::executeRET;
            instr->extraInfo = COND_NC;
            strcpy(instr->mnemonic, "RET NC");
            break;
        case 0xD1: /* POP DE */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_SP;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_DE;
            instr->cycleCost = 3;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executePOP;
            strcpy(instr->mnemonic, "POP DE");
            break;
        case 0xD2: /* JP NC, a16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->cycleCost = 4;
            instr->extraInfo = COND_NC;
            instr->executionFunction = &Cpu::executeJP;
            strcpy(instr->mnemonic, "JP NC, a16");
            break;
        // case 0xD3: /* No instruction */
        //     break;
        case 0xD4: /* CALL NC, a16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->cycleCost = 6;
            instr->extraInfo = COND_NC;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "CALL NC, a16");
            break;
        case 0xD5: /* PUSH DE */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_DE;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_SP;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executePUSH;
            strcpy(instr->mnemonic, "PUSH DE");
            break;
        // case 0xD6: /* SUB d8 */
        //     Log::printVerbose("SUB d8");
        //     executeSUB(fetchNextInstruction());
        //     cycles = 2;
        //     break;
        case 0xD7: /* RST 0x10 */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = 0x0010;
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "RST 0x10");
            break;
        case 0xD8: /* RET C */
            instr->instructionLength = 1;
            instr->cycleCost = 5;
            instr->executionFunction = &Cpu::executeRET;
            instr->extraInfo = COND_C;
            strcpy(instr->mnemonic, "RET C");
            break;
        case 0xD9:
            instr->instructionLength = 1;
            instr->cycleCost = 4;
            instr->executionFunction = &Cpu::executeRET;
            instr->extraInfo = COND_IE;
            strcpy(instr->mnemonic, "RETI");
            break;
        case 0xDA: /* JP C, a16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->cycleCost = 4;
            instr->extraInfo = COND_C;
            instr->executionFunction = &Cpu::executeJP;
            strcpy(instr->mnemonic, "JP C, a16");
            break;
        // case 0xDB: /* No instruction */
        //     break;
        case 0xDC: /* CALL C, a16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->cycleCost = 6;
            instr->extraInfo = COND_C;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "CALL C, a16");
            break;
        // case 0xDD: /* No instruction */
        //     break;
        // case 0xDE: /* SBC A, d8 */
        //     Log::printVerbose("SBC A, d8");
        //     executeADC(fetchNextInstruction());
        //     cycles = 2;
        //     break;
        case 0xDF: /* RST 0x18 */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = 0x0018;
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "RST 0x18");
            break;
        case 0xE0: /* LD (a8), A */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_IMM_PTR;
            instr->operandDst.immediate = mmu->read(reg.getProgramCounter() + 1) + 0xff00;
            instr->cycleCost = 3;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (a8), A");
            break;
        case 0xE1: /* POP HL */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_SP;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_HL;
            instr->cycleCost = 3;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executePOP;
            strcpy(instr->mnemonic, "POP HL");
            break;
        case 0xE2: /* LD (C), A */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_IMM_PTR;
            instr->operandDst.immediate = reg.readSingle(RegID_C) + 0xff00;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (C), A");
            break;
        // case 0xE3: /* No instruction */
        //     break;
        // case 0xE4: /* No instruction */
        //     break;
        case 0xE5: /* PUSH HL */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_HL;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_SP;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executePUSH;
            strcpy(instr->mnemonic, "PUSH HL");
            break;
        case 0xE6: /* AND d8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 2;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeAND;
            strcpy(instr->mnemonic, "AND d8");
            break;
        case 0xE7: /* RST 0x20 */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = 0x0020;
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "RST 0x20");
            break;
        // case 0xE8: /* ADD SP, r8 */
        //     Log::printVerbose("ADD SP, r8");
        //     r8 = (i8) fetchNextInstruction();
        //     executeADD16(RegID_HL, r8);
        //     cycles = 2;
        //     break;
        case 0xE9: /* JP (HL) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_HL;
            instr->cycleCost = 1;
            instr->extraInfo = COND_HL;
            instr->executionFunction = &Cpu::executeJP;
            strcpy(instr->mnemonic, "JP (HL)");
            break;
        case 0xEA: /* LD a16, A */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_IMM_PTR;
            instr->operandDst.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD (a16), A");
            break;
        // case 0xEB: /* No instruction */
        //     break;
        // case 0xEC: /* No instruction */
        //     break;
        // case 0xED: /* No instruction */
        //     break;
        // case 0xEE: /* XOR d8 */
        //     Log::printVerbose("XOR d8");
        //     executeXOR(fetchNextInstruction());
        //     cycles = 2;
        //     break;
        case 0xEF: /* RST 0x28 */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = 0x0028;
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "RST 0x28");
            break;
        case 0xF0: /* LD A, (a8) */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM_PTR;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1) + 0xff00;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 3;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, (a8)");
            break;
        case 0xF1: /* POP AF */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_SP;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_AF;
            instr->cycleCost = 3;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executePOP;
            strcpy(instr->mnemonic, "POP AF");
            break;
        case 0xF2: /* LD A, (C) */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_IMM_PTR;
            instr->operandSrc.immediate = reg.readSingle(RegID_C) + 0xff00;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, (C)");
            break;
        case 0xF3: /* DI */
            instr->instructionLength = 1;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeDI;
            strcpy(instr->mnemonic, "DI");
            break;
        // case 0xF4: /* No instruction */
        //     break;
        case 0xF5: /* PUSH AF */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_AF;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_SP;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executePUSH;
            strcpy(instr->mnemonic, "PUSH AF");
            break;
        // case 0xF6: /* OR d8 */
        //     Log::printVerbose("OR d8");
        //     executeOR(fetchNextInstruction());
        //     cycles = 2;
        //     break;
        case 0xF7: /* RST 0x30 */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = 0x0030;
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "RST 0x30");
            break;
        // case 0xF8: /* LD HL, SP+r8 */
        //     Log::printVerbose("LD HL, SP+r8");
        //     src = (i8) fetchNextInstruction();
        //     src16 = reg.getStackPointer();
        //     reg.write16(RegID_HL, src16 + src);
        //     cycles = 3;
        //     break;
        // case 0xF9: /* LD SP, HL */
        //     Log::printVerbose("LD SP, HL");
        //     src16 = reg.read16(RegID_HL);
        //     reg.write16(RegID_SP, src16);
        //     break;
        case 0xFA: /* LD A, a16 */
            instr->instructionLength = 3;
            instr->operandSrc.type = OP_IMM_PTR;
            instr->operandSrc.immediate = mmu->read2Bytes(reg.getProgramCounter() + 1);
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeLD8;
            strcpy(instr->mnemonic, "LD A, (a16)");
            break;
        case 0xFB: /* EI */
            instr->instructionLength = 1;
            instr->cycleCost = 1;
            instr->executionFunction = &Cpu::executeEI;
            strcpy(instr->mnemonic, "EI");
            break;
        // case 0xFC: /* No instruction */
        //     break;
        // case 0xFD: /* No instruction */
        //     break;
        case 0xFE: /* CP d8 */
            instr->instructionLength = 2;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = mmu->read(reg.getProgramCounter() + 1);
            instr->cycleCost = 2;
            instr->executionFunction = &Cpu::executeCP;
            strcpy(instr->mnemonic, "CP d8");
            break;
        case 0xFF: /* RST 0x38 */
            instr->instructionLength = 1;
            instr->operandSrc.type = OP_IMM;
            instr->operandSrc.immediate = 0x0038;
            instr->operandDst.type = OP_NONE;
            instr->cycleCost = 4;
            instr->extraInfo = COND_NONE;
            instr->executionFunction = &Cpu::executeCALL;
            strcpy(instr->mnemonic, "RST 0x38");
            break;
        default:
            fmt::print(stderr, "Error, unsupported opcode: {:#02x}\n", opcode);
            exit(EXIT_FAILURE);
            break;
    }
}


/**
 * Decoding of the prefixed opcode table. This table contains 8 bit shifts, rotations and bit
 * functions.
 */
void Cpu::decodePrefixedOpcode(instruction_t *instr)
{
    u8 prefixedOpcode = mmu->read(reg.getProgramCounter() + 1);
    instr->instructionLength = 2;
    instr->cycleCost = 2;

    /* Find the instruction that needs to be executed. */
    switch(prefixedOpcode)
    {
//         case 0x0: /* RLC B */
//             Log::printVerbose("RLC B");
//             src = reg.read8(RegID_B);
//             result = executeRLC(src);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0x1: /* RLC C */
//             Log::printVerbose("RLC C");
//             src = reg.read8(RegID_C);
//             result = executeRLC(src);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0x2: /* RLC D */
//             Log::printVerbose("RLC D");
//             src = reg.read8(RegID_D);
//             result = executeRLC(src);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0x3: /* RLC E */
//             Log::printVerbose("RLC E");
//             src = reg.read8(RegID_E);
//             result = executeRLC(src);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0x4: /* RLC H */
//             Log::printVerbose("RLC H");
//             src = reg.read8(RegID_H);
//             result = executeRLC(src);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0x5: /* RLC L */
//             Log::printVerbose("RLC L");
//             src = reg.read8(RegID_L);
//             result = executeRLC(src);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0x6: /* RLC (HL) */
//             Log::printVerbose("RLC (HL)");
//             src = readMem(RegID_HL);
//             result = executeRLC(src);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0x7: /* RLC A */
//             Log::printVerbose("RLC A");
//             src = reg.read8(RegID_A);
//             result = executeRLC(src);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0x8: /* RRC B */
//             Log::printVerbose("RRC B");
//             src = reg.read8(RegID_B);
//             result = executeRRC(src);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0x9: /* RRC C */
//             Log::printVerbose("RRC C");
//             src = reg.read8(RegID_C);
//             result = executeRRC(src);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xA: /* RRC D */
//             Log::printVerbose("RRC D");
//             src = reg.read8(RegID_D);
//             result = executeRRC(src);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xB: /* RRC E */
//             Log::printVerbose("RRC E");
//             src = reg.read8(RegID_E);
//             result = executeRRC(src);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0xC: /* RRC H */
//             Log::printVerbose("RRC H");
//             src = reg.read8(RegID_H);
//             result = executeRRC(src);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xD: /* RRC L */
//             Log::printVerbose("RRC L");
//             src = reg.read8(RegID_L);
//             result = executeRRC(src);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xE: /* RRC (HL) */
//             Log::printVerbose("RRC (HL)");
//             src = readMem(RegID_HL);
//             result = executeRRC(src);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xF: /* RRC A */
//             Log::printVerbose("RRC A");
//             src = reg.read8(RegID_A);
//             result = executeRRC(src);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0x10: /* RL B */
//             Log::printVerbose("RL B");
//             src = reg.read8(RegID_B);
//             result = executeRL(src);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0x11: /* RL C */
//             Log::printVerbose("RL C");
//             src = reg.read8(RegID_C);
//             result = executeRL(src);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0x12: /* RL D */
//             Log::printVerbose("RL D");
//             src = reg.read8(RegID_D);
//             result = executeRL(src);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0x13: /* RL E */
//             Log::printVerbose("RL E");
//             src = reg.read8(RegID_E);
//             result = executeRL(src);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0x14: /* RL H */
//             Log::printVerbose("RL H");
//             src = reg.read8(RegID_H);
//             result = executeRL(src);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0x15: /* RL L */
//             Log::printVerbose("RL L");
//             src = reg.read8(RegID_L);
//             result = executeRL(src);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0x16: /* RL (HL) */
//             Log::printVerbose("RL (HL)");
//             src = readMem(RegID_HL);
//             result = executeRL(src);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0x17: /* RL A */
//             Log::printVerbose("RL A");
//             src = reg.read8(RegID_A);
//             result = executeRL(src);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0x18: /* RR B */
//             Log::printVerbose("RR B");
//             src = reg.read8(RegID_B);
//             result = executeRR(src);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0x19: /* RR C */
//             Log::printVerbose("RR C");
//             src = reg.read8(RegID_C);
//             result = executeRR(src);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0x1A: /* RR D */
//             Log::printVerbose("RR D");
//             src = reg.read8(RegID_D);
//             result = executeRR(src);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0x1B: /* RR E */
//             Log::printVerbose("RR E");
//             src = reg.read8(RegID_E);
//             result = executeRR(src);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0x1C:/* RR H */
//             Log::printVerbose("RR H");
//             src = reg.read8(RegID_H);
//             result = executeRR(src);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0x1D: /* RR L */
//             Log::printVerbose("RR L");
//             src = reg.read8(RegID_L);
//             result = executeRR(src);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0x1E: /* RR (HL) */
//             Log::printVerbose("RR (HL)");
//             src = readMem(RegID_HL);
//             result = executeRR(src);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0x1F: /* RR A */
//             Log::printVerbose("RR A");
//             src = reg.read8(RegID_A);
//             result = executeRR(src);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0x20: /* SLA B */
//             Log::printVerbose("SLA B");
//             src = reg.read8(RegID_B);
//             result = executeSLA(src);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0x21: /* SLA C */
//             Log::printVerbose("SLA C");
//             src = reg.read8(RegID_C);
//             result = executeSLA(src);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0x22: /* SLA D */
//             Log::printVerbose("SLA D");
//             src = reg.read8(RegID_D);
//             result = executeSLA(src);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0x23: /* SLA E */
//             Log::printVerbose("SLA E");
//             src = reg.read8(RegID_E);
//             result = executeSLA(src);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0x24: /* SLA H */
//             Log::printVerbose("SLA H");
//             src = reg.read8(RegID_H);
//             result = executeSLA(src);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0x25: /* SLA L */
//             Log::printVerbose("SLA L");
//             src = reg.read8(RegID_L);
//             result = executeSLA(src);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0x26: /* SLA (HL) */
//             Log::printVerbose("SLA (HL)");
//             src = readMem(RegID_HL);
//             result = executeSLA(src);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0x27: /* SLA A */
//             Log::printVerbose("SLA A");
//             src = reg.read8(RegID_A);
//             result = executeSLA(src);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0x28: /* SRA B */
//             Log::printVerbose("SRA B");
//             src = reg.read8(RegID_B);
//             result = executeSRA(src);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0x29: /* SRA C */
//             Log::printVerbose("SRA C");
//             src = reg.read8(RegID_C);
//             result = executeSRA(src);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0x2A: /* SRA D */
//             Log::printVerbose("SRA D");
//             src = reg.read8(RegID_D);
//             result = executeSRA(src);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0x2B: /* SRA E */
//             Log::printVerbose("SRA E");
//             src = reg.read8(RegID_E);
//             result = executeSRA(src);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0x2C: /* SRA H */
//             Log::printVerbose("SRA H");
//             src = reg.read8(RegID_H);
//             result = executeSRA(src);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0x2D: /* SRA L */
//             Log::printVerbose("SRA L");
//             src = reg.read8(RegID_L);
//             result = executeSRA(src);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0x2E: /* SRA (HL) */
//             Log::printVerbose("SRA (HL)");
//             src = readMem(RegID_HL);
//             result = executeSRA(src);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0x2F: /* SRA A */
//             Log::printVerbose("SRA A");
//             src = reg.read8(RegID_A);
//             result = executeSRA(src);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
        case 0x30: /* SWAP B */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->executionFunction = &Cpu::executeSWAP;
            strcpy(instr->mnemonic, "SWAP B");
            break;
        case 0x31: /* SWAP C */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->executionFunction = &Cpu::executeSWAP;
            strcpy(instr->mnemonic, "SWAP C");
            break;
        case 0x32: /* SWAP D */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->executionFunction = &Cpu::executeSWAP;
            strcpy(instr->mnemonic, "SWAP D");
            break;
        case 0x33: /* SWAP E */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->executionFunction = &Cpu::executeSWAP;
            strcpy(instr->mnemonic, "SWAP E");
            break;
        case 0x34: /* SWAP H */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->executionFunction = &Cpu::executeSWAP;
            strcpy(instr->mnemonic, "SWAP H");
            break;
        case 0x35: /* SWAP L */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->executionFunction = &Cpu::executeSWAP;
            strcpy(instr->mnemonic, "SWAP L");
            break;
        case 0x36: /* SWAP (HL) */
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->cycleCost = 4;
            instr->executionFunction = &Cpu::executeSWAP;
            strcpy(instr->mnemonic, "SWAP (HL)");
            break;
        case 0x37: /* SWAP A */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->executionFunction = &Cpu::executeSWAP;
            strcpy(instr->mnemonic, "SWAP A");
            break;
//         case 0x38: /* SRL B */
//             Log::printVerbose("SRL B");
//             src = reg.read8(RegID_B);
//             result = executeSRL(src);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0x39: /* SRL C */
//             Log::printVerbose("SRL C");
//             src = reg.read8(RegID_C);
//             result = executeSRL(src);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0x3A: /* SRL D */
//             Log::printVerbose("SRL D");
//             src = reg.read8(RegID_D);
//             result = executeSRL(src);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0x3B: /* SRL E */
//             Log::printVerbose("SRL E");
//             src = reg.read8(RegID_E);
//             result = executeSRL(src);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0x3C: /* SRL H */
//             Log::printVerbose("SRL H");
//             src = reg.read8(RegID_H);
//             result = executeSRL(src);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0x3D: /* SRL L */
//             Log::printVerbose("SRL L");
//             src = reg.read8(RegID_L);
//             result = executeSRL(src);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0x3E: /* SRL A */
//             Log::printVerbose("SRL A");
//             src = readMem(RegID_HL);
//             result = executeSRL(src);
//             writeMem(RegID_HL, result);
//             cycles = 2;
//             break;
//         case 0x3F: /* SRL A */
//             Log::printVerbose("SRL A");
//             src = reg.read8(RegID_A);
//             result = executeSRL(src);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0x40: /* BIT 0, B */
//             Log::printVerbose("BIT 0, B");
//             src = reg.read8(RegID_B);
//             executeBIT(src, 0);
//             cycles = 2;
//             break;
//         case 0x41: /* BIT 0, C */
//             Log::printVerbose("BIT 0, C");
//             src = reg.read8(RegID_C);
//             executeBIT(src, 0);
//             cycles = 2;
//             break;
//         case 0x42: /* BIT 0, D */
//             Log::printVerbose("BIT 0, D");
//             src = reg.read8(RegID_D);
//             executeBIT(src, 0);
//             cycles = 2;
//             break;
//         case 0x43: /* BIT 0, E */
//             Log::printVerbose("BIT 0, E");
//             src = reg.read8(RegID_E);
//             executeBIT(src, 0);
//             cycles = 2;
//             break;
//         case 0x44: /* BIT 0, H */
//             Log::printVerbose("BIT 0, H");
//             src = reg.read8(RegID_H);
//             executeBIT(src, 0);
//             cycles = 2;
//             break;
//         case 0x45: /* BIT 0, L */
//             Log::printVerbose("BIT 0, L");
//             src = reg.read8(RegID_L);
//             executeBIT(src, 0);
//             cycles = 2;
//             break;
//         case 0x46: /* BIT 0, (HL) */
//             Log::printVerbose("BIT 0, (HL)");
//             src = readMem(RegID_HL);
//             executeBIT(src, 0);
//             cycles = 4;
//             break;
//         case 0x47: /* BIT 0, A */
//             Log::printVerbose("BIT 0, A");
//             src = reg.read8(RegID_A);
//             executeBIT(src, 0);
//             cycles = 2;
//             break;
//         case 0x48: /* BIT 1, B */
//             Log::printVerbose("BIT 1, B");
//             src = reg.read8(RegID_B);
//             executeBIT(src, 1);
//             cycles = 2;
//             break;
//         case 0x49: /* BIT 1, C */
//             Log::printVerbose("BIT 1, C");
//             src = reg.read8(RegID_C);
//             executeBIT(src, 1);
//             cycles = 2;
//             break;
//         case 0x4A: /* BIT 1, D */
//             Log::printVerbose("BIT 1, D");
//             src = reg.read8(RegID_D);
//             executeBIT(src, 1);
//             cycles = 2;
//             break;
//         case 0x4B: /* BIT 1, E */
//             Log::printVerbose("BIT 1, E");
//             src = reg.read8(RegID_E);
//             executeBIT(src, 1);
//             cycles = 2;
//             break;
//         case 0x4C: /* BIT 1, H */
//             Log::printVerbose("BIT 1, H");
//             src = reg.read8(RegID_H);
//             executeBIT(src, 1);
//             cycles = 2;
//             break;
//         case 0x4D: /* BIT 1, L */
//             Log::printVerbose("BIT 1, L");
//             src = reg.read8(RegID_L);
//             executeBIT(src, 1);
//             cycles = 2;
//             break;
//         case 0x4E: /* BIT 1 (HL) */
//             Log::printVerbose("BIT 1, (HL)");
//             src = readMem(RegID_HL);
//             executeBIT(src, 1);
//             cycles = 4;
//             break;
//         case 0x4F: /* BIT 1, A */
//             Log::printVerbose("BIT 1, A");
//             src = reg.read8(RegID_A);
//             executeBIT(src, 1);
//             cycles = 2;
//             break;
//         case 0x50: /* BIT 2, B */
//             Log::printVerbose("BIT 2, B");
//             src = reg.read8(RegID_B);
//             executeBIT(src, 2);
//             cycles = 2;
//             break;
//         case 0x51: /* BIT 2, C */
//             Log::printVerbose("BIT 2, C");
//             src = reg.read8(RegID_C);
//             executeBIT(src, 2);
//             cycles = 2;
//             break;
//         case 0x52: /* BIT 2, D */
//             Log::printVerbose("BIT 2, D");
//             src = reg.read8(RegID_D);
//             executeBIT(src, 2);
//             cycles = 2;
//             break;
//         case 0x53: /* BIT 2, E */
//             Log::printVerbose("BIT 2, E");
//             src = reg.read8(RegID_E);
//             executeBIT(src, 2);
//             cycles = 2;
//             break;
//         case 0x54: /* BIT 2, H */
//             Log::printVerbose("BIT 2, H");
//             src = reg.read8(RegID_H);
//             executeBIT(src, 2);
//             cycles = 2;
//             break;
//         case 0x55: /* BIT 2, L */
//             Log::printVerbose("BIT 2, L");
//             src = reg.read8(RegID_L);
//             executeBIT(src, 2);
//             cycles = 2;
//             break;
//         case 0x56: /* BIT 2, (HL) */
//             Log::printVerbose("BIT 2, (HL)");
//             src = readMem(RegID_HL);
//             executeBIT(src, 2);
//             cycles = 4;
//             break;
//         case 0x57: /* BIT 2, A */
//             Log::printVerbose("BIT 2, A");
//             src = reg.read8(RegID_A);
//             executeBIT(src, 2);
//             cycles = 2;
//             break;
//         case 0x58: /* BIT 3, B */
//             Log::printVerbose("BIT 3, B");
//             src = reg.read8(RegID_B);
//             executeBIT(src, 3);
//             cycles = 2;
//             break;
//         case 0x59: /* BIT 3, C */
//             Log::printVerbose("BIT 3, C");
//             src = reg.read8(RegID_C);
//             executeBIT(src, 3);
//             cycles = 2;
//             break;
//         case 0x5A: /* BIT 3, D */
//             Log::printVerbose("BIT 3, D");
//             src = reg.read8(RegID_D);
//             executeBIT(src, 3);
//             cycles = 2;
//             break;
//         case 0x5B: /* BIT 3, E */
//             Log::printVerbose("BIT 3, E");
//             src = reg.read8(RegID_E);
//             executeBIT(src, 3);
//             cycles = 2;
//             break;
//         case 0x5C: /* BIT 3, H */
//             Log::printVerbose("BIT 3, H");
//             src = reg.read8(RegID_H);
//             executeBIT(src, 3);
//             cycles = 2;
//             break;
//         case 0x5D: /* BIT 3, L */
//             Log::printVerbose("BIT 3, L");
//             src = reg.read8(RegID_L);
//             executeBIT(src, 3);
//             cycles = 2;
//             break;
//         case 0x5E: /* BIT 3, (HL) */
//             Log::printVerbose("BIT 3, (HL)");
//             src = readMem(RegID_HL);
//             executeBIT(src, 3);
//             cycles = 4;
//             break;
//         case 0x5F: /* BIT 3, A */
//             Log::printVerbose("BIT 3, A");
//             src = reg.read8(RegID_A);
//             executeBIT(src, 3);
//             cycles = 2;
//             break;
//         case 0x60: /* BIT 4, B */
//             Log::printVerbose("BIT 4, B");
//             src = reg.read8(RegID_B);
//             executeBIT(src, 4);
//             cycles = 2;
//             break;
//         case 0x61: /* BIT 4, C */
//             Log::printVerbose("BIT 4, C");
//             src = reg.read8(RegID_C);
//             executeBIT(src, 4);
//             cycles = 2;
//             break;
//         case 0x62: /* BIT 4, D */
//             Log::printVerbose("BIT 4, D");
//             src = reg.read8(RegID_D);
//             executeBIT(src, 4);
//             cycles = 2;
//             break;
//         case 0x63: /* BIT 4, E */
//             Log::printVerbose("BIT 4, E");
//             src = reg.read8(RegID_E);
//             executeBIT(src, 4);
//             cycles = 2;
//             break;
//         case 0x64: /* BIT 4, H */
//             Log::printVerbose("BIT 4, H");
//             src = reg.read8(RegID_H);
//             executeBIT(src, 4);
//             cycles = 2;
//             break;
//         case 0x65: /* BIT 4, L */
//             Log::printVerbose("BIT 4, L");
//             src = reg.read8(RegID_L);
//             executeBIT(src, 4);
//             cycles = 2;
//             break;
//         case 0x66: /* BIT 4, (HL) */
//             Log::printVerbose("BIT 4, (HL)");
//             src = readMem(RegID_HL);
//             executeBIT(src, 4);
//             cycles = 4;
//             break;
//         case 0x67: /* BIT 4, A */
//             Log::printVerbose("BIT 4, A");
//             src = reg.read8(RegID_A);
//             executeBIT(src, 4);
//             cycles = 2;
//             break;
//         case 0x68: /* BIT 5, B */
//             Log::printVerbose("BIT 5, B");
//             src = reg.read8(RegID_B);
//             executeBIT(src, 5);
//             cycles = 2;
//             break;
//         case 0x69: /* BIT 5, C */
//             Log::printVerbose("BIT 5, C");
//             src = reg.read8(RegID_C);
//             executeBIT(src, 5);
//             cycles = 2;
//             break;
//         case 0x6A: /* BIT 5, D */
//             Log::printVerbose("BIT 5, D");
//             src = reg.read8(RegID_D);
//             executeBIT(src, 5);
//             cycles = 2;
//             break;
//         case 0x6B: /* BIT 5, E */
//             Log::printVerbose("BIT 5, E");
//             src = reg.read8(RegID_E);
//             executeBIT(src, 5);
//             cycles = 2;
//             break;
//         case 0x6C: /* BIT 5, H */
//             Log::printVerbose("BIT 5, H");
//             src = reg.read8(RegID_H);
//             executeBIT(src, 5);
//             cycles = 2;
//             break;
//         case 0x6D: /* BIT 5, L */
//             Log::printVerbose("BIT 5, L");
//             src = reg.read8(RegID_L);
//             executeBIT(src, 5);
//             cycles = 2;
//             break;
//         case 0x6E: /* BIT 5, (HL) */
//             Log::printVerbose("BIT 5, (HL)");
//             src = readMem(RegID_HL);
//             executeBIT(src, 5);
//             cycles = 4;
//             break;
//         case 0x6F: /* BIT 5, A */
//             Log::printVerbose("BIT 5, A");
//             src = reg.read8(RegID_A);
//             executeBIT(src, 5);
//             cycles = 2;
//             break;
//         case 0x70: /* BIT 6, B */
//             Log::printVerbose("BIT 6, B");
//             src = reg.read8(RegID_B);
//             executeBIT(src, 6);
//             cycles = 2;
//             break;
//         case 0x71: /* BIT 6, C */
//             Log::printVerbose("BIT 6, C");
//             src = reg.read8(RegID_C);
//             executeBIT(src, 6);
//             cycles = 2;
//             break;
//         case 0x72: /* BIT 6, D */
//             Log::printVerbose("BIT 6, D");
//             src = reg.read8(RegID_D);
//             executeBIT(src, 6);
//             cycles = 2;
//             break;
//         case 0x73: /* BIT 6, E */
//             Log::printVerbose("BIT 6, H");
//             src = reg.read8(RegID_H);
//             executeBIT(src, 6);
//             cycles = 2;
//             break;
//         case 0x74: /* BIT 6, H */
//             Log::printVerbose("BIT 6, H");
//             src = reg.read8(RegID_H);
//             executeBIT(src, 6);
//             cycles = 2;
//             break;
//         case 0x75: /* BIT 6, L */
//             Log::printVerbose("BIT 6, L");
//             src = reg.read8(RegID_L);
//             executeBIT(src, 6);
//             cycles = 2;
//             break;
//         case 0x76: /* BIT 6 (HL) */
//             Log::printVerbose("BIT 6, (HL)");
//             src = readMem(RegID_HL);
//             executeBIT(src, 6);
//             cycles = 4;
//             break;
//         case 0x77: /* BIT 6, A */
//             Log::printVerbose("BIT 6, A");
//             src = reg.read8(RegID_A);
//             executeBIT(src, 6);
//             cycles = 2;
//             break;
//         case 0x78: /* BIT 7, B */
//             Log::printVerbose("BIT 7, B");
//             src = reg.read8(RegID_B);
//             executeBIT(src, 7);
//             cycles = 2;
//             break;
//         case 0x79: /* BIT 7, C */
//             Log::printVerbose("BIT 7, C");
//             src = reg.read8(RegID_C);
//             executeBIT(src, 7);
//             cycles = 2;
//             break;
//         case 0x7A: /* BIT 7, D */
//             Log::printVerbose("BIT 7, D");
//             src = reg.read8(RegID_D);
//             executeBIT(src, 7);
//             cycles = 2;
//             break;
//         case 0x7B: /* BIT 7, E */
//             Log::printVerbose("BIT 7, E");
//             src = reg.read8(RegID_E);
//             executeBIT(src, 7);
//             cycles = 2;
//             break;
//         case 0x7C: /* BIT 7, H */
//             Log::printVerbose("BIT 7, H");
//             src = reg.read8(RegID_H);
//             executeBIT(src, 7);
//             cycles = 2;
//             break;
//         case 0x7D: /* BIT 7, L */
//             Log::printVerbose("BIT 7, L");
//             src = reg.read8(RegID_L);
//             executeBIT(src, 7);
//             cycles = 2;
//             break;
//         case 0x7E: /* BIT 7, (HL) */
//             Log::printVerbose("BIT 7, (HL)");
//             src = readMem(RegID_HL);
//             executeBIT(src, 7);
//             cycles = 4;
//             break;
//         case 0x7F: /* BIT 7, A */
//             Log::printVerbose("BIT 7, A");
//             src = reg.read8(RegID_A);
//             executeBIT(src, 7);
//             cycles = 2;
//             break;
        case 0x80: /* RES 0, B */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->extraInfo = 0;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 0, B");
            break;
        case 0x81: /* RES 0, C */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->extraInfo = 0;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 0, C");
            break;
        case 0x82: /* RES 0, D */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->extraInfo = 0;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 0, D");
            break;
        case 0x83: /* RES 0, E */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->extraInfo = 0;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 0, E");
            break;
        case 0x84: /* RES 0, H */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->extraInfo = 0;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 0, H");
            break;
        case 0x85: /* RES 0, L */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->extraInfo = 0;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 0, L");
            break;
        case 0x86: /* RES 0, (HL) */
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->extraInfo = 0;
            instr->cycleCost = 4;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 0, (HL)");
            break;
        case 0x87: /* RES 0, A */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->extraInfo = 0;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 0, A");
            break;
        case 0x88: /* RES 1, B */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->extraInfo = 1;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 1, B");
            break;
        case 0x89: /* RES 1, C */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->extraInfo = 1;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 1, C");
            break;
        case 0x8A: /* RES 1, D */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->extraInfo = 1;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 1, D");
            break;
        case 0x8B: /* RES 1, E */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->extraInfo = 1;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 1, E");
            break;
        case 0x8C: /* RES 1, H */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->extraInfo = 1;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 1, H");
            break;
        case 0x8D: /* RES 1, L */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->extraInfo = 1;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 1, L");
            break;
        case 0x8E: /* RES 1 (HL) */
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->extraInfo = 1;
            instr->cycleCost = 4;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 1, (HL)");
            break;
        case 0x8F: /* RES 1, A */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->extraInfo = 1;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 1, A");
            break;
        case 0x90: /* RES 2, B */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->extraInfo = 2;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 2, B");
            break;
        case 0x91: /* RES 2, C */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->extraInfo = 2;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 2, C");
            break;
        case 0x92: /* RES 2, D */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->extraInfo = 2;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 2, D");
            break;
        case 0x93: /* RES 2, E */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->extraInfo = 2;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 2, E");
            break;
        case 0x94: /* RES 2, H */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->extraInfo = 2;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 2, H");
            break;
        case 0x95: /* RES 2, L */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->extraInfo = 2;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 2, L");
            break;
        case 0x96: /* RES 2, (HL) */
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->extraInfo = 2;
            instr->cycleCost = 4;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 2, (HL)");
            break;
        case 0x97: /* RES 2, A */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->extraInfo = 2;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 2, A");
            break;
        case 0x98: /* RES 3, B */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_B;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_B;
            instr->extraInfo = 3;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 3, B");
            break;
        case 0x99: /* RES 3, C */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_C;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_C;
            instr->extraInfo = 3;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 3, C");
            break;
        case 0x9A: /* RES 3, D */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_D;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_D;
            instr->extraInfo = 3;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 3, D");
            break;
        case 0x9B: /* RES 3, E */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_E;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_E;
            instr->extraInfo = 3;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 3, E");
            break;
        case 0x9C: /* RES 3, H */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_H;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_H;
            instr->extraInfo = 3;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 3, H");
            break;
        case 0x9D: /* RES 3, L */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_L;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_L;
            instr->extraInfo = 3;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 3, L");
            break;
        case 0x9E: /* RES 3, (HL) */
            instr->operandSrc.type = OP_MEM;
            instr->operandSrc.memPtr = RegID_HL;
            instr->operandDst.type = OP_MEM;
            instr->operandDst.memPtr = RegID_HL;
            instr->extraInfo = 3;
            instr->cycleCost = 4;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 3, (HL)");
            break;
        case 0x9F: /* RES 3, A */
            instr->operandSrc.type = OP_REG;
            instr->operandSrc.reg = RegID_A;
            instr->operandDst.type = OP_REG;
            instr->operandDst.reg = RegID_A;
            instr->extraInfo = 3;
            instr->executionFunction = &Cpu::executeRES;
            strcpy(instr->mnemonic, "RES 3, A");
            break;
//         case 0xA0: /* RES 4, B */
//             Log::printVerbose("RES 4, B");
//             src = reg.read8(RegID_B);
//             result = executeRES(src, 4);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xA1: /* RES 4, C */
//             Log::printVerbose("RES 4, C");
//             src = reg.read8(RegID_C);
//             result = executeRES(src, 4);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xA2: /* RES 4, D */
//             Log::printVerbose("RES 4, D");
//             src = reg.read8(RegID_D);
//             result = executeRES(src, 4);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xA3: /* RES 4, E */
//             Log::printVerbose("RES 4, E");
//             src = reg.read8(RegID_E);
//             result = executeRES(src, 4);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0xA4: /* RES 4, H */
//             Log::printVerbose("RES 4, H");
//             src = reg.read8(RegID_H);
//             result = executeRES(src, 4);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xA5: /* RES 4, L */
//             Log::printVerbose("RES 4, L");
//             src = reg.read8(RegID_L);
//             result = executeRES(src, 4);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xA6: /* RES 4, (HL) */
//             Log::printVerbose("RES 4, (HL)");
//             src = readMem(RegID_HL);
//             result = executeRES(src, 4);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xA7: /* RES 4, A */
//             Log::printVerbose("RES 4, A");
//             src = reg.read8(RegID_A);
//             result = executeRES(src, 4);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0xA8: /* RES 5, B */
//             Log::printVerbose("RES 5, B");
//             src = reg.read8(RegID_B);
//             result = executeRES(src, 5);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xA9: /* RES 5, C */
//             Log::printVerbose("RES 5, C");
//             src = reg.read8(RegID_C);
//             result = executeRES(src, 5);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xAA: /* RES 5, D */
//             Log::printVerbose("RES 5, D");
//             src = reg.read8(RegID_D);
//             result = executeRES(src, 5);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xAB: /* RES 5, E */
//             Log::printVerbose("RES 5, E");
//             src = reg.read8(RegID_E);
//             result = executeRES(src, 5);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0xAC: /* RES 5, H */
//             Log::printVerbose("RES 5, H");
//             src = reg.read8(RegID_H);
//             result = executeRES(src, 5);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xAD: /* RES 5, L */
//             Log::printVerbose("RES 5, L");
//             src = reg.read8(RegID_L);
//             result = executeRES(src, 5);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xAE: /* RES 5, (HL) */
//             Log::printVerbose("RES 5, (HL)");
//             src = readMem(RegID_HL);
//             result = executeRES(src, 5);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xAF: /* RES 5, A */
//             Log::printVerbose("RES 5, A");
//             src = reg.read8(RegID_A);
//             result = executeRES(src, 5);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0xB0: /* RES 6, B */
//             Log::printVerbose("RES 6, B");
//             src = reg.read8(RegID_B);
//             result = executeRES(src, 6);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xB1: /* RES 6, C */
//             Log::printVerbose("RES 6, C");
//             src = reg.read8(RegID_C);
//             result = executeRES(src, 6);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xB2: /* RES 6, D */
//             Log::printVerbose("RES 6, D");
//             src = reg.read8(RegID_D);
//             result = executeRES(src, 6);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xB3: /* RES 6, E */
//             Log::printVerbose("RES 6, H");
//             src = reg.read8(RegID_H);
//             result = executeRES(src, 6);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xB4: /* RES 6, H */
//             Log::printVerbose("RES 6, H");
//             src = reg.read8(RegID_H);
//             result = executeRES(src, 6);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xB5: /* RES 6, L */
//             Log::printVerbose("RES 6, L");
//             src = reg.read8(RegID_L);
//             result = executeRES(src, 6);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xB6: /* RES 6 (HL) */
//             Log::printVerbose("RES 6, (HL)");
//             src = readMem(RegID_HL);
//             result = executeRES(src, 6);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xB7: /* RES 6, A */
//             Log::printVerbose("RES 6, A");
//             src = reg.read8(RegID_A);
//             result = executeRES(src, 6);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0xB8: /* RES 7, B */
//             Log::printVerbose("RES 7, B");
//             src = reg.read8(RegID_B);
//             result = executeRES(src, 7);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xB9: /* RES 7, C */
//             Log::printVerbose("RES 7, C");
//             src = reg.read8(RegID_C);
//             result = executeRES(src, 7);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xBA: /* RES 7, D */
//             Log::printVerbose("RES 7, D");
//             src = reg.read8(RegID_D);
//             result = executeRES(src, 7);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xBB: /* RES 7, E */
//             Log::printVerbose("RES 7, E");
//             src = reg.read8(RegID_E);
//             result = executeRES(src, 7);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0xBC: /* RES 7, H */
//             Log::printVerbose("RES 7, H");
//             src = reg.read8(RegID_H);
//             result = executeRES(src, 7);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xBD: /* RES 7, L */
//             Log::printVerbose("RES 7, L");
//             src = reg.read8(RegID_L);
//             result = executeRES(src, 7);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xBE: /* RES 7, (HL) */
//             Log::printVerbose("RES 7, (HL)");
//             src = readMem(RegID_HL);
//             result = executeRES(src, 7);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xBF: /* RES 7, A */
//             Log::printVerbose("RES 7, A");
//             src = reg.read8(RegID_A);
//             result = executeRES(src, 7);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0xC0: /* SET 0, B */
//             Log::printVerbose("SET 0, B");
//             src = reg.read8(RegID_B);
//             result = executeSET(src, 0);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xC1: /* SET 0, C */
//             Log::printVerbose("SET 0, C");
//             src = reg.read8(RegID_C);
//             result = executeSET(src, 0);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xC2: /* SET 0, D */
//             Log::printVerbose("SET 0, D");
//             src = reg.read8(RegID_D);
//             result = executeSET(src, 0);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xC3: /* SET 0, E */
//             Log::printVerbose("SET 0, E");
//             src = reg.read8(RegID_E);
//             result = executeSET(src, 0);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0xC4: /* SET 0, H */
//             Log::printVerbose("SET 0, H");
//             src = reg.read8(RegID_H);
//             result = executeSET(src, 0);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xC5: /* SET 0, L */
//             Log::printVerbose("SET 0, L");
//             src = reg.read8(RegID_L);
//             result = executeSET(src, 0);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xC6: /* SET 0, (HL) */
//             Log::printVerbose("SET 0, (HL)");
//             src = readMem(RegID_HL);
//             result = executeSET(src, 0);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xC7: /* SET 0, A */
//             Log::printVerbose("SET 0, A");
//             src = reg.read8(RegID_A);
//             result = executeSET(src, 0);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0xC8: /* SET 1, B */
//             Log::printVerbose("SET 1, B");
//             src = reg.read8(RegID_B);
//             result = executeSET(src, 1);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xC9: /* SET 1, C */
//             Log::printVerbose("SET 1, C");
//             src = reg.read8(RegID_C);
//             result = executeSET(src, 1);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xCA: /* SET 1, D */
//             Log::printVerbose("SET 1, D");
//             src = reg.read8(RegID_D);
//             result = executeSET(src, 1);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xCB: /* SET 1, E */
//             Log::printVerbose("SET 1, E");
//             src = reg.read8(RegID_E);
//             result = executeSET(src, 1);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0xCC: /* SET 1, H */
//             Log::printVerbose("SET 1, H");
//             src = reg.read8(RegID_H);
//             result = executeSET(src, 1);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xCD: /* SET 1, L */
//             Log::printVerbose("SET 1, L");
//             src = reg.read8(RegID_L);
//             result = executeSET(src, 1);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xCE: /* SET 1 (HL) */
//             Log::printVerbose("SET 1, (HL)");
//             src = readMem(RegID_HL);
//             result = executeSET(src, 1);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xCF: /* SET 1, A */
//             Log::printVerbose("SET 1, A");
//             src = reg.read8(RegID_A);
//             result = executeSET(src, 1);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0xD0: /* SET 2, B */
//             Log::printVerbose("SET 2, B");
//             src = reg.read8(RegID_B);
//             result = executeSET(src, 2);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xD1: /* SET 2, C */
//             Log::printVerbose("SET 2, C");
//             src = reg.read8(RegID_C);
//             result = executeSET(src, 2);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xD2: /* SET 2, D */
//             Log::printVerbose("SET 2, D");
//             src = reg.read8(RegID_D);
//             result = executeSET(src, 2);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xD3: /* SET 2, E */
//             Log::printVerbose("SET 2, E");
//             src = reg.read8(RegID_E);
//             result = executeSET(src, 2);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0xD4: /* SET 2, H */
//             Log::printVerbose("SET 2, H");
//             src = reg.read8(RegID_H);
//             result = executeSET(src, 2);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xD5: /* SET 2, L */
//             Log::printVerbose("SET 2, L");
//             src = reg.read8(RegID_L);
//             result = executeSET(src, 2);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xD6: /* SET 2, (HL) */
//             Log::printVerbose("SET 2, (HL)");
//             src = readMem(RegID_HL);
//             result = executeSET(src, 2);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xD7: /* SET 2, A */
//             Log::printVerbose("SET 2, A");
//             src = reg.read8(RegID_A);
//             result = executeSET(src, 2);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0xD8: /* SET 3, B */
//             Log::printVerbose("SET 3, B");
//             src = reg.read8(RegID_B);
//             result = executeSET(src, 3);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xD9: /* SET 3, C */
//             Log::printVerbose("SET 3, C");
//             src = reg.read8(RegID_C);
//             result = executeSET(src, 3);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xDA: /* SET 3, D */
//             Log::printVerbose("SET 3, D");
//             src = reg.read8(RegID_D);
//             result = executeSET(src, 3);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xDB: /* SET 3, E */
//             Log::printVerbose("SET 3, E");
//             src = reg.read8(RegID_E);
//             result = executeSET(src, 3);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0xDC: /* SET 3, H */
//             Log::printVerbose("SET 3, H");
//             src = reg.read8(RegID_H);
//             result = executeSET(src, 3);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xDD: /* SET 3, L */
//             Log::printVerbose("SET 3, L");
//             src = reg.read8(RegID_L);
//             result = executeSET(src, 3);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xDE: /* SET 3, (HL) */
//             Log::printVerbose("SET 3, (HL)");
//             src = readMem(RegID_HL);
//             result = executeSET(src, 3);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xDF: /* SET 3, A */
//             Log::printVerbose("SET 3, A");
//             src = reg.read8(RegID_A);
//             result = executeSET(src, 3);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0xE0: /* SET 4, B */
//             Log::printVerbose("SET 4, B");
//             src = reg.read8(RegID_B);
//             result = executeSET(src, 4);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xE1: /* SET 4, C */
//             Log::printVerbose("SET 4, C");
//             src = reg.read8(RegID_C);
//             result = executeSET(src, 4);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xE2: /* SET 4, D */
//             Log::printVerbose("SET 4, D");
//             src = reg.read8(RegID_D);
//             result = executeSET(src, 4);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xE3: /* SET 4, E */
//             Log::printVerbose("SET 4, E");
//             src = reg.read8(RegID_E);
//             result = executeSET(src, 4);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0xE4: /* SET 4, H */
//             Log::printVerbose("SET 4, H");
//             src = reg.read8(RegID_H);
//             result = executeSET(src, 4);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xE5: /* SET 4, L */
//             Log::printVerbose("SET 4, L");
//             src = reg.read8(RegID_L);
//             result = executeSET(src, 4);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xE6: /* SET 4, (HL) */
//             Log::printVerbose("SET 4, (HL)");
//             src = readMem(RegID_HL);
//             result = executeSET(src, 4);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xE7: /* SET 4, A */
//             Log::printVerbose("SET 4, A");
//             src = reg.read8(RegID_A);
//             result = executeSET(src, 4);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0xE8: /* SET 5, B */
//             Log::printVerbose("SET 5, B");
//             src = reg.read8(RegID_B);
//             result = executeSET(src, 5);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xE9: /* SET 5, C */
//             Log::printVerbose("SET 5, C");
//             src = reg.read8(RegID_C);
//             result = executeSET(src, 5);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xEA: /* SET 5, D */
//             Log::printVerbose("SET 5, D");
//             src = reg.read8(RegID_D);
//             result = executeSET(src, 5);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xEB: /* SET 5, E */
//             Log::printVerbose("SET 5, E");
//             src = reg.read8(RegID_E);
//             result = executeSET(src, 5);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0xEC: /* SET 5, H */
//             Log::printVerbose("SET 5, H");
//             src = reg.read8(RegID_H);
//             result = executeSET(src, 5);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xED: /* SET 5, L */
//             Log::printVerbose("SET 5, L");
//             src = reg.read8(RegID_L);
//             result = executeSET(src, 5);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xEE: /* SET 5, (HL) */
//             Log::printVerbose("SET 5, (HL)");
//             src = readMem(RegID_HL);
//             result = executeSET(src, 5);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xEF: /* SET 5, A */
//             Log::printVerbose("SET 5, A");
//             src = reg.read8(RegID_A);
//             result = executeSET(src, 5);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0xF0: /* SET 6, B */
//             Log::printVerbose("SET 6, B");
//             src = reg.read8(RegID_B);
//             result = executeSET(src, 6);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xF1: /* SET 6, C */
//             Log::printVerbose("SET 6, C");
//             src = reg.read8(RegID_C);
//             result = executeSET(src, 6);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xF2: /* SET 6, D */
//             Log::printVerbose("SET 6, D");
//             src = reg.read8(RegID_D);
//             result = executeSET(src, 6);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xF3: /* SET 6, E */
//             Log::printVerbose("SET 6, H");
//             src = reg.read8(RegID_H);
//             result = executeSET(src, 6);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xF4: /* SET 6, H */
//             Log::printVerbose("SET 6, H");
//             src = reg.read8(RegID_H);
//             result = executeSET(src, 6);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xF5: /* SET 6, L */
//             Log::printVerbose("SET 6, L");
//             src = reg.read8(RegID_L);
//             result = executeSET(src, 6);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xF6: /* SET 6 (HL) */
//             Log::printVerbose("SET 6, (HL)");
//             src = readMem(RegID_HL);
//             result = executeSET(src, 6);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xF7: /* SET 6, A */
//             Log::printVerbose("SET 6, A");
//             src = reg.read8(RegID_A);
//             result = executeSET(src, 6);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
//         case 0xF8: /* SET 7, B */
//             Log::printVerbose("SET 7, B");
//             src = reg.read8(RegID_B);
//             result = executeSET(src, 7);
//             reg.write8(RegID_B, result);
//             cycles = 2;
//             break;
//         case 0xF9: /* SET 7, C */
//             Log::printVerbose("SET 7, C");
//             src = reg.read8(RegID_C);
//             result = executeSET(src, 7);
//             reg.write8(RegID_C, result);
//             cycles = 2;
//             break;
//         case 0xFA: /* SET 7, D */
//             Log::printVerbose("SET 7, D");
//             src = reg.read8(RegID_D);
//             result = executeSET(src, 7);
//             reg.write8(RegID_D, result);
//             cycles = 2;
//             break;
//         case 0xFB: /* SET 7, E */
//             Log::printVerbose("SET 7, E");
//             src = reg.read8(RegID_E);
//             result = executeSET(src, 7);
//             reg.write8(RegID_E, result);
//             cycles = 2;
//             break;
//         case 0xFC: /* SET 7, H */
//             Log::printVerbose("SET 7, H");
//             src = reg.read8(RegID_H);
//             result = executeSET(src, 7);
//             reg.write8(RegID_H, result);
//             cycles = 2;
//             break;
//         case 0xFD: /* SET 7, L */
//             Log::printVerbose("SET 7, L");
//             src = reg.read8(RegID_L);
//             result = executeSET(src, 7);
//             reg.write8(RegID_L, result);
//             cycles = 2;
//             break;
//         case 0xFE: /* SET 7, (HL) */
//             Log::printVerbose("SET 7, (HL)");
//             src = readMem(RegID_HL);
//             result = executeSET(src, 7);
//             writeMem(RegID_HL, result);
//             cycles = 4;
//             break;
//         case 0xFF: /* SET 7, A */
//             Log::printVerbose("SET 7, A");
//             src = reg.read8(RegID_A);
//             result = executeSET(src, 7);
//             reg.write8(RegID_A, result);
//             cycles = 2;
//             break;
            default:
                fmt::print(stderr, "Error, unsupported prefixed opcode: cb {:02x}\n", prefixedOpcode);
                exit(EXIT_FAILURE);
                break;
    }
}
