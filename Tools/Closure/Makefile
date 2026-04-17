fils := $(wildcard *.asm)
targets := $(fils:%.asm=%.bin)

all: $(targets)

%.o: %.asm
	as $< -o $@
%.hex: %.o
	objdump -d  $< | cut -f2 | grep -v "_start" | grep -v "tail_call" | grep -v text | grep -v file | tee $@
%.bin: %.hex
	xxd -r -p $< $@

clean:
	rm -f *.hex *.bin

.PHONY: all clean
