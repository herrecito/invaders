#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "instructions.h"

cpu_t *cpu_new(mem_t *mem) {
    cpu_t *cpu = malloc(sizeof(cpu_t));
    cpu->pc = 0;
    cpu->mem = mem;
    return cpu;
}


void cpu_dump(cpu_t *cpu) {
    printf("IR:   0x%02x  ", cpu->ir);
    printf("PC: 0x%04x  ", cpu->pc);
    printf("SP: 0x%04x\n", cpu->sp);
    printf("BC: 0x%04x  ", cpu_get_rep(cpu, rep_bc));
    printf("DE: 0x%04x  ", cpu_get_rep(cpu, rep_de));
    printf("HL: 0x%04x  ", cpu_get_rep(cpu, rep_hl));
    printf("ACC: 0x%02x\n", cpu_get_re(cpu, re_a));
    printf("Z S P CY AC I\n");
    printf("%1i %1i %1i  %1i  %1i %1i\n",
            cpu->flags.z, cpu->flags.s, cpu->flags.p, cpu->flags.cy,
            cpu->flags.ac, cpu->i);
}


uint16_t cpu_status_word(cpu_t *cpu) {
    return (cpu_get_re(cpu, re_a) << 8) |
        cpu->flags.cy |
        cpu->flags.p << 2 |
        cpu->flags.ac << 4 |
        cpu->flags.z << 6 |
        cpu->flags.s << 7;
}


void cpu_set_re(cpu_t *cpu, enum RE r, uint8_t value) {
    cpu->registers[r] = value;
}


void cpu_set_rep(cpu_t *cpu, enum REP rp, uint16_t value) {
    cpu->registers[rp] = value >> 8;
    cpu->registers[rp+1] = value;
}


uint8_t cpu_get_re(cpu_t *cpu, enum RE r) {
    return cpu->registers[r];
}


uint16_t cpu_get_rep(cpu_t *cpu, enum REP rp) {
    return (cpu->registers[rp] << 8) | (cpu->registers[rp+1]);
}


void cpu_read_bytes_to_wz(cpu_t *cpu) {
    cpu_set_re(cpu, re_z, mem_read(cpu->mem, cpu->pc));
    cpu_set_re(cpu, re_w, mem_read(cpu->mem, cpu->pc+1));
    cpu->pc += 2;
}


void cpu_read_byte_to_z(cpu_t *cpu) {
    cpu_set_re(cpu, re_z, mem_read(cpu->mem, cpu->pc));
    cpu->pc++;
}


void cpu_fetch(cpu_t *cpu) {
    cpu->ir = mem_read(cpu->mem, cpu->pc);
    cpu->pc++;
}


void cpu_handle_flags(cpu_t *cpu, uint32_t result, size_t size, int flags) {
    if (flags & f_z) {
        // TODO May be wrong
        cpu->flags.z = (result == 0);
    }

    if (flags & f_p) {
        cpu->flags.p = (result & 1) == 0;
    }

    if (flags & f_s) {
        cpu->flags.s = (result & (1 << (size-1))) >> (size-1) == 1;
    }

    if (flags & f_cy) {
        cpu->flags.cy = (result & ((0xfffffff) << size)) != 0;
    }
}

void cpu_push(cpu_t *cpu, uint16_t value) {
    mem_write(cpu->mem, cpu->sp-1, value >> 8);
    mem_write(cpu->mem, cpu->sp-2, value);

    cpu->sp -= 2;
}

uint16_t cpu_pop(cpu_t *cpu) {
    uint16_t r =
        (mem_read(cpu->mem, cpu->sp+1) << 8) | mem_read(cpu->mem, cpu->sp);

    cpu->sp += 2;

    return r;
}


int cpu_run_instruction(cpu_t *cpu) {
    switch (cpu->ir) {
        case 0x00:  // NOP
            return 4;
        case 0x40:  // MOV B, B
            return MOV(cpu, re_b, re_b);
        case 0x41:  // MOV B, C
            return MOV(cpu, re_b, re_c);
        case 0x42:  // MOV B, D
            return MOV(cpu, re_b, re_d);
        case 0x43:  // MOV B, E
            return MOV(cpu, re_b, re_e);
        case 0x44:  // MOV B, H
            return MOV(cpu, re_b, re_h);
        case 0x47:  // MOV B, A
            return MOV(cpu, re_b, re_a);
        case 0x48:  // MOV C, B
            return MOV(cpu, re_c, re_b);
        case 0x4f:  // MOV C, A
            return MOV(cpu, re_c, re_a);
        case 0x57:  // MOV D, A
            return MOV(cpu, re_d, re_a);
        case 0x5f:  // MOV E, A
            return MOV(cpu, re_e, re_a);
        case 0x61:  // MOV H, C
            return MOV(cpu, re_h, re_c);
        case 0x64:  // MOV H, H
            return MOV(cpu, re_h, re_h);
        case 0x65:  // MOV H, L
            return MOV(cpu, re_h, re_l);
        case 0x67:  // MOV H, A
            return MOV(cpu, re_h, re_a);
        case 0x68:  // MOV L, B
            return MOV(cpu, re_l, re_b);
        case 0x69:  // MOV L, C
            return MOV(cpu, re_l, re_c);
        case 0x6f:  // MOV L, A
            return MOV(cpu, re_l, re_a);
        case 0x78:  // MOV A, B
            return MOV(cpu, re_a, re_b);
        case 0x79:  // MOV A, C
            return MOV(cpu, re_a, re_c);
        case 0x7a:  // MOV A, D
            return MOV(cpu, re_a, re_d);
        case 0x7b:  // MOV A, E
            return MOV(cpu, re_a, re_e);
        case 0x7c:  // MOV A, H
            return MOV(cpu, re_a, re_h);
        case 0x7d:  // MOV A, L
            return MOV(cpu, re_a, re_l);
        case 0x46:  // MOV B, M
            return MOV_from_mem(cpu, re_b);
        case 0x4e:  // MOV C, M
            return MOV_from_mem(cpu, re_c);
        case 0x5e:  // MOV E, M
            return MOV_from_mem(cpu, re_e);
        case 0x7e:  // MOV A, M
            return MOV_from_mem(cpu, re_a);
        case 0x56:  // MOV D, M
            return MOV_from_mem(cpu, re_d);
        case 0x66:  // MOV H, M
            return MOV_from_mem(cpu, re_h);
        case 0x70:  // MOV M, B
            return MOV_to_mem(cpu, re_b);
        case 0x71:  // MOV M, C
            return MOV_to_mem(cpu, re_c);
        case 0x77:  // MOV M, A
            return MOV_to_mem(cpu, re_a);
        case 0x01:  // LXI B, D16
            cpu_read_bytes_to_wz(cpu);
            return LXI(cpu, rep_bc);
        case 0x11:  // LXI D, D16
            cpu_read_bytes_to_wz(cpu);
            return LXI(cpu, rep_de);
        case 0x21:  // LXI H, D16
            cpu_read_bytes_to_wz(cpu);
            return LXI(cpu, rep_hl);
        case 0x06:  // MVI B, D8
            cpu_read_byte_to_z(cpu);
            return MVI(cpu, re_b);
        case 0x16:  // MVI D, D8
            cpu_read_byte_to_z(cpu);
            return MVI(cpu, re_d);
        case 0x0e:  // MVI C, D8
            cpu_read_byte_to_z(cpu);
            return MVI(cpu, re_c);
        case 0x1e:  // MVI E, D8
            cpu_read_byte_to_z(cpu);
            return MVI(cpu, re_e);
        case 0x26:  // MVI H, D8
            cpu_read_byte_to_z(cpu);
            return MVI(cpu, re_h);
        case 0x2e:  // MVI L, D8
            cpu_read_byte_to_z(cpu);
            return MVI(cpu, re_l);
        case 0x3e:  // MVI A, D8
            cpu_read_byte_to_z(cpu);
            return MVI(cpu, re_a);
        case 0x36:  // MVI M, D8
            cpu_read_byte_to_z(cpu);
            return MVI_to_mem(cpu);
        case 0x0a:  // LDAX B
            return LDAX(cpu, rep_bc);
        case 0x1a:  // LDAX D
            return LDAX(cpu, rep_de);
        case 0x31:  // LXI SP, D16
            cpu_read_bytes_to_wz(cpu);
            return LXI_SP(cpu);
        case 0x3a:  // LDA D16
            cpu_read_bytes_to_wz(cpu);
            return LDA(cpu);
        case 0x32:  // STA D16
            cpu_read_bytes_to_wz(cpu);
            return STA(cpu);
        case 0x02:  // STAX B
            return STAX(cpu, rep_bc);
        case 0x12:  // STAX D
            return STAX(cpu, rep_de);
        case 0x2a:  // LHLD addr
            cpu_read_bytes_to_wz(cpu);
            return LHLD(cpu);
        case 0x22:  // SHLD addr
            cpu_read_bytes_to_wz(cpu);
            return SHLD(cpu);
        case 0xeb:  // XCHG
            return XCHG(cpu);

        case 0x80: // ADD B
            return ADD(cpu, re_b);
        case 0x81: // ADD C
            return ADD(cpu, re_c);
        case 0x82: // ADD D
            return ADD(cpu, re_d);
        case 0x83: // ADD E
            return ADD(cpu, re_e);
        case 0x85: // ADD L
            return ADD(cpu, re_l);
        case 0x86: // ADD M
            return ADD_M(cpu);
        case 0x04: // INR B
            return INR(cpu, re_b);
        case 0x0c: // INR C
            return INR(cpu, re_c);
        case 0x14: // INR D
            return INR(cpu, re_d);
        case 0x1c: // INR E
            return INR(cpu, re_e);
        case 0x24: // INR H
            return INR(cpu, re_h);
        case 0x2c: // INR L
            return INR(cpu, re_l);
        case 0x3c: // INR A
            return INR(cpu, re_a);
        case 0x34: // INR M
            return INR_M(cpu);
        case 0x05: // DCR B
            return DCR(cpu, re_b);
        case 0x0d: // DCR C
            return DCR(cpu, re_c);
        case 0x15: // DCR D
            return DCR(cpu, re_d);
        case 0x3d: // DCR A
            return DCR(cpu, re_a);
        case 0x35: // DCR M
            return DCR_M(cpu);
        case 0x03: // INX B
            return INX(cpu, rep_bc);
        case 0x13: // INX D
            return INX(cpu, rep_de);
        case 0x23: // INX H
            return INX(cpu, rep_hl);
        case 0x1b: // DCX D
            return DCX(cpu, rep_de);
        case 0x2b: // DCX H
            return DCX(cpu, rep_hl);
        case 0x09: // DAD B
            return DAD(cpu, rep_bc);
        case 0x19: // DAD D
            return DAD(cpu, rep_de);
        case 0x29: // DAD H
            return DAD(cpu, rep_hl);
        case 0xc6: // ADI D8
            cpu_read_byte_to_z(cpu);
            return ADI(cpu);
        case 0x8a: // ADC D
            return ADC(cpu, re_d);
        case 0x97: // SUB A
            return SUB(cpu, re_a);
        case 0xd6: // SUI D8
            cpu_read_byte_to_z(cpu);
            return SUI(cpu);
        case 0xde: // SBI D8
            cpu_read_byte_to_z(cpu);
            return SBI(cpu);
        case 0x27: // DAA
            return DAA(cpu);

        case 0xa0: // ANA B
            return ANA(cpu, re_b);
        case 0xa7: // ANA A
            return ANA(cpu, re_a);
        case 0xa6: // ANA M
            return ANA_M(cpu);
        case 0xe6: // ANI D8
            cpu_read_byte_to_z(cpu);
            return ANI(cpu);
        case 0xa8: // XRA B
            return XRA(cpu, re_b);
        case 0xaf: // XRA A
            return XRA(cpu, re_a);
        case 0xb0: // ORA B
            return ORA(cpu, re_b);
        case 0xb4: // ORA H
            return ORA(cpu, re_h);
        case 0xb6: // ORA M
            return ORA_M(cpu);
        case 0xf6: // ORI D8
            cpu_read_byte_to_z(cpu);
            return ORI(cpu);
        case 0xb8: // CMP B
            return CMP(cpu, re_b);
        case 0xbc: // CMP H
            return CMP(cpu, re_h);
        case 0xbe: // CMP M
            return CMP_M(cpu);
        case 0xfe: // CPI D8
            cpu_read_byte_to_z(cpu);
            return CPI(cpu);
        case 0x07: // RLC
            return RLC(cpu);
        case 0x0f: // RRC
            return RRC(cpu);
        case 0x1f: // RAR
            return RAR(cpu);
        case 0x2f: // CMA
            return CMA(cpu);
        case 0x37: // STC
            return STC(cpu);

        case 0xc3:  // JMP addr
            cpu_read_bytes_to_wz(cpu);
            return JMP(cpu);
        case 0xca:  // JZ addr
            cpu_read_bytes_to_wz(cpu);
            return JZ(cpu);
        case 0xc2:  // JNZ addr
            cpu_read_bytes_to_wz(cpu);
            return JNZ(cpu);
        case 0xda:  // JC addr
            cpu_read_bytes_to_wz(cpu);
            return JC(cpu);
        case 0xd2:  // JNC addr
            cpu_read_bytes_to_wz(cpu);
            return JNC(cpu);
        case 0xfa:  // JM addr
            cpu_read_bytes_to_wz(cpu);
            return JM(cpu);
        case 0xcd:  // CALL addr
            cpu_read_bytes_to_wz(cpu);
            return CALL(cpu);
        case 0xcc:  // CZ addr
            cpu_read_bytes_to_wz(cpu);
            return CZ(cpu);
        case 0xc4:  // CNZ addr
            cpu_read_bytes_to_wz(cpu);
            return CNZ(cpu);
        case 0xd4:  // CNC addr
            cpu_read_bytes_to_wz(cpu);
            return CNC(cpu);
        case 0xc9:  // RET
            return RET(cpu);
        case 0xc8:  // RZ
            return RZ(cpu);
        case 0xc0:  // RNZ
            return RNZ(cpu);
        case 0xd8:  // RC
            return RC(cpu);
        case 0xd0:  // RNC
            return RNC(cpu);
        case 0xe9:  // PCHL
            return PCHL(cpu);

        case 0xc5:  // PUSH B
            return PUSH(cpu, rep_bc);
        case 0xd5:  // PUSH D
            return PUSH(cpu, rep_de);
        case 0xe5:  // PUSH H
            return PUSH(cpu, rep_hl);
        case 0xf5:  // PUSH PSW
            return PUSH_PSW(cpu);
        case 0xc1:  // POP B
            return POP(cpu, rep_bc);
        case 0xd1:  // POP D
            return POP(cpu, rep_de);
        case 0xe1:  // POP H
            return POP(cpu, rep_hl);
        case 0xf1:  // POP PSW
            return POP_PSW(cpu);
        case 0xe3:  // XTHL
            return XTHL(cpu);
        case 0xdb:  // IN D8
            cpu_read_byte_to_z(cpu);
            return IN(cpu);
        case 0xd3:  // OUT D8
            cpu_read_byte_to_z(cpu);
            return OUT(cpu);
        case 0xfb:  // EI
            return EI(cpu);

        default:
            return 0;
    }
}
