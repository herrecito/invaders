#ifndef _H_CPU_
#define _H_CPU_

#include "mem.h"

typedef enum CF {
    f_z  = 1,
    f_s  = 1 << 1,
    f_p  = 1 << 2,
    f_cy = 1 << 3,
    f_ac = 1 << 4
} flag;

#define F_ALL (f_z | f_s | f_p | f_cy | f_ac)
#define F_ZSP (f_z | f_s | f_p)

struct cpu {
    uint8_t  ir;  // Instruction register
    uint16_t pc;  // Program counter
    uint16_t sp;  // Stack pointer
    union {
        struct {
            uint8_t c;  // general purpose registers
            uint8_t b;
            uint8_t e;
            uint8_t d;
            uint8_t l;
            uint8_t h;
            uint8_t z;  // temporary registers
            uint8_t w;
            union {
                uint8_t f;  // Status Register
                struct {
                    uint8_t cy:1;   // Carry
                    uint8_t   :1;
                    uint8_t  p:1;   // Parity
                    uint8_t   :1;
                    uint8_t ac:1;   // Auxiliary Carry
                    uint8_t  i:1;   // Interrupt
                    uint8_t  z:1;   // Zero
                    uint8_t  s:1;   // Sign
                } flags;
            };
            uint8_t a;  // Accumulator
        };
        struct {
            uint16_t bc;
            uint16_t de;
            uint16_t hl;
            uint16_t wz;
            uint16_t af;
        };
    };
    mem_t *mem;             // RAM
    uint8_t ports[9];       // Ports
};

extern struct cpu cpu;

void cpu_dump();
void cpu_fetch();
int cpu_run_instruction();
void cpu_push(uint16_t value);
uint16_t cpu_pop();
void cpu_handle_flags(uint32_t result, size_t size, int flags);
void cpu_read_bytes_to_wz();
void cpu_read_byte_to_z();


#endif
