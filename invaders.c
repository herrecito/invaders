#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>

#include "mem.h"
#include "cpu.h"
#include "disassembler.h"
#include "instructions.h"

#define TITLE "Space Invaders"
#define MEM_SIZE 0x10000
#define HEIGHT 256
#define WIDTH 224

// 8080
mem_t *ram;
cpu_t *cpu;

// SDL
SDL_Window *win;
SDL_Surface *surf;


void load_rom(mem_t *mem, const char *file_name) {
    // Open file
    FILE *f = fopen(file_name, "rb");
    if (!f) {
        printf("Could not open ROM: %s\n", file_name);
        exit(1);
    }

    // Get size
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Read into buffer
    unsigned char *buffer = malloc(fsize);
    fread(buffer, fsize, 1, f);

    // Load into memory
    mem_load(mem, 0, buffer, fsize);

    // Clean up
    free(buffer);
    fclose(f);
}


void draw_video_ram(void) {
    // Lock surface to make the pixels pointer valid
    if (SDL_LockSurface(surf)) { puts(SDL_GetError()); exit(1); }
    uint32_t *pix = surf->pixels;

    memset(surf->pixels, 0, WIDTH * HEIGHT * 4);

    int i = 0x2400;
    for (int col = 0; col < WIDTH; col ++) {
        for (int row = HEIGHT; row > 0; row -= 8) {
            for (int j = 0; j < 8; j++) {
                if (ram->mem[i] & 1 << j) {
                    pix[(row - j) * WIDTH + col] = 0xFFFFFF;
                }
            }

            i++;
        }
    }

    SDL_UnlockSurface(surf);

    // Update window
    if (SDL_UpdateWindowSurface(win)) { puts(SDL_GetError()); }
}


void init() {
    // Init 8080
    ram = mem_new(MEM_SIZE);
    cpu = cpu_new(ram);

    cpu->sp = 0xf000;  // TODO Why?

    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO)) { printf("%s\n", SDL_GetError()); exit(1); }

    // Create a window
    win = SDL_CreateWindow(TITLE, 0, 0, WIDTH, HEIGHT, 0);
    if (!win) { puts("Failed to create window"); exit(1); }

    // Get surface
    surf = SDL_GetWindowSurface(win);
    if (!surf) { puts("Failed to get surface"); exit(1); }
}

void handle_input(void) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
                case SDL_KEYDOWN:
                    switch (ev.key.keysym.sym) {
                        case 'c':  // Insert coin
                            cpu->ports[1] |= 0x1;
                            break;
                        case 's':  // P1 Start
                            cpu->ports[1] |= 0x1 << 2;
                            break;
                        case 'w': // P1 Shoot
                            cpu->ports[1] |= 0x1 << 4;
                            break;
                        case 'a': // P1 Move Left
                            cpu->ports[1] |= 0x1 << 5;
                            break;
                        case 'd': // P1 Move Right
                            cpu->ports[1] |= 0x1 << 6;
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (ev.key.keysym.sym) {
                        case 'c': // Insert coin
                            cpu->ports[1] &= ~0x1;
                            break;
                        case 's': // P1 Start
                            cpu->ports[1] &= ~(0x1 << 2);
                            break;
                        case 'w': // P1 shoot
                            cpu->ports[1] &= ~(0x1 << 4);
                            break;
                        case 'a': // P1 Move left
                            cpu->ports[1] &= ~(0x1 << 5);
                            break;
                        case 'd': // P1 Move Right
                            cpu->ports[1] &= ~(0x1 << 6);
                            break;
                        case 'q':  // Quit
                            exit(0);
                            break;
                    }
                    break;
        }
    }
}

void generate_interrupt(uint16_t addr) {
    cpu_push(cpu, cpu->pc);
    cpu->pc = addr;
    cpu->i = 0;
}


int main() {
    // Init 8080 and SDL
    init();

    // Load ROM
    load_rom(ram, "invaders.rom");

    // Run emulation
    long long cycles = 0;
    int c;
    uint16_t shift_register = 0;
    int shift_amount = 0;
    while (1) {
        cpu_fetch(cpu);

        // Shift register
        if (cpu->ir == 0xd3) { // OUT
            if (ram->mem[cpu->pc] == 2) { // Set shift amount
                shift_amount = cpu_get_re(cpu, re_a);
            } else if (ram->mem[cpu->pc] == 4) { // Set data in shift register
                shift_register = (cpu_get_re(cpu, re_a) << 8) | (shift_register >> 8);
            }
        } else if (cpu->ir == 0xdb) { // IN
            if (ram->mem[cpu->pc] == 3) { // Shift and read data
                cpu_set_re(cpu, re_a, shift_register >> (8 - shift_amount));
            }
        }

        if ((c = cpu_run_instruction(cpu))) {
            cycles += c;
        } else {
            printf("Error: Unimplemented instruction: ");
            disassemble(&ram->mem[cpu->pc-1]);
            puts("");
            cpu_dump(cpu);
            exit(1);
        }

        if (cpu->i) {
            if (cycles > 34132) {  // End of screen
                draw_video_ram();
                generate_interrupt(0x10);
                cycles -= 34132;
            } else if (cycles > 17066) {  // Mid of screen
                generate_interrupt(0x08);
            }
        }

        // Input
        handle_input();

    }

    return 0;
}
