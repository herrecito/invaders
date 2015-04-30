#ifndef _H_CPU_
#define _H_CPU_

#include "mem.h"

typedef enum RE {
    re_b = 0, re_c, re_d, re_e, re_h, re_l, // General purpose registers
    re_w, re_z,                             // Temporary registers
    re_a                                    // Accumulator
} re_t;

typedef enum REP {
    rep_bc = 0, rep_de = 2, rep_hl = 4,
    rep_wz = 6
} rep_t;

typedef enum CF {
    f_z  = 1,
    f_s  = 1 << 1,
    f_p  = 1 << 2,
    f_cy = 1 << 3,
    f_ac = 1 << 4
} flag_t;

#define F_ALL (f_z | f_s | f_p | f_cy | f_ac)
#define F_ZSP (f_z | f_s | f_p)

struct ConditionFlags {
    uint8_t z:1;   // Zero
    uint8_t s:1;   // Sign
    uint8_t p:1;   // Parity
    uint8_t cy:1;  // Carry
    uint8_t ac:1;  // Auxiliary Carry
};

typedef struct CPU {
    uint8_t ir;             // Instruction register
    uint16_t pc;            // Program counter
    uint16_t sp;            // Stack pointer
    uint8_t registers[9];   // General registers
    mem_t *mem;             // RAM
    uint8_t ports[9];       // Ports
    uint8_t i:1;            // Interrupt flag
    struct ConditionFlags flags;
} cpu_t;

cpu_t *cpu_new(mem_t *mem);
void cpu_dump(cpu_t *cpu);
uint8_t cpu_get_re(cpu_t *cpu, re_t r);
uint16_t cpu_get_rep(cpu_t *cpu, rep_t rp);
void cpu_set_re(cpu_t *cpu, re_t r, uint8_t value);
void cpu_set_rep(cpu_t *cpu, rep_t rp, uint16_t value);
void cpu_fetch(cpu_t *cpu);
int cpu_run_instruction(cpu_t *cpu);
void cpu_push(cpu_t *cpu, uint16_t value);
uint16_t cpu_pop(cpu_t *cpu);
void cpu_handle_flags(cpu_t *cpu, uint32_t result, size_t size, int flags);
uint16_t cpu_status_word(cpu_t *cpu);


#endif
