#ifndef _H_INSTRUCTIONS_
#define _H_INSTRUCTIONS_

/* Data transfer Group */
int MOV(re_t to, re_t from);
int MOV_from_mem(re_t to);
int MOV_to_mem(re_t from);
int MVI(re_t to);
int MVI_to_mem();
int LXI(rep_t to);
int LXI_SP();
int LDA();
int STA();
int LHLD();
int SHLD();
int LDAX(rep_t rp);
int STAX(rep_t rp);
int XCHG();

/* Arithmetic Group */
int ADD(re_t r);
int ADD_M();
int ADI();
int ADC(re_t r);
int SUB(re_t r);
int SUI();
int SBI();
int INR(re_t r);
int INR_M();
int DCR(re_t r);
int DCR_M();
int INX(rep_t rp);
int DCX(rep_t rp);
int DAD(rep_t rp);
int DAD_sp();
int DAA();

/* Logic Group */
int ANA(re_t r);
int ANA_M();
int ANI();
int XRA(re_t r);
int ORA(re_t r);
int ORA_M();
int ORI();
int CMP(re_t r);
int CMP_M();
int CPI();
int RLC();
int RRC();
int RAR();
int CMA();
int STC();

/* Branch Group */
int JMP();
int JZ();
int JNZ();
int JC();
int JNC();
int JM();
int CALL();
int CZ();
int CNZ();
int CNC();
int RET();
int RZ();
int RNZ();
int RC();
int RNC();
int PCHL();

/* Stack, IO and Machine Control Group */
int PUSH(rep_t rp);
int PUSH_PSW();
int POP(rep_t rp);
int POP_PSW();
int XTHL();
int IN();
int OUT();
int EI();

#endif
