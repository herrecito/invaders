CC=clang
CFLAGS=-g -Wall -std=c99

bin_folder=bin

objects=\
		$(bin_folder)/mem.o\
		$(bin_folder)/cpu.o\
		$(bin_folder)/disassembler.o

default: mkdirs invaders

$(bin_folder)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

invaders: $(objects) invaders.c
	$(CC) $(CFLAGS) $(shell sdl2-config --cflags) $(shell sdl2-config --libs) -o $@ $^

mkdirs:
	[[ -e bin ]] || mkdir -p $(bin_folder)

clean:
	rm -rf $(bin_folder)
	rm -f invaders

.PHONY: clean mkdirs
