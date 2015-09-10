#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"

struct cpu cpu; // Global


/*
 * Data Transfer Group
 */

// MOV r1, r2 (Move register to register)
static int MOV(uint8_t *dest, const uint8_t *src) {
    *dest = *src;

    return 5;
}

// MOV r, M (Move memory to register)
static int MOV_from_mem(uint8_t *src) {
    *src = mem_read(cpu.mem, cpu.hl);

    return 7;
}

// MOV M, r (Move register to memory)
static int MOV_to_mem(const uint8_t *src) {
    mem_write(cpu.mem, cpu.hl, *src);

    return 7;
}

// MVI r, D8 (Move immediate register)
static int MVI(uint8_t *dest) {
    cpu_read_byte_to_z();

    *dest = cpu.z;

    return 7;
}

// MVI M, D8 (Move to memory immediate)
static int MVI_to_mem() {
    cpu_read_byte_to_z();

    mem_write(cpu.mem, cpu.hl, cpu.z);

    return 10;
}

// LXI rp, D16 (Load register pair immediate)
static int LXI(uint16_t *dest) {
    cpu_read_bytes_to_wz();

    *dest = cpu.wz;

    return 10;
}

// LDA addr (Load accumulator direct)
static int LDA() {
    cpu_read_bytes_to_wz();

    cpu.a = mem_read(cpu.mem, cpu.wz);

    return 13;
}

// STA addr (Store Accumulator direct)
static int STA() {
    cpu_read_bytes_to_wz();

    mem_write(cpu.mem, cpu.wz, cpu.a);

    return 13;
}

// LHLD addr (Load H and L direct)
static int LHLD() {
    cpu_read_bytes_to_wz();

    cpu.h =  mem_read(cpu.mem, cpu.wz+1);
    cpu.l =  mem_read(cpu.mem, cpu.wz);

    return 16;
}

// SHLD addr (Store H and L direct)
static int SHLD() {
    cpu_read_bytes_to_wz();

    mem_write(cpu.mem, cpu.wz+1, cpu.h);
    mem_write(cpu.mem, cpu.wz, cpu.l);

    return 16;
}

// LDAX B (Load accumulator indirect)
static int LDAX_B() {
    cpu.a = mem_read(cpu.mem, cpu.bc);

    return 7;
}

// LDAX D (Load accumulator indirect)
static int LDAX_D() {
    cpu.a = mem_read(cpu.mem, cpu.de);

    return 7;
}

// STAX B (Store accumulator indirect)
static int STAX_B() {
    mem_write(cpu.mem, cpu.bc, cpu.a);

    return 7;
}

// STAX D (Store accumulator indirect)
static int STAX_D() {
    mem_write(cpu.mem, cpu.de, cpu.a);

    return 7;
}

// XCHG (Exchange H and L with D and E)
static int XCHG() {
    // TODO Watch out!
    cpu.hl ^= cpu.de; cpu.de ^= cpu.hl; cpu.hl ^= cpu.de;

    return 4;
}



/*
 * Arithmetic Group
 */

// ADD r (Add Register)
static int ADD(uint8_t *r) {
    uint16_t result = cpu.a + *r;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 4;
}

// ADD M (Add memory)
static int ADD_M() {
    uint16_t result = cpu.a + mem_read(cpu.mem, cpu.hl);
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// ADI D8 (Add immediate)
static int ADI() {
    cpu_read_byte_to_z();
    uint16_t result = cpu.a + cpu.z;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// ADC r (Add Register with carry)
static int ADC(uint8_t *r) {
    uint16_t result = cpu.a + *r + cpu.flags.cy;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 4;

}

// SUB r (Subtract Register)
static int SUB(uint8_t *r) {
    uint16_t result = cpu.a - *r;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 4;
}

// SUI D8 (Add immediate)
static int SUI() {
    cpu_read_byte_to_z();
    uint16_t result = cpu.a - cpu.z;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// Subtract immediate with borrow
static int SBI() {
    cpu_read_byte_to_z();
    uint16_t result = cpu.a - cpu.z - cpu.flags.cy;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// INR r (Increment Register)
static int INR(uint8_t *r) {
    uint16_t result = *r + 1;
    *r = result;
    cpu_handle_flags(result, 8, F_ZSP | f_ac);

    return 5;
}

// INR M (Increment memory)
static int INR_M() {
    uint32_t result = mem_read(cpu.mem, cpu.hl) + 1;
    mem_write(cpu.mem, cpu.hl, result);
    cpu_handle_flags(result, 16, F_ZSP | f_ac);

    return 10;
}

// DCR r (Decrement Register)
static int DCR(uint8_t *r) {
    uint16_t result = *r - 1;
    *r = result;
    cpu_handle_flags(result, 8, F_ZSP | f_ac);
    return 5;
}

// DCR M (Decrement memory)
static int DCR_M() {
    uint32_t result = mem_read(cpu.mem, cpu.hl) - 1;
    mem_write(cpu.mem, cpu.hl, result);
    cpu_handle_flags(result, 16, F_ZSP | f_ac);

    return 10;
}

// INX rp (Increment register pair)
static int INX(uint16_t *rp) {
    (*rp)++;

    return 5;
}

// DCX rp (Decrement register pair)
static int DCX(uint16_t *rp) {
    (*rp)--;

    return 5;
}

// DAD rp (Add register pair to HL)
static int DAD(uint16_t *rp) {
    uint32_t result = cpu.hl + *rp;
    cpu.hl = result;
    cpu_handle_flags(result, 16, f_cy);

    return 10;
}

// Decimal adjust accumulator
static int DAA() {
    uint8_t low_nibble = cpu.a & 0x0F;
    uint8_t high_nibble = (cpu.a & 0xF0) >> 4;

    if (low_nibble > 9 || cpu.flags.ac) {
        low_nibble += 6;
        cpu.flags.ac = 1;
    }

    if (high_nibble > 9 || cpu.flags.cy) {
        high_nibble += 6;
        cpu.flags.cy = 1;
    }

    cpu.a = high_nibble << 4 | low_nibble;

    return 4;
}



/*
 * Logic Group
 */

// ANA r (AND Register)
static int ANA(uint8_t *r) {
    uint8_t result = cpu.a & *r;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP | f_ac);
    cpu.flags.cy = 0;

    return 4;
}

// ANA M (AND Memory)
static int ANA_M() {
    uint8_t result = cpu.a & mem_read(cpu.mem, cpu.hl);
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}


// ANI D8 (AND immediate)
static int ANI() {
    cpu_read_byte_to_z();
    uint8_t result = cpu.a & cpu.z;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}

// XRA r (Exclusive OR Register)
static int XRA(uint8_t *r) {
    uint8_t result = cpu.a ^ *r;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 4;
}

// ORA r (OR Register)
static int ORA(uint8_t *r) {
    uint8_t result = cpu.a | *r;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 4;
}

// ORA M (OR memory)
static int ORA_M() {
    uint8_t result = cpu.a | mem_read(cpu.mem, cpu.hl);
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}

// ORI D8 (OR immediate)
static int ORI() {
    cpu_read_byte_to_z();
    uint8_t result = cpu.a | cpu.z;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}

// CMP r (Compare register)
static int CMP(uint8_t *r) {
    uint16_t result = cpu.a - *r;
    cpu_handle_flags(result, 8, F_ALL);

    return 4;
}

// CMP M (Compare memory)
static int CMP_M() {
    uint16_t result = cpu.a - mem_read(cpu.mem, cpu.hl);
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// CPI D8 (Compare immediate)
static int CPI() {
    cpu_read_byte_to_z();
    uint16_t result = cpu.a - cpu.z;
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// RLC (Rotate left)
static int RLC() {
    uint8_t temp = cpu.a;
    cpu.a = (temp << 1) | (temp & 0x80) >> 7;
    cpu.flags.cy = (temp & 0x80) >> 7;

    return 4;
}

// RRC (Rotate Right)
static int RRC() {
    uint8_t temp = cpu.a;
    cpu.a = ((temp & 1) << 7) | (temp >> 1);
    cpu.flags.cy = (temp & 1);

    return 4;
}

// RAR (Rotate right through carry)
static int RAR() {
    uint8_t temp = cpu.a;
    cpu.a = (cpu.flags.cy << 7) | (temp >> 1);
    cpu.flags.cy = (temp & 1);

    return 4;
}

// CMA (Complement accumulator)
static int CMA() {
    cpu.a = ~cpu.a;

    return 4;
}

// STC (Set carry)
static int STC() {
    cpu.flags.cy = 1;

    return 4;
}


/*
 * Branch Group
 */

// JMP addr (Jump)
static int JMP() {
    cpu.pc = cpu.wz;

    return 10;
}

// JZ addr (Conditional jump) (Zero)
static int JZ() {
    if (cpu.flags.z) { JMP(); }
    return 10;
}

// JNZ addr (Conditional jump) (Not Zero)
static int JNZ() {
    if (!cpu.flags.z) { JMP(); }

    return 10;
}

// JC addr (Conditional jump) (Carry)
static int JC() {
    if (cpu.flags.cy) { JMP(); }

    return 10;
}

// JNC addr (Conditional jump) (No Carry)
static int JNC() {
    if (!cpu.flags.cy) { JMP(); }

    return 10;
}

// JM addr (Conditional jump) (Minus)
static int JM() {
    if (cpu.flags.s) { JMP(); }

    return 10;
}

// CALL addr (Call)
static int CALL() {
    cpu_push(cpu.pc);
    cpu.pc = cpu.wz;

    return 17;
}

// CZ (Condition call) (Zero)
static int CZ() {
    if (cpu.flags.z) {
        CALL();
        return 17;
    } else {
        return 11;
    }
}

// CNZ (Condition call) (Not Zero)
static int CNZ() {
    if (!cpu.flags.z) {
        CALL();
        return 17;
    } else {
        return 11;
    }
}

// CNC (Condition call) (Not Carry)
static int CNC() {
    if (!cpu.flags.cy) {
        CALL();
        return 17;
    } else {
        return 11;
    }
}

// RET (Return)
static int RET() {
    cpu.pc = cpu_pop();

    return 10;
}

// RZ (Conditional Return) (Zero)
static int RZ() {
    if (cpu.flags.z) {
        RET();
        return 11;
    } else {
        return 5;
    }
}

// RNZ (Conditional Return) (Not Zero)
static int RNZ() {
    if (!cpu.flags.z) {
        RET();
        return 11;
    } else {
        return 5;
    }
}

// RC (Conditional Return) (Carry)
static int RC() {
    if (cpu.flags.cy) {
        RET();
        return 11;
    } else {
        return 5;
    }
}

// RNC (Conditional Return) (Not Carry)
static int RNC() {
    if (!cpu.flags.cy) {
        RET();
        return 11;
    } else {
        return 5;
    }
}

// PCHL (Jump HL indirect, move HL to PC)
static int PCHL() {
    cpu.pc = cpu.hl;

    return 5;
}



/*
 * Stack, IO and Machine Control Group
 */

// PUSH B (Push)
static int PUSH_B() {
    cpu_push(cpu.bc);

    return 11;
}

// PUSH D (Push)
static int PUSH_D() {
    cpu_push(cpu.de);

    return 11;
}

// PUSH H (Push)
static int PUSH_H() {
    cpu_push(cpu.hl);

    return 11;
}

// PUSH PSW (Push processor status word) [Note: And accumulator]
static int PUSH_PSW() {
    cpu_push(cpu.af);

    return 11;
}

// POP B (Pop)
static int POP_B() {
    cpu.bc = cpu_pop();

    return 10;
}

// POP D (Pop)
static int POP_D() {
    cpu.de = cpu_pop();

    return 10;
}

// POP H (Pop)
static int POP_H() {
    cpu.hl = cpu_pop();

    return 10;
}

// POP PSW (Pop processor status word)
static int POP_PSW() {
    cpu.af = cpu_pop();

    return 10;
}

// XTHL (Exchange stack top with H and L)
static int XTHL() {
    uint16_t temp = cpu_pop();
    cpu_push(cpu.hl);
    cpu.hl = temp;

    return 18;
}

// IN port (Input)
static int IN() {
    cpu_read_byte_to_z();
    uint8_t port = cpu.z;
    if (port == 3) { return 10; }
    cpu.a = cpu.ports[port];

    return 10;
}

// OUT port (Output)
static int OUT() {
    cpu_read_byte_to_z();
    uint8_t port = cpu.z;
    if (port == 2 || port == 4) { return 10; }
    cpu.ports[port] = cpu.a;

    return 10;
}

// EI (Enable interrupts)
static int EI() {
    cpu.flags.i = 1;

    return 4;
}

static int NOP() {
    return 4;
}


void cpu_dump() {
    printf("IR:   0x%02x  ", cpu.ir);
    printf("PC: 0x%04x  ", cpu.pc);
    printf("SP: 0x%04x\n", cpu.sp);
    printf("BC: 0x%04x  ", cpu.bc);
    printf("DE: 0x%04x  ", cpu.de);
    printf("HL: 0x%04x  ", cpu.hl);
    printf("AF: 0x%04x\n", cpu.af);
}


void cpu_fetch() {
    cpu.ir = mem_read(cpu.mem, cpu.pc);
    cpu.pc++;
}


void cpu_read_bytes_to_wz() {
    cpu.z = mem_read(cpu.mem, cpu.pc);
    cpu.w = mem_read(cpu.mem, cpu.pc+1);

    cpu.pc += 2;
}


void cpu_read_byte_to_z() {
    cpu.z = mem_read(cpu.mem, cpu.pc);

    cpu.pc++;
}



void cpu_handle_flags(uint32_t result, size_t size, int flags) {
    if (flags & f_z) {
        // TODO Will be wrong if result has overflowed
        cpu.flags.z = (result == 0);
    }

    if (flags & f_p) {
        cpu.flags.p = (result & 1) == 0;
    }

    if (flags & f_s) {
        cpu.flags.s = (result & (1 << (size-1))) >> (size-1) == 1;
    }

    if (flags & f_ac) {
        cpu.flags.ac = 0;
    }

    if (flags & f_cy) {
        cpu.flags.cy = (result & ((0xfffffff) << size)) != 0;
    }
}

void cpu_push(uint16_t value) {
    mem_write(cpu.mem, cpu.sp-1, value >> 8);
    mem_write(cpu.mem, cpu.sp-2, value);

    cpu.sp -= 2;
}

uint16_t cpu_pop() {
    uint16_t r = (mem_read(cpu.mem, cpu.sp+1) << 8) | mem_read(cpu.mem, cpu.sp);

    cpu.sp += 2;

    return r;
}


int cpu_run_instruction() {
    switch (cpu.ir) {
        case 0x00:  // NOP
            return NOP();

        case 0x40:  // MOV B, B
            return MOV(&cpu.b, &cpu.b);
        case 0x41:  // MOV B, C
            return MOV(&cpu.b, &cpu.c);
        case 0x42:  // MOV B, D
            return MOV(&cpu.b, &cpu.d);
        case 0x43:  // MOV B, E
            return MOV(&cpu.b, &cpu.e);
        case 0x44:  // MOV B, H
            return MOV(&cpu.b, &cpu.h);
        case 0x47:  // MOV B, A
            return MOV(&cpu.b, &cpu.a);
        case 0x48:  // MOV C, B
            return MOV(&cpu.c, &cpu.b);
        case 0x4f:  // MOV C, A
            return MOV(&cpu.c, &cpu.a);
        case 0x57:  // MOV D, A
            return MOV(&cpu.d, &cpu.a);
        case 0x5f:  // MOV E, A
            return MOV(&cpu.e, &cpu.a);
        case 0x61:  // MOV H, C
            return MOV(&cpu.h, &cpu.c);
        case 0x64:  // MOV H, H
            return MOV(&cpu.h, &cpu.h);
        case 0x65:  // MOV H, L
            return MOV(&cpu.h, &cpu.l);
        case 0x67:  // MOV H, A
            return MOV(&cpu.h, &cpu.a);
        case 0x68:  // MOV L, B
            return MOV(&cpu.l, &cpu.b);
        case 0x69:  // MOV L, C
            return MOV(&cpu.l, &cpu.c);
        case 0x6f:  // MOV L, A
            return MOV(&cpu.l, &cpu.a);
        case 0x78:  // MOV A, B
            return MOV(&cpu.a, &cpu.b);
        case 0x79:  // MOV A, C
            return MOV(&cpu.a, &cpu.c);
        case 0x7a:  // MOV A, D
            return MOV(&cpu.a, &cpu.d);
        case 0x7b:  // MOV A, E
            return MOV(&cpu.a, &cpu.e);
        case 0x7c:  // MOV A, H
            return MOV(&cpu.a, &cpu.h);
        case 0x7d:  // MOV A, L
            return MOV(&cpu.a, &cpu.l);

        case 0x46:  // MOV B, M
            return MOV_from_mem(&cpu.b);
        case 0x4e:  // MOV C, M
            return MOV_from_mem(&cpu.c);
        case 0x5e:  // MOV E, M
            return MOV_from_mem(&cpu.e);
        case 0x7e:  // MOV A, M
            return MOV_from_mem(&cpu.a);
        case 0x56:  // MOV D, M
            return MOV_from_mem(&cpu.d);
        case 0x66:  // MOV H, M
            return MOV_from_mem(&cpu.h);

        case 0x70:  // MOV M, B
            return MOV_to_mem(&cpu.b);
        case 0x71:  // MOV M, C
            return MOV_to_mem(&cpu.c);
        case 0x72:  // MOV M, D
            return MOV_to_mem(&cpu.d);
        case 0x73:  // MOV M, E
            return MOV_to_mem(&cpu.e);
        case 0x77:  // MOV M, A
            return MOV_to_mem(&cpu.a);

        case 0x01:  // LXI B, D16
            return LXI(&cpu.bc);
        case 0x11:  // LXI D, D16
            return LXI(&cpu.de);
        case 0x21:  // LXI H, D16
            return LXI(&cpu.hl);
        case 0x31:  // LXI SP, D16
            return LXI(&cpu.sp);

        case 0x06:  // MVI B, D8
            return MVI(&cpu.b);
        case 0x16:  // MVI D, D8
            return MVI(&cpu.d);
        case 0x0e:  // MVI C, D8
            return MVI(&cpu.c);
        case 0x1e:  // MVI E, D8
            return MVI(&cpu.e);
        case 0x26:  // MVI H, D8
            return MVI(&cpu.h);
        case 0x2e:  // MVI L, D8
            return MVI(&cpu.l);
        case 0x3e:  // MVI A, D8
            return MVI(&cpu.a);

        case 0x36:  // MVI M, D8
            return MVI_to_mem();

        case 0x0a:  // LDAX B
            return LDAX_B();
        case 0x1a:  // LDAX D
            return LDAX_D();

        case 0x3a:  // LDA D16
            return LDA();

        case 0x32:  // STA D16
            return STA();

        case 0x02:  // STAX B
            return STAX_B();
        case 0x12:  // STAX D
            return STAX_D();

        case 0x2a:  // LHLD addr
            return LHLD();

        case 0x22:  // SHLD addr
            return SHLD();

        case 0xeb:  // XCHG
            return XCHG();

        case 0x80: // ADD B
            return ADD(&cpu.b);
        case 0x81: // ADD C
            return ADD(&cpu.c);
        case 0x82: // ADD D
            return ADD(&cpu.d);
        case 0x83: // ADD E
            return ADD(&cpu.e);
        case 0x85: // ADD L
            return ADD(&cpu.l);

        case 0x86: // ADD M
            return ADD_M();

        case 0x04: // INR B
            return INR(&cpu.b);
        case 0x0c: // INR C
            return INR(&cpu.c);
        case 0x14: // INR D
            return INR(&cpu.d);
        case 0x1c: // INR E
            return INR(&cpu.e);
        case 0x24: // INR H
            return INR(&cpu.h);
        case 0x2c: // INR L
            return INR(&cpu.l);
        case 0x3c: // INR A
            return INR(&cpu.a);

        case 0x34: // INR M
            return INR_M();

        case 0x05: // DCR B
            return DCR(&cpu.b);
        case 0x0d: // DCR C
            return DCR(&cpu.c);
        case 0x15: // DCR D
            return DCR(&cpu.d);
        case 0x25: // DCR H
            return DCR(&cpu.h);
        case 0x3d: // DCR A
            return DCR(&cpu.a);

        case 0x35: // DCR M
            return DCR_M();

        case 0x03: // INX B
            return INX(&cpu.bc);
        case 0x13: // INX D
            return INX(&cpu.de);
        case 0x23: // INX H
            return INX(&cpu.hl);

        case 0x1b: // DCX D
            return DCX(&cpu.de);
        case 0x2b: // DCX H
            return DCX(&cpu.hl);

        case 0x09: // DAD B
            return DAD(&cpu.bc);
        case 0x19: // DAD D
            return DAD(&cpu.de);
        case 0x29: // DAD H
            return DAD(&cpu.hl);

        case 0xc6: // ADI D8
            return ADI();

        case 0x8a: // ADC D
            return ADC(&cpu.d);

        case 0x97: // SUB A
            return SUB(&cpu.a);

        case 0xd6: // SUI D8
            return SUI();

        case 0xde: // SBI D8
            return SBI();

        case 0x27: // DAA
            return DAA();

        case 0xa0: // ANA B
            return ANA(&cpu.b);
        case 0xa1: // ANA C
            return ANA(&cpu.c);
        case 0xa7: // ANA A
            return ANA(&cpu.a);

        case 0xa6: // ANA M
            return ANA_M();

        case 0xe6: // ANI D8
            return ANI();

        case 0xa8: // XRA B
            return XRA(&cpu.b);
        case 0xaf: // XRA A
            return XRA(&cpu.a);

        case 0xb0: // ORA B
            return ORA(&cpu.b);
        case 0xb4: // ORA H
            return ORA(&cpu.h);

        case 0xb6: // ORA M
            return ORA_M();

        case 0xf6: // ORI D8
            return ORI();

        case 0xb8: // CMP B
            return CMP(&cpu.b);
        case 0xbc: // CMP H
            return CMP(&cpu.h);

        case 0xbe: // CMP M
            return CMP_M();

        case 0xfe: // CPI D8
            return CPI();

        case 0x07: // RLC
            return RLC();
        case 0x0f: // RRC
            return RRC();
        case 0x1f: // RAR
            return RAR();

        case 0x2f: // CMA
            return CMA();
        case 0x37: // STC
            return STC();

        case 0xc3:  // JMP addr
            cpu_read_bytes_to_wz();
            return JMP();
        case 0xca:  // JZ addr
            cpu_read_bytes_to_wz();
            return JZ();
        case 0xc2:  // JNZ addr
            cpu_read_bytes_to_wz();
            return JNZ();
        case 0xda:  // JC addr
            cpu_read_bytes_to_wz();
            return JC();
        case 0xd2:  // JNC addr
            cpu_read_bytes_to_wz();
            return JNC();
        case 0xfa:  // JM addr
            cpu_read_bytes_to_wz();
            return JM();

        case 0xcd:  // CALL addr
            cpu_read_bytes_to_wz();
            return CALL();
        case 0xcc:  // CZ addr
            cpu_read_bytes_to_wz();
            return CZ();
        case 0xc4:  // CNZ addr
            cpu_read_bytes_to_wz();
            return CNZ();
        case 0xd4:  // CNC addr
            cpu_read_bytes_to_wz();
            return CNC();

        case 0xc9:  // RET
            return RET();
        case 0xc8:  // RZ
            return RZ();
        case 0xc0:  // RNZ
            return RNZ();
        case 0xd8:  // RC
            return RC();
        case 0xd0:  // RNC
            return RNC();

        case 0xe9:  // PCHL
            return PCHL();

        case 0xc5:  // PUSH B
            return PUSH_B();
        case 0xd5:  // PUSH D
            return PUSH_D();
        case 0xe5:  // PUSH H
            return PUSH_H();

        case 0xf5:  // PUSH PSW
            return PUSH_PSW();

        case 0xc1:  // POP B
            return POP_B();
        case 0xd1:  // POP D
            return POP_D();
        case 0xe1:  // POP H
            return POP_H();

        case 0xf1:  // POP PSW
            return POP_PSW();
        case 0xe3:  // XTHL
            return XTHL();

        case 0xdb:  // IN D8
            return IN();
        case 0xd3:  // OUT D8
            return OUT();

        case 0xfb:  // EI
            return EI();

        default:
            return 0;
    }
}
