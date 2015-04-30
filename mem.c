#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "mem.h"


mem_t *mem_new(int size) {
    mem_t *mem = malloc(sizeof(mem_t));
    mem->size = size;

    mem->mem = malloc(sizeof(uint8_t) * size);

    return mem;
}

void mem_load(mem_t *mem, int offset, const uint8_t *data, size_t size) {
    memcpy(mem->mem + offset, data, size);
}


void mem_reset(mem_t *mem) {
    memset(mem->mem, 0, mem->size);
}


void mem_write(mem_t *mem, uint16_t addr, uint8_t value) {
    assert(addr >= 0 && addr < mem->size);
    mem->mem[addr] = value;
}


uint8_t mem_read(mem_t *mem, uint16_t addr) {
    return mem->mem[addr];
}


void mem_dump(mem_t *mem) {
    for (int i = 0; i < mem->size; i++) {
        printf("%04x: %02x\n", i, mem->mem[i]);
    }
}
