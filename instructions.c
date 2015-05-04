#include <stdio.h>

#include "cpu.h"
#include "instructions.h"


/*
 * Data Transfer Group
 */

// MOV r1, r2 (Move Register)
int MOV(re_t to, re_t from) {
    cpu_set_re(to, cpu_get_re(from));

    return 5;
}

// MOV r, M (Move from memory)
int MOV_from_mem(re_t to) {
    uint16_t addr = cpu_get_rep(rep_hl);
    cpu.registers[to] = mem_read(cpu.mem, addr);

    return 7;
}

// MOV M, r (Move to memory)
int MOV_to_mem(re_t from) {
    uint16_t addr = cpu_get_rep(rep_hl);
    mem_write(cpu.mem, addr, cpu_get_re(from));

    return 7;
}

// MVI r, D8 (Move Immediate)
int MVI(re_t to) {
    cpu_set_re(to, cpu_get_re(re_z));

    return 7;
}

// MVI M, D8 (Move to memory immediate)
int MVI_to_mem() {
    uint16_t addr = cpu_get_rep(rep_hl);
    uint8_t byte2 = cpu_get_re(re_z);

    mem_write(cpu.mem, addr, byte2);

    return 10;
}

// LXI rp, D16 (Load register pair immediate)
int LXI(rep_t to) {
    uint16_t value = cpu_get_rep(rep_wz);
    cpu_set_rep(to, value);

    return 10;
}

int LXI_SP() {
    uint16_t value = cpu_get_rep(rep_wz);
    cpu.sp = value;

    return 10;
}

// LDA addr (Load Accumulator direct)
int LDA() {
    uint16_t addr = cpu_get_rep(rep_wz);
    uint8_t value = mem_read(cpu.mem, addr);
    cpu_set_re(re_a, value);

    return 13;
}

// STA addr (Store Accumulator direct)
int STA() {
    uint16_t addr = cpu_get_rep(rep_wz);
    mem_write(cpu.mem, addr, cpu_get_re(re_a));

    return 13;
}

// LHLD addr (Load H and L direct)
int LHLD() {
    uint16_t addr = cpu_get_rep(rep_wz);

    cpu_set_re(re_h, mem_read(cpu.mem, addr+1));
    cpu_set_re(re_l, mem_read(cpu.mem, addr));

    return 16;
}

// SHLD addr (Store H and L direct)
int SHLD() {
    uint16_t addr = cpu_get_rep(rep_wz);

    mem_write(cpu.mem, addr+1, cpu_get_re(re_h));
    mem_write(cpu.mem, addr, cpu_get_re(re_l));

    return 16;
}

// LDAX rp (Load accumulator indirect)
int LDAX(rep_t rp) {
    cpu_set_re(re_a, mem_read(cpu.mem, cpu_get_rep(rp)));

    return 7;
}

// STAX rp (Store accumulator indirect)
int STAX(rep_t rp) {
    mem_write(cpu.mem, cpu_get_rep(rp), cpu_get_re(re_a));

    return 7;
}

// XCHG (Exchange H and L with D and E)
int XCHG() {
    uint16_t temp = cpu_get_rep(rep_hl);
    cpu_set_rep(rep_hl, cpu_get_rep(rep_de));
    cpu_set_rep(rep_de, temp);

    return 4;
}



/*
 * Arithmetic Group
 */

// ADD r (Add Register)
int ADD(re_t r) {
    uint16_t result = cpu_get_re(re_a) + cpu_get_re(r);
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ALL);

    return 4;
}

// ADD M (Add memory)
int ADD_M() {
    uint16_t addr = cpu_get_rep(rep_hl);
    uint16_t result = cpu_get_re(re_a) + mem_read(cpu.mem, addr);
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// ADI D8 (Add immediate)
int ADI() {
    uint8_t byte2 = cpu_get_re(re_z);
    uint16_t result = cpu_get_re(re_a) + byte2;
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// ADC r (Add Register with carry)
int ADC(re_t r) {
    uint16_t result =
        cpu_get_re(re_a) + cpu_get_re(r) + cpu.flags.cy;
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ALL);

    return 4;

}

// SUB r (Subtract Register)
int SUB(re_t r) {
    uint16_t result = cpu_get_re(re_a) - cpu_get_re(r);
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ALL);

    return 4;
}

// SUI D8 (Add immediate)
int SUI() {
    uint8_t byte2 = cpu_get_re(re_z);
    uint16_t result = cpu_get_re(re_a) - byte2;
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

int SBI() {
    uint8_t byte2 = cpu_get_re(re_z);
    uint16_t result = cpu_get_re(re_a) - byte2 - cpu.flags.cy;
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// INR r (Increment Register)
int INR(re_t r) {
    uint16_t result = cpu_get_re(r) + 1;
    cpu_set_re(r, result);
    cpu_handle_flags(result, 8, F_ZSP | f_ac);

    return 5;
}

// INR M (Increment memory)
int INR_M() {
    uint16_t addr = cpu_get_rep(rep_hl);
    uint32_t result = mem_read(cpu.mem, addr) + 1;
    mem_write(cpu.mem, addr, result);
    cpu_handle_flags(result, 16, F_ZSP | f_ac);

    return 10;
}

// DCR r (Decrement Register)
int DCR(re_t r) {
    uint16_t result = cpu_get_re(r) - 1;
    cpu_set_re(r, result);
    cpu_handle_flags(result, 8, F_ZSP | f_ac);
    return 5;
}

// DCR M (Decrement memory)
int DCR_M() {
    uint16_t addr = cpu_get_rep(rep_hl);
    uint32_t result = mem_read(cpu.mem, addr) - 1;
    mem_write(cpu.mem, addr, result);
    cpu_handle_flags(result, 16, F_ZSP | f_ac);

    return 10;
}

// INX rp (Increment register pair)
int INX(rep_t rp) {
    cpu_set_rep(rp, cpu_get_rep(rp) + 1);

    return 5;
}

// DCX rp (Decrement register pair)
int DCX(rep_t rp) {
    cpu_set_rep(rp, cpu_get_rep(rp) - 1);

    return 5;
}

// DAD rp (Add register pair to HL)
int DAD(rep_t rp) {
    uint32_t result = cpu_get_rep(rep_hl) + cpu_get_rep(rp);
    cpu_set_rep(rep_hl, result);
    cpu_handle_flags(result, 16, f_cy);

    return 10;
}

int DAD_sp() {
    uint32_t result = cpu_get_rep(rep_hl) + cpu.sp;
    cpu_set_rep(rep_hl, result);
    cpu_handle_flags(result, 16, f_cy);

    return 10;
}

int DAA() {
    // TODO

    return 4;
}



/*
 * Logic Group
 */

// ANA r (AND Register)
int ANA(re_t r) {
    uint8_t result = cpu_get_re(re_a) & cpu_get_re(r);
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ZSP | f_ac);
    cpu.flags.cy = 0;

    return 4;
}

// ANA M (AND Memory)
int ANA_M() {
    uint8_t result =
        cpu_get_re(re_a) & mem_read(cpu.mem, cpu_get_rep(rep_hl));
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}


// ANI D8 (AND immediate)
int ANI() {
    uint8_t byte2 = cpu_get_re(re_z);
    uint8_t result = cpu_get_re(re_a) & byte2;
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}

// XRA r (Exclusive OR Register)
int XRA(re_t r) {
    uint8_t result = cpu_get_re(re_a) ^ cpu_get_re(r);
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 4;
}

// ORA r (OR Register)
int ORA(re_t r) {
    uint8_t result = cpu_get_re(re_a) | cpu_get_re(r);
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 4;
}

// ORA M (OR memory)
int ORA_M() {
    uint8_t result =
        cpu_get_re(re_a) | mem_read(cpu.mem, cpu_get_rep(rep_hl));
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}

// ORI D8 (OR immediate)
int ORI() {
    uint8_t byte2 = cpu_get_re(re_z);
    uint8_t result = cpu_get_re(re_a) | byte2;
    cpu_set_re(re_a, result);
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}

// CMP r (Compare register)
int CMP(re_t r) {
    uint16_t result = cpu_get_re(re_a) - cpu_get_re(r);
    cpu_handle_flags(result, 8, F_ALL);

    return 4;
}

// CMP M (Compare memory)
int CMP_M() {
    uint16_t result =
        cpu_get_re(re_a) - mem_read(cpu.mem, cpu_get_rep(rep_hl));
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// CPI D8 (Compare immediate)
int CPI() {
    uint8_t byte2 = cpu_get_re(re_z);
    uint16_t result = cpu_get_re(re_a) - byte2;
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// RLC (Rotate left)
int RLC() {
    uint8_t temp = cpu_get_re(re_a);
    cpu_set_re(re_a, (temp << 1) | (temp & 0x80) >> 7);
    cpu.flags.cy = (temp & 0x80) >> 7;

    return 4;
}

// RRC (Rotate Right)
int RRC() {
    uint8_t temp = cpu_get_re(re_a);
    cpu_set_re(re_a, ((temp & 1) << 7) | (temp >> 1));
    cpu.flags.cy = (temp & 1);

    return 4;
}

// RAR (Rotate right through carry)
int RAR() {
    uint8_t temp = cpu_get_re(re_a);
    cpu_set_re(re_a, (cpu.flags.cy << 7) | (temp >> 1));
    cpu.flags.cy = (temp & 1);

    return 4;
}

// CMA (Complement accumulator)
int CMA() {
    cpu_set_re(re_a, ~cpu_get_re(re_a));

    return 4;
}

// STC (Set carry)
int STC() {
    cpu.flags.cy = 1;

    return 4;
}


/*
 * Branch Group
 */

// JMP addr (Jump)
int JMP() {
    uint16_t addr = cpu_get_rep(rep_wz);
    cpu.pc = addr;

    return 10;
}

// JZ addr (Conditional jump) (Zero)
int JZ() {
    if (cpu.flags.z) { JMP(); }
    return 10;
}

// JNZ addr (Conditional jump) (Not Zero)
int JNZ() {
    if (!cpu.flags.z) { JMP(); }

    return 10;
}

// JC addr (Conditional jump) (Carry)
int JC() {
    if (cpu.flags.cy) { JMP(); }

    return 10;
}

// JNC addr (Conditional jump) (No Carry)
int JNC() {
    if (!cpu.flags.cy) { JMP(); }

    return 10;
}

// JM addr (Conditional jump) (Minus)
int JM() {
    if (cpu.flags.s) { JMP(); }

    return 10;
}

// CALL addr (Call)
int CALL() {
    uint16_t addr = cpu_get_rep(rep_wz);
    cpu_push(cpu.pc);
    cpu.pc = addr;

    return 17;
}

// CZ (Condition call) (Zero)
int CZ() {
    if (cpu.flags.z) {
        CALL();
        return 17;
    } else {
        return 11;
    }
}

// CNZ (Condition call) (Not Zero)
int CNZ() {
    if (!cpu.flags.z) {
        CALL();
        return 17;
    } else {
        return 11;
    }
}

// CNC (Condition call) (Not Carry)
int CNC() {
    if (!cpu.flags.cy) {
        CALL();
        return 17;
    } else {
        return 11;
    }
}

// RET (Return)
int RET() {
    uint16_t addr = cpu_pop(cpu);
    cpu.pc = addr;

    return 10;
}

// RZ (Conditional Return) (Zero)
int RZ() {
    if (cpu.flags.z) {
        RET();
        return 11;
    } else {
        return 5;
    }
}

// RNZ (Conditional Return) (Not Zero)
int RNZ() {
    if (!cpu.flags.z) {
        RET();
        return 11;
    } else {
        return 5;
    }
}

// RC (Conditional Return) (Carry)
int RC() {
    if (cpu.flags.cy) {
        RET();
        return 11;
    } else {
        return 5;
    }
}

// RNC (Conditional Return) (Not Carry)
int RNC() {
    if (!cpu.flags.cy) {
        RET();
        return 11;
    } else {
        return 5;
    }
}

// PCHL (Jump HL indirect, move HL to PC)
int PCHL() {
    cpu.pc = cpu_get_rep(rep_hl);

    return 5;
}



/*
 * Stack, IO and Machine Control Group
 */

// PUSH rp (Push)
int PUSH(rep_t rp) {
    cpu_push(cpu_get_rep(rp));

    return 11;
}

// PUSH PSW (Push processor status word) [Note: And accumulator]
int PUSH_PSW() {
    cpu_push(cpu_status_word());

    return 11;
}

// POP rp (Pop)
int POP(rep_t rp) {
    cpu_set_rep(rp, cpu_pop());

    return 10;
}

// POP PSW (Pop processor status word)
int POP_PSW() {
    uint16_t status_word = cpu_pop();

    cpu_set_re(re_a, status_word >> 8);

    cpu.flags.cy = status_word;
    cpu.flags.p = status_word >> 2;
    cpu.flags.ac = status_word >> 4;
    cpu.flags.z = status_word >> 6;
    cpu.flags.s = status_word >> 7;

    return 10;
}

// XTHL (Exchange stack top with H and L)
int XTHL() {
    uint16_t temp = cpu_pop();
    cpu_push(cpu_get_rep(rep_hl));
    cpu_set_rep(rep_hl, temp);

    return 18;
}

// IN port (Input)
int IN() {
    uint8_t port = cpu_get_re(re_z);
    if (port == 3) { return 10; }
    cpu_set_re(re_a, cpu.ports[port]);

    return 10;
}

// OUT port (Output)
int OUT() {
    uint8_t port = cpu_get_re(re_z);
    if (port == 2 || port == 4) { return 10; }
    cpu.ports[port] = cpu_get_re(re_a);

    return 10;
}

// EI (Enable interrupts)
int EI() {
    cpu.i = 1;

    return 4;
}
