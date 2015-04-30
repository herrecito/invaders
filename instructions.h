#ifndef _H_INSTRUCTIONS_
#define _H_INSTRUCTIONS_

/* Data transfer Group */
int MOV(cpu_t *cpu, re_t to, re_t from);
int MOV_from_mem(cpu_t *cpu, re_t to);
int MOV_to_mem(cpu_t *cpu, re_t from);
int MVI(cpu_t *cpu, re_t to);
int MVI_to_mem(cpu_t *cpu);
int LXI(cpu_t *cpu, rep_t to);
int LXI_SP(cpu_t *cpu);
int LDA(cpu_t *cpu);
int STA(cpu_t *cpu);
int LHLD(cpu_t *cpu);
int SHLD(cpu_t *cpu);
int LDAX(cpu_t *cpu, rep_t rp);
int STAX(cpu_t *cpu, rep_t rp);
int XCHG(cpu_t *cpu);

/* Arithmetic Group */
int ADD(cpu_t *cpu, re_t r);
int ADD_M(cpu_t *cpu);
int ADI(cpu_t *cpu);
int ADC(cpu_t *cpu, re_t r);
int SUB(cpu_t *cpu, re_t r);
int SUI(cpu_t *cpu);
int SBI(cpu_t *cpu);
int INR(cpu_t *cpu, re_t r);
int INR_M(cpu_t *cpu);
int DCR(cpu_t *cpu, re_t r);
int DCR_M(cpu_t *cpu);
int INX(cpu_t *cpu, rep_t rp);
int DCX(cpu_t *cpu, rep_t rp);
int DAD(cpu_t *cpu, rep_t rp);
int DAD_sp(cpu_t *cpu);
int DAA(cpu_t *cpu);

/* Logic Group */
int ANA(cpu_t *cpu, re_t r);
int ANA_M(cpu_t *cpu);
int ANI(cpu_t *cpu);
int XRA(cpu_t *cpu, re_t r);
int ORA(cpu_t *cpu, re_t r);
int ORA_M(cpu_t *cpu);
int ORI(cpu_t *cpu);
int CMP(cpu_t *cpu, re_t r);
int CMP_M(cpu_t *cpu);
int CPI(cpu_t *cpu);
int RLC(cpu_t *cpu);
int RRC(cpu_t *cpu);
int RAR(cpu_t *cpu);
int CMA(cpu_t *cpu);
int STC(cpu_t *cpu);

/* Branch Group */
int JMP(cpu_t *cpu);
int JZ(cpu_t *cpu);
int JNZ(cpu_t *cpu);
int JC(cpu_t *cpu);
int JNC(cpu_t *cpu);
int JM(cpu_t *cpu);
int CALL(cpu_t *cpu);
int CZ(cpu_t *cpu);
int CNZ(cpu_t *cpu);
int CNC(cpu_t *cpu);
int RET(cpu_t *cpu);
int RZ(cpu_t *cpu);
int RNZ(cpu_t *cpu);
int RC(cpu_t *cpu);
int RNC(cpu_t *cpu);
int PCHL(cpu_t *cpu);

/* Stack, IO and Machine Control Group */
int PUSH(cpu_t *cpu, rep_t rp);
int PUSH_PSW(cpu_t *cpu);
int POP(cpu_t *cpu, rep_t rp);
int POP_PSW(cpu_t *cpu);
int XTHL(cpu_t *cpu);
int IN(cpu_t *cpu);
int OUT(cpu_t *cpu);
int EI(cpu_t *cpu);

#endif
