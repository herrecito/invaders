#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "instructions.h"

void cpu_dump() {
    printf("IR:   0x%02x  ", cpu.ir);
    printf("PC: 0x%04x  ", cpu.pc);
    printf("SP: 0x%04x\n", cpu.sp);
    printf("BC: 0x%04x  ", cpu_get_rep(rep_bc));
    printf("DE: 0x%04x  ", cpu_get_rep(rep_de));
    printf("HL: 0x%04x  ", cpu_get_rep(rep_hl));
    printf("ACC: 0x%02x\n", cpu_get_re(re_a));
    printf("Z S P CY AC I\n");
    printf("%1i %1i %1i  %1i  %1i %1i\n",
            cpu.flags.z, cpu.flags.s, cpu.flags.p, cpu.flags.cy,
            cpu.flags.ac, cpu.i);
}


uint16_t cpu_status_word() {
    return (cpu_get_re(re_a) << 8) |
        cpu.flags.cy |
        cpu.flags.p << 2 |
        cpu.flags.ac << 4 |
        cpu.flags.z << 6 |
        cpu.flags.s << 7;
}


void cpu_set_re(enum RE r, uint8_t value) {
    cpu.registers[r] = value;
}


void cpu_set_rep(enum REP rp, uint16_t value) {
    cpu.registers[rp] = value >> 8;
    cpu.registers[rp+1] = value;
}


uint8_t cpu_get_re(enum RE r) {
    return cpu.registers[r];
}


uint16_t cpu_get_rep(enum REP rp) {
    return (cpu.registers[rp] << 8) | (cpu.registers[rp+1]);
}


void cpu_read_bytes_to_wz() {
    cpu_set_re(re_z, mem_read(cpu.mem, cpu.pc));
    cpu_set_re(re_w, mem_read(cpu.mem, cpu.pc+1));
    cpu.pc += 2;
}


void cpu_read_byte_to_z() {
    cpu_set_re(re_z, mem_read(cpu.mem, cpu.pc));
    cpu.pc++;
}


void cpu_fetch() {
    cpu.ir = mem_read(cpu.mem, cpu.pc);
    cpu.pc++;
}


void cpu_handle_flags(uint32_t result, size_t size, int flags) {
    if (flags & f_z) {
        // TODO May be wrong
        cpu.flags.z = (result == 0);
    }

    if (flags & f_p) {
        cpu.flags.p = (result & 1) == 0;
    }

    if (flags & f_s) {
        cpu.flags.s = (result & (1 << (size-1))) >> (size-1) == 1;
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
    uint16_t r =
        (mem_read(cpu.mem, cpu.sp+1) << 8) | mem_read(cpu.mem, cpu.sp);

    cpu.sp += 2;

    return r;
}


int cpu_run_instruction() {
    switch (cpu.ir) {
        case 0x00:  // NOP
            return 4;
        case 0x40:  // MOV B, B
            return MOV(re_b, re_b);
        case 0x41:  // MOV B, C
            return MOV(re_b, re_c);
        case 0x42:  // MOV B, D
            return MOV(re_b, re_d);
        case 0x43:  // MOV B, E
            return MOV(re_b, re_e);
        case 0x44:  // MOV B, H
            return MOV(re_b, re_h);
        case 0x47:  // MOV B, A
            return MOV(re_b, re_a);
        case 0x48:  // MOV C, B
            return MOV(re_c, re_b);
        case 0x4f:  // MOV C, A
            return MOV(re_c, re_a);
        case 0x57:  // MOV D, A
            return MOV(re_d, re_a);
        case 0x5f:  // MOV E, A
            return MOV(re_e, re_a);
        case 0x61:  // MOV H, C
            return MOV(re_h, re_c);
        case 0x64:  // MOV H, H
            return MOV(re_h, re_h);
        case 0x65:  // MOV H, L
            return MOV(re_h, re_l);
        case 0x67:  // MOV H, A
            return MOV(re_h, re_a);
        case 0x68:  // MOV L, B
            return MOV(re_l, re_b);
        case 0x69:  // MOV L, C
            return MOV(re_l, re_c);
        case 0x6f:  // MOV L, A
            return MOV(re_l, re_a);
        case 0x78:  // MOV A, B
            return MOV(re_a, re_b);
        case 0x79:  // MOV A, C
            return MOV(re_a, re_c);
        case 0x7a:  // MOV A, D
            return MOV(re_a, re_d);
        case 0x7b:  // MOV A, E
            return MOV(re_a, re_e);
        case 0x7c:  // MOV A, H
            return MOV(re_a, re_h);
        case 0x7d:  // MOV A, L
            return MOV(re_a, re_l);
        case 0x46:  // MOV B, M
            return MOV_from_mem(re_b);
        case 0x4e:  // MOV C, M
            return MOV_from_mem(re_c);
        case 0x5e:  // MOV E, M
            return MOV_from_mem(re_e);
        case 0x7e:  // MOV A, M
            return MOV_from_mem(re_a);
        case 0x56:  // MOV D, M
            return MOV_from_mem(re_d);
        case 0x66:  // MOV H, M
            return MOV_from_mem(re_h);
        case 0x70:  // MOV M, B
            return MOV_to_mem(re_b);
        case 0x71:  // MOV M, C
            return MOV_to_mem(re_c);
        case 0x77:  // MOV M, A
            return MOV_to_mem(re_a);
        case 0x01:  // LXI B, D16
            cpu_read_bytes_to_wz();
            return LXI(rep_bc);
        case 0x11:  // LXI D, D16
            cpu_read_bytes_to_wz();
            return LXI(rep_de);
        case 0x21:  // LXI H, D16
            cpu_read_bytes_to_wz();
            return LXI(rep_hl);
        case 0x06:  // MVI B, D8
            cpu_read_byte_to_z();
            return MVI(re_b);
        case 0x16:  // MVI D, D8
            cpu_read_byte_to_z();
            return MVI(re_d);
        case 0x0e:  // MVI C, D8
            cpu_read_byte_to_z();
            return MVI(re_c);
        case 0x1e:  // MVI E, D8
            cpu_read_byte_to_z();
            return MVI(re_e);
        case 0x26:  // MVI H, D8
            cpu_read_byte_to_z();
            return MVI(re_h);
        case 0x2e:  // MVI L, D8
            cpu_read_byte_to_z();
            return MVI(re_l);
        case 0x3e:  // MVI A, D8
            cpu_read_byte_to_z();
            return MVI(re_a);
        case 0x36:  // MVI M, D8
            cpu_read_byte_to_z();
            return MVI_to_mem();
        case 0x0a:  // LDAX B
            return LDAX(rep_bc);
        case 0x1a:  // LDAX D
            return LDAX(rep_de);
        case 0x31:  // LXI SP, D16
            cpu_read_bytes_to_wz();
            return LXI_SP();
        case 0x3a:  // LDA D16
            cpu_read_bytes_to_wz();
            return LDA();
        case 0x32:  // STA D16
            cpu_read_bytes_to_wz();
            return STA();
        case 0x02:  // STAX B
            return STAX(rep_bc);
        case 0x12:  // STAX D
            return STAX(rep_de);
        case 0x2a:  // LHLD addr
            cpu_read_bytes_to_wz();
            return LHLD();
        case 0x22:  // SHLD addr
            cpu_read_bytes_to_wz();
            return SHLD();
        case 0xeb:  // XCHG
            return XCHG();

        case 0x80: // ADD B
            return ADD(re_b);
        case 0x81: // ADD C
            return ADD(re_c);
        case 0x82: // ADD D
            return ADD(re_d);
        case 0x83: // ADD E
            return ADD(re_e);
        case 0x85: // ADD L
            return ADD(re_l);
        case 0x86: // ADD M
            return ADD_M();
        case 0x04: // INR B
            return INR(re_b);
        case 0x0c: // INR C
            return INR(re_c);
        case 0x14: // INR D
            return INR(re_d);
        case 0x1c: // INR E
            return INR(re_e);
        case 0x24: // INR H
            return INR(re_h);
        case 0x2c: // INR L
            return INR(re_l);
        case 0x3c: // INR A
            return INR(re_a);
        case 0x34: // INR M
            return INR_M();
        case 0x05: // DCR B
            return DCR(re_b);
        case 0x0d: // DCR C
            return DCR(re_c);
        case 0x15: // DCR D
            return DCR(re_d);
        case 0x3d: // DCR A
            return DCR(re_a);
        case 0x35: // DCR M
            return DCR_M();
        case 0x03: // INX B
            return INX(rep_bc);
        case 0x13: // INX D
            return INX(rep_de);
        case 0x23: // INX H
            return INX(rep_hl);
        case 0x1b: // DCX D
            return DCX(rep_de);
        case 0x2b: // DCX H
            return DCX(rep_hl);
        case 0x09: // DAD B
            return DAD(rep_bc);
        case 0x19: // DAD D
            return DAD(rep_de);
        case 0x29: // DAD H
            return DAD(rep_hl);
        case 0xc6: // ADI D8
            cpu_read_byte_to_z();
            return ADI();
        case 0x8a: // ADC D
            return ADC(re_d);
        case 0x97: // SUB A
            return SUB(re_a);
        case 0xd6: // SUI D8
            cpu_read_byte_to_z();
            return SUI();
        case 0xde: // SBI D8
            cpu_read_byte_to_z();
            return SBI();
        case 0x27: // DAA
            return DAA();

        case 0xa0: // ANA B
            return ANA(re_b);
        case 0xa7: // ANA A
            return ANA(re_a);
        case 0xa6: // ANA M
            return ANA_M();
        case 0xe6: // ANI D8
            cpu_read_byte_to_z();
            return ANI();
        case 0xa8: // XRA B
            return XRA(re_b);
        case 0xaf: // XRA A
            return XRA(re_a);
        case 0xb0: // ORA B
            return ORA(re_b);
        case 0xb4: // ORA H
            return ORA(re_h);
        case 0xb6: // ORA M
            return ORA_M();
        case 0xf6: // ORI D8
            cpu_read_byte_to_z();
            return ORI();
        case 0xb8: // CMP B
            return CMP(re_b);
        case 0xbc: // CMP H
            return CMP(re_h);
        case 0xbe: // CMP M
            return CMP_M();
        case 0xfe: // CPI D8
            cpu_read_byte_to_z();
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
            return PUSH(rep_bc);
        case 0xd5:  // PUSH D
            return PUSH(rep_de);
        case 0xe5:  // PUSH H
            return PUSH(rep_hl);
        case 0xf5:  // PUSH PSW
            return PUSH_PSW();
        case 0xc1:  // POP B
            return POP(rep_bc);
        case 0xd1:  // POP D
            return POP(rep_de);
        case 0xe1:  // POP H
            return POP(rep_hl);
        case 0xf1:  // POP PSW
            return POP_PSW();
        case 0xe3:  // XTHL
            return XTHL();
        case 0xdb:  // IN D8
            cpu_read_byte_to_z();
            return IN();
        case 0xd3:  // OUT D8
            cpu_read_byte_to_z();
            return OUT();
        case 0xfb:  // EI
            return EI();

        default:
            return 0;
    }
}
