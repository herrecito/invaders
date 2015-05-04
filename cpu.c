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
