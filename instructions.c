#include <stdio.h>

#include "cpu.h"
#include "instructions.h"


/*
 * Data Transfer Group
 */

// MOV r1, r2 (Move Register)
int MOV(cpu_t *cpu, re_t to, re_t from) {
    cpu_set_re(cpu, to, cpu_get_re(cpu, from));

    return 5;
}

// MOV r, M (Move from memory)
int MOV_from_mem(cpu_t *cpu, re_t to) {
    uint16_t addr = cpu_get_rep(cpu, rep_hl);
    cpu->registers[to] = mem_read(cpu->mem, addr);

    return 7;
}

// MOV M, r (Move to memory)
int MOV_to_mem(cpu_t *cpu, re_t from) {
    uint16_t addr = cpu_get_rep(cpu, rep_hl);
    mem_write(cpu->mem, addr, cpu_get_re(cpu, from));

    return 7;
}

// MVI r, D8 (Move Immediate)
int MVI(cpu_t *cpu, re_t to) {
    cpu_set_re(cpu, to, cpu_get_re(cpu, re_z));

    return 7;
}

// MVI M, D8 (Move to memory immediate)
int MVI_to_mem(cpu_t *cpu) {
    uint16_t addr = cpu_get_rep(cpu, rep_hl);
    uint8_t byte2 = cpu_get_re(cpu, re_z);

    mem_write(cpu->mem, addr, byte2);

    return 10;
}

// LXI rp, D16 (Load register pair immediate)
int LXI(cpu_t *cpu, rep_t to) {
    uint16_t value = cpu_get_rep(cpu, rep_wz);
    cpu_set_rep(cpu, to, value);

    return 10;
}

int LXI_SP(cpu_t *cpu) {
    uint16_t value = cpu_get_rep(cpu, rep_wz);
    cpu->sp = value;

    return 10;
}

// LDA addr (Load Accumulator direct)
int LDA(cpu_t *cpu) {
    uint16_t addr = cpu_get_rep(cpu, rep_wz);
    uint8_t value = mem_read(cpu->mem, addr);
    cpu_set_re(cpu, re_a, value);

    return 13;
}

// STA addr (Store Accumulator direct)
int STA(cpu_t *cpu) {
    uint16_t addr = cpu_get_rep(cpu, rep_wz);
    mem_write(cpu->mem, addr, cpu_get_re(cpu, re_a));

    return 13;
}

// LHLD addr (Load H and L direct)
int LHLD(cpu_t *cpu) {
    uint16_t addr = cpu_get_rep(cpu, rep_wz);

    cpu_set_re(cpu, re_h, mem_read(cpu->mem, addr+1));
    cpu_set_re(cpu, re_l, mem_read(cpu->mem, addr));

    return 16;
}

// SHLD addr (Store H and L direct)
int SHLD(cpu_t *cpu) {
    uint16_t addr = cpu_get_rep(cpu, rep_wz);

    mem_write(cpu->mem, addr+1, cpu_get_re(cpu, re_h));
    mem_write(cpu->mem, addr, cpu_get_re(cpu, re_l));

    return 16;
}

// LDAX rp (Load accumulator indirect)
int LDAX(cpu_t *cpu, rep_t rp) {
    cpu_set_re(cpu, re_a, mem_read(cpu->mem, cpu_get_rep(cpu, rp)));

    return 7;
}

// STAX rp (Store accumulator indirect)
int STAX(cpu_t *cpu, rep_t rp) {
    mem_write(cpu->mem, cpu_get_rep(cpu, rp), cpu_get_re(cpu, re_a));

    return 7;
}

// XCHG (Exchange H and L with D and E)
int XCHG(cpu_t *cpu) {
    uint16_t temp = cpu_get_rep(cpu, rep_hl);
    cpu_set_rep(cpu, rep_hl, cpu_get_rep(cpu, rep_de));
    cpu_set_rep(cpu, rep_de, temp);

    return 4;
}



/*
 * Arithmetic Group
 */

// ADD r (Add Register)
int ADD(cpu_t *cpu, re_t r) {
    uint16_t result = cpu_get_re(cpu, re_a) + cpu_get_re(cpu, r);
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ALL);

    return 4;
}

// ADD M (Add memory)
int ADD_M(cpu_t *cpu) {
    uint16_t addr = cpu_get_rep(cpu, rep_hl);
    uint16_t result = cpu_get_re(cpu, re_a) + mem_read(cpu->mem, addr);
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ALL);

    return 7;
}

// ADI D8 (Add immediate)
int ADI(cpu_t *cpu) {
    uint8_t byte2 = cpu_get_re(cpu, re_z);
    uint16_t result = cpu_get_re(cpu, re_a) + byte2;
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ALL);

    return 7;
}

// ADC r (Add Register with carry)
int ADC(cpu_t *cpu, re_t r) {
    uint16_t result =
        cpu_get_re(cpu, re_a) + cpu_get_re(cpu, r) + cpu->flags.cy;
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ALL);

    return 4;

}

// SUB r (Subtract Register)
int SUB(cpu_t *cpu, re_t r) {
    uint16_t result = cpu_get_re(cpu, re_a) - cpu_get_re(cpu, r);
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ALL);

    return 4;
}

// SUI D8 (Add immediate)
int SUI(cpu_t *cpu) {
    uint8_t byte2 = cpu_get_re(cpu, re_z);
    uint16_t result = cpu_get_re(cpu, re_a) - byte2;
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ALL);

    return 7;
}

int SBI(cpu_t *cpu) {
    uint8_t byte2 = cpu_get_re(cpu, re_z);
    uint16_t result = cpu_get_re(cpu, re_a) - byte2 - cpu->flags.cy;
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ALL);

    return 7;
}

// INR r (Increment Register)
int INR(cpu_t *cpu, re_t r) {
    uint16_t result = cpu_get_re(cpu, r) + 1;
    cpu_set_re(cpu, r, result);
    cpu_handle_flags(cpu, result, 8, F_ZSP | f_ac);

    return 5;
}

// INR M (Increment memory)
int INR_M(cpu_t *cpu) {
    uint16_t addr = cpu_get_rep(cpu, rep_hl);
    uint32_t result = mem_read(cpu->mem, addr) + 1;
    mem_write(cpu->mem, addr, result);
    cpu_handle_flags(cpu, result, 16, F_ZSP | f_ac);

    return 10;
}

// DCR r (Decrement Register)
int DCR(cpu_t *cpu, re_t r) {
    uint16_t result = cpu_get_re(cpu, r) - 1;
    cpu_set_re(cpu, r, result);
    cpu_handle_flags(cpu, result, 8, F_ZSP | f_ac);
    return 5;
}

// DCR M (Decrement memory)
int DCR_M(cpu_t *cpu) {
    uint16_t addr = cpu_get_rep(cpu, rep_hl);
    uint32_t result = mem_read(cpu->mem, addr) - 1;
    mem_write(cpu->mem, addr, result);
    cpu_handle_flags(cpu, result, 16, F_ZSP | f_ac);

    return 10;
}

// INX rp (Increment register pair)
int INX(cpu_t *cpu, rep_t rp) {
    cpu_set_rep(cpu, rp, cpu_get_rep(cpu, rp) + 1);

    return 5;
}

// DCX rp (Decrement register pair)
int DCX(cpu_t *cpu, rep_t rp) {
    cpu_set_rep(cpu, rp, cpu_get_rep(cpu, rp) - 1);

    return 5;
}

// DAD rp (Add register pair to HL)
int DAD(cpu_t *cpu, rep_t rp) {
    uint32_t result = cpu_get_rep(cpu, rep_hl) + cpu_get_rep(cpu, rp);
    cpu_set_rep(cpu, rep_hl, result);
    cpu_handle_flags(cpu, result, 16, f_cy);

    return 10;
}

int DAD_sp(cpu_t *cpu) {
    uint32_t result = cpu_get_rep(cpu, rep_hl) + cpu->sp;
    cpu_set_rep(cpu, rep_hl, result);
    cpu_handle_flags(cpu, result, 16, f_cy);

    return 10;
}

int DAA(cpu_t *cpu) {
    uint8_t low_nyble = cpu_get_re(cpu, re_a) & 0xF;
    if (low_nyble > 9 || cpu->flags.ac) {
    }
    // TODO

    return 4;
}



/*
 * Logic Group
 */

// ANA r (AND Register)
int ANA(cpu_t *cpu, re_t r) {
    uint8_t result = cpu_get_re(cpu, re_a) & cpu_get_re(cpu, r);
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ZSP | f_ac);
    cpu->flags.cy = 0;

    return 4;
}

// ANA M (AND Memory)
int ANA_M(cpu_t *cpu) {
    uint8_t result =
        cpu_get_re(cpu, re_a) & mem_read(cpu->mem, cpu_get_rep(cpu, rep_hl));
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ZSP);
    cpu->flags.cy = cpu->flags.ac = 0;

    return 7;
}


// ANI D8 (AND immediate)
int ANI(cpu_t *cpu) {
    uint8_t byte2 = cpu_get_re(cpu, re_z);
    uint8_t result = cpu_get_re(cpu, re_a) & byte2;
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ZSP);
    cpu->flags.cy = cpu->flags.ac = 0;

    return 7;
}

// XRA r (Exclusive OR Register)
int XRA(cpu_t *cpu, re_t r) {
    uint8_t result = cpu_get_re(cpu, re_a) ^ cpu_get_re(cpu, r);
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ZSP);
    cpu->flags.cy = cpu->flags.ac = 0;

    return 4;
}

// ORA r (OR Register)
int ORA(cpu_t *cpu, re_t r) {
    uint8_t result = cpu_get_re(cpu, re_a) | cpu_get_re(cpu, r);
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ZSP);
    cpu->flags.cy = cpu->flags.ac = 0;

    return 4;
}

// ORA M (OR memory)
int ORA_M(cpu_t *cpu) {
    uint8_t result =
        cpu_get_re(cpu, re_a) | mem_read(cpu->mem, cpu_get_rep(cpu, rep_hl));
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ZSP);
    cpu->flags.cy = cpu->flags.ac = 0;

    return 7;
}

// ORI D8 (OR immediate)
int ORI(cpu_t *cpu) {
    uint8_t byte2 = cpu_get_re(cpu, re_z);
    uint8_t result = cpu_get_re(cpu, re_a) | byte2;
    cpu_set_re(cpu, re_a, result);
    cpu_handle_flags(cpu, result, 8, F_ZSP);
    cpu->flags.cy = cpu->flags.ac = 0;

    return 7;
}

// CMP r (Compare register)
int CMP(cpu_t *cpu, re_t r) {
    uint16_t result = cpu_get_re(cpu, re_a) - cpu_get_re(cpu, r);
    cpu_handle_flags(cpu, result, 8, F_ALL);

    return 4;
}

// CMP M (Compare memory)
int CMP_M(cpu_t *cpu) {
    uint16_t result =
        cpu_get_re(cpu, re_a) - mem_read(cpu->mem, cpu_get_rep(cpu, rep_hl));
    cpu_handle_flags(cpu, result, 8, F_ALL);

    return 7;
}

// CPI D8 (Compare immediate)
int CPI(cpu_t *cpu) {
    uint8_t byte2 = cpu_get_re(cpu, re_z);
    uint16_t result = cpu_get_re(cpu, re_a) - byte2;
    cpu_handle_flags(cpu, result, 8, F_ALL);

    return 7;
}

// RLC (Rotate left)
int RLC(cpu_t *cpu) {
    uint8_t temp = cpu_get_re(cpu, re_a);
    cpu_set_re(cpu, re_a, (temp << 1) | (temp & 0x80) >> 7);
    cpu->flags.cy = (temp & 0x80) >> 7;

    return 4;
}

// RRC (Rotate Right)
int RRC(cpu_t *cpu) {
    uint8_t temp = cpu_get_re(cpu, re_a);
    cpu_set_re(cpu, re_a, ((temp & 1) << 7) | (temp >> 1));
    cpu->flags.cy = (temp & 1);

    return 4;
}

// RAR (Rotate right through carry)
int RAR(cpu_t *cpu) {
    uint8_t temp = cpu_get_re(cpu, re_a);
    cpu_set_re(cpu, re_a, (cpu->flags.cy << 7) | (temp >> 1));
    cpu->flags.cy = (temp & 1);

    return 4;
}

// CMA (Complement accumulator)
int CMA(cpu_t *cpu) {
    cpu_set_re(cpu, re_a, ~cpu_get_re(cpu, re_a));

    return 4;
}

// STC (Set carry)
int STC(cpu_t *cpu) {
    cpu->flags.cy = 1;

    return 4;
}


/*
 * Branch Group
 */

// JMP addr (Jump)
int JMP(cpu_t *cpu) {
    uint16_t addr = cpu_get_rep(cpu, rep_wz);
    cpu->pc = addr;

    return 10;
}

// JZ addr (Conditional jump) (Zero)
int JZ(cpu_t *cpu) {
    if (cpu->flags.z) { JMP(cpu); }
    return 10;
}

// JNZ addr (Conditional jump) (Not Zero)
int JNZ(cpu_t *cpu) {
    if (!cpu->flags.z) { JMP(cpu); }

    return 10;
}

// JC addr (Conditional jump) (Carry)
int JC(cpu_t *cpu) {
    if (cpu->flags.cy) { JMP(cpu); }

    return 10;
}

// JNC addr (Conditional jump) (No Carry)
int JNC(cpu_t *cpu) {
    if (!cpu->flags.cy) { JMP(cpu); }

    return 10;
}

// JM addr (Conditional jump) (Minus)
int JM(cpu_t *cpu) {
    if (cpu->flags.s) { JMP(cpu); }

    return 10;
}

// CALL addr (Call)
int CALL(cpu_t *cpu) {
    uint16_t addr = cpu_get_rep(cpu, rep_wz);
    cpu_push(cpu, cpu->pc);
    cpu->pc = addr;

    return 17;
}

// CZ (Condition call) (Zero)
int CZ(cpu_t *cpu) {
    if (cpu->flags.z) {
        CALL(cpu);
        return 17;
    } else {
        return 11;
    }
}

// CNZ (Condition call) (Not Zero)
int CNZ(cpu_t *cpu) {
    if (!cpu->flags.z) {
        CALL(cpu);
        return 17;
    } else {
        return 11;
    }
}

// CNC (Condition call) (Not Carry)
int CNC(cpu_t *cpu) {
    if (!cpu->flags.cy) {
        CALL(cpu);
        return 17;
    } else {
        return 11;
    }
}

// RET (Return)
int RET(cpu_t *cpu) {
    uint16_t addr = cpu_pop(cpu);
    cpu->pc = addr;

    return 10;
}

// RZ (Conditional Return) (Zero)
int RZ(cpu_t *cpu) {
    if (cpu->flags.z) {
        RET(cpu);
        return 11;
    } else {
        return 5;
    }
}

// RNZ (Conditional Return) (Not Zero)
int RNZ(cpu_t *cpu) {
    if (!cpu->flags.z) {
        RET(cpu);
        return 11;
    } else {
        return 5;
    }
}

// RC (Conditional Return) (Carry)
int RC(cpu_t *cpu) {
    if (cpu->flags.cy) {
        RET(cpu);
        return 11;
    } else {
        return 5;
    }
}

// RNC (Conditional Return) (Not Carry)
int RNC(cpu_t *cpu) {
    if (!cpu->flags.cy) {
        RET(cpu);
        return 11;
    } else {
        return 5;
    }
}

// PCHL (Jump HL indirect, move HL to PC)
int PCHL(cpu_t *cpu) {
    cpu->pc = cpu_get_rep(cpu, rep_hl);

    return 5;
}



/*
 * Stack, IO and Machine Control Group
 */

// PUSH rp (Push)
int PUSH(cpu_t *cpu, rep_t rp) {
    cpu_push(cpu, cpu_get_rep(cpu, rp));

    return 11;
}

// PUSH PSW (Push processor status word) [Note: And accumulator]
int PUSH_PSW(cpu_t *cpu) {
    cpu_push(cpu, cpu_status_word(cpu));

    return 11;
}

// POP rp (Pop)
int POP(cpu_t *cpu, rep_t rp) {
    cpu_set_rep(cpu, rp, cpu_pop(cpu));

    return 10;
}

// POP PSW (Pop processor status word)
int POP_PSW(cpu_t *cpu) {
    uint16_t status_word = cpu_pop(cpu);

    cpu_set_re(cpu, re_a, status_word >> 8);

    cpu->flags.cy = status_word;
    cpu->flags.p = status_word >> 2;
    cpu->flags.ac = status_word >> 4;
    cpu->flags.z = status_word >> 6;
    cpu->flags.s = status_word >> 7;

    return 10;
}

// XTHL (Exchange stack top with H and L)
int XTHL(cpu_t *cpu) {
    uint16_t temp = cpu_pop(cpu);
    cpu_push(cpu, cpu_get_rep(cpu, rep_hl));
    cpu_set_rep(cpu, rep_hl, temp);

    return 18;
}

// IN port (Input)
int IN(cpu_t *cpu) {
    uint8_t port = cpu_get_re(cpu, re_z);
    if (port == 3) { return 10; }
    cpu_set_re(cpu, re_a, cpu->ports[port]);

    return 10;
}

// OUT port (Output)
int OUT(cpu_t *cpu) {
    uint8_t port = cpu_get_re(cpu, re_z);
    if (port == 2 || port == 4) { return 10; }
    cpu->ports[port] = cpu_get_re(cpu, re_a);

    return 10;
}

// EI (Enable interrupts)
int EI(cpu_t *cpu) {
    cpu->i = 1;

    return 4;
}
