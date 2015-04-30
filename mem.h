#ifndef _H_MEM_
#define _H_MEM_

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    int size;
    uint8_t *mem;
} mem_t;

mem_t *mem_new(int size);
void mem_reset(mem_t *mem);
void mem_load(mem_t *mem, int offset, const uint8_t *data, size_t size);
void mem_write(mem_t *mem, uint16_t addr, uint8_t value);
uint8_t mem_read(mem_t *mem, uint16_t addr);
void mem_dump(mem_t *mem);

#endif
