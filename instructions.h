/*
 * Data Transfer Group
 */

// MOV r1, r2 (Move register to register)
int MOV(uint8_t *dest, const uint8_t *src) {
    *dest = *src;

    return 5;
}

// MOV r, M (Move memory to register)
int MOV_from_mem(uint8_t *src) {
    *src = mem_read(cpu.mem, cpu.hl);

    return 7;
}

// MOV M, r (Move register to memory)
int MOV_to_mem(const uint8_t *src) {
    mem_write(cpu.mem, cpu.hl, *src);

    return 7;
}

// MVI r, D8 (Move immediate register)
int MVI(uint8_t *dest) {
    cpu_read_byte_to_z();

    *dest = cpu.z;

    return 7;
}

// MVI M, D8 (Move to memory immediate)
int MVI_to_mem() {
    cpu_read_byte_to_z();

    mem_write(cpu.mem, cpu.hl, cpu.z);

    return 10;
}

// LXI rp, D16 (Load register pair immediate)
int LXI(uint16_t *dest) {
    cpu_read_bytes_to_wz();

    *dest = cpu.wz;

    return 10;
}

// LDA addr (Load accumulator direct)
int LDA() {
    cpu_read_bytes_to_wz();

    cpu.a = mem_read(cpu.mem, cpu.wz);

    return 13;
}

// STA addr (Store Accumulator direct)
int STA() {
    cpu_read_bytes_to_wz();

    mem_write(cpu.mem, cpu.wz, cpu.a);

    return 13;
}

// LHLD addr (Load H and L direct)
int LHLD() {
    cpu_read_bytes_to_wz();

    cpu.h =  mem_read(cpu.mem, cpu.wz+1);
    cpu.l =  mem_read(cpu.mem, cpu.wz);

    return 16;
}

// SHLD addr (Store H and L direct)
int SHLD() {
    cpu_read_bytes_to_wz();

    mem_write(cpu.mem, cpu.wz+1, cpu.h);
    mem_write(cpu.mem, cpu.wz, cpu.l);

    return 16;
}

// LDAX B (Load accumulator indirect)
int LDAX_B() {
    cpu.a = mem_read(cpu.mem, cpu.bc);

    return 7;
}

// LDAX D (Load accumulator indirect)
int LDAX_D() {
    cpu.a = mem_read(cpu.mem, cpu.de);

    return 7;
}

// STAX B (Store accumulator indirect)
int STAX_B() {
    mem_write(cpu.mem, cpu.bc, cpu.a);

    return 7;
}

// STAX D (Store accumulator indirect)
int STAX_D() {
    mem_write(cpu.mem, cpu.de, cpu.a);

    return 7;
}

// XCHG (Exchange H and L with D and E)
int XCHG() {
    // TODO Watch out!
    cpu.hl ^= cpu.de; cpu.de ^= cpu.hl; cpu.hl ^= cpu.de;

    return 4;
}



/*
 * Arithmetic Group
 */

// ADD r (Add Register)
int ADD(uint8_t *r) {
    uint16_t result = cpu.a + *r;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 4;
}

// ADD M (Add memory)
int ADD_M() {
    uint16_t result = cpu.a + mem_read(cpu.mem, cpu.hl);
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// ADI D8 (Add immediate)
int ADI() {
    cpu_read_byte_to_z();
    uint16_t result = cpu.a + cpu.z;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// ADC r (Add Register with carry)
int ADC(uint8_t *r) {
    uint16_t result = cpu.a + *r + cpu.flags.cy;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 4;

}

// SUB r (Subtract Register)
int SUB(uint8_t *r) {
    uint16_t result = cpu.a - *r;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 4;
}

// SUI D8 (Add immediate)
int SUI() {
    cpu_read_byte_to_z();
    uint16_t result = cpu.a - cpu.z;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// Subtract immediate with borrow
int SBI() {
    cpu_read_byte_to_z();
    uint16_t result = cpu.a - cpu.z - cpu.flags.cy;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// INR r (Increment Register)
int INR(uint8_t *r) {
    uint16_t result = *r + 1;
    *r = result;
    cpu_handle_flags(result, 8, F_ZSP | f_ac);

    return 5;
}

// INR M (Increment memory)
int INR_M() {
    uint32_t result = mem_read(cpu.mem, cpu.hl) + 1;
    mem_write(cpu.mem, cpu.hl, result);
    cpu_handle_flags(result, 16, F_ZSP | f_ac);

    return 10;
}

// DCR r (Decrement Register)
int DCR(uint8_t *r) {
    uint16_t result = *r - 1;
    *r = result;
    cpu_handle_flags(result, 8, F_ZSP | f_ac);
    return 5;
}

// DCR M (Decrement memory)
int DCR_M() {
    uint32_t result = mem_read(cpu.mem, cpu.hl) - 1;
    mem_write(cpu.mem, cpu.hl, result);
    cpu_handle_flags(result, 16, F_ZSP | f_ac);

    return 10;
}

// INX rp (Increment register pair)
int INX(uint16_t *rp) {
    (*rp)++;

    return 5;
}

// DCX rp (Decrement register pair)
int DCX(uint16_t *rp) {
    (*rp)--;

    return 5;
}

// DAD rp (Add register pair to HL)
int DAD(uint16_t *rp) {
    uint32_t result = cpu.hl + *rp;
    cpu.hl = result;
    cpu_handle_flags(result, 16, f_cy);

    return 10;
}

// Decimal adjust accumulator
int DAA() {
    // TODO

    return 4;
}



/*
 * Logic Group
 */

// ANA r (AND Register)
int ANA(uint8_t *r) {
    uint8_t result = cpu.a & *r;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP | f_ac);
    cpu.flags.cy = 0;

    return 4;
}

// ANA M (AND Memory)
int ANA_M() {
    uint8_t result = cpu.a & mem_read(cpu.mem, cpu.hl);
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}


// ANI D8 (AND immediate)
int ANI() {
    cpu_read_byte_to_z();
    uint8_t result = cpu.a & cpu.z;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}

// XRA r (Exclusive OR Register)
int XRA(uint8_t *r) {
    uint8_t result = cpu.a ^ *r;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 4;
}

// ORA r (OR Register)
int ORA(uint8_t *r) {
    uint8_t result = cpu.a | *r;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 4;
}

// ORA M (OR memory)
int ORA_M() {
    uint8_t result = cpu.a | mem_read(cpu.mem, cpu.hl);
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}

// ORI D8 (OR immediate)
int ORI() {
    cpu_read_byte_to_z();
    uint8_t result = cpu.a | cpu.z;
    cpu.a = result;
    cpu_handle_flags(result, 8, F_ZSP);
    cpu.flags.cy = cpu.flags.ac = 0;

    return 7;
}

// CMP r (Compare register)
int CMP(uint8_t *r) {
    uint16_t result = cpu.a - *r;
    cpu_handle_flags(result, 8, F_ALL);

    return 4;
}

// CMP M (Compare memory)
int CMP_M() {
    uint16_t result = cpu.a - mem_read(cpu.mem, cpu.hl);
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// CPI D8 (Compare immediate)
int CPI() {
    cpu_read_byte_to_z();
    uint16_t result = cpu.a - cpu.z;
    cpu_handle_flags(result, 8, F_ALL);

    return 7;
}

// RLC (Rotate left)
int RLC() {
    uint8_t temp = cpu.a;
    cpu.a = (temp << 1) | (temp & 0x80) >> 7;
    cpu.flags.cy = (temp & 0x80) >> 7;

    return 4;
}

// RRC (Rotate Right)
int RRC() {
    uint8_t temp = cpu.a;
    cpu.a = ((temp & 1) << 7) | (temp >> 1);
    cpu.flags.cy = (temp & 1);

    return 4;
}

// RAR (Rotate right through carry)
int RAR() {
    uint8_t temp = cpu.a;
    cpu.a = (cpu.flags.cy << 7) | (temp >> 1);
    cpu.flags.cy = (temp & 1);

    return 4;
}

// CMA (Complement accumulator)
int CMA() {
    cpu.a = ~cpu.a;

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
    cpu.pc = cpu.wz;

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
    cpu_push(cpu.pc);
    cpu.pc = cpu.wz;

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
    cpu.pc = cpu_pop();

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
    cpu.pc = cpu.hl;

    return 5;
}



/*
 * Stack, IO and Machine Control Group
 */

// PUSH B (Push)
int PUSH_B() {
    cpu_push(cpu.bc);

    return 11;
}

// PUSH D (Push)
int PUSH_D() {
    cpu_push(cpu.de);

    return 11;
}

// PUSH H (Push)
int PUSH_H() {
    cpu_push(cpu.hl);

    return 11;
}

// PUSH PSW (Push processor status word) [Note: And accumulator]
int PUSH_PSW() {
    cpu_push(cpu.af);

    return 11;
}

// POP B (Pop)
int POP_B() {
    cpu.bc = cpu_pop();

    return 10;
}

// POP D (Pop)
int POP_D() {
    cpu.de = cpu_pop();

    return 10;
}

// POP H (Pop)
int POP_H() {
    cpu.hl = cpu_pop();

    return 10;
}

// POP PSW (Pop processor status word)
int POP_PSW() {
    cpu.af = cpu_pop();

    return 10;
}

// XTHL (Exchange stack top with H and L)
int XTHL() {
    uint16_t temp = cpu_pop();
    cpu_push(cpu.hl);
    cpu.hl = temp;

    return 18;
}

// IN port (Input)
int IN() {
    cpu_read_byte_to_z();
    uint8_t port = cpu.z;
    if (port == 3) { return 10; }
    cpu.a = cpu.ports[port];

    return 10;
}

// OUT port (Output)
int OUT() {
    cpu_read_byte_to_z();
    uint8_t port = cpu.z;
    if (port == 2 || port == 4) { return 10; }
    cpu.ports[port] = cpu.a;

    return 10;
}

// EI (Enable interrupts)
int EI() {
    cpu.flags.i = 1;

    return 4;
}

int NOP() {
    return 4;
}
