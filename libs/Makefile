
flags :=  -g -m64 -D_SQ64 -fno-exceptions -fno-rtti -Wall -fno-strict-aliasing -fPIC
CLANG := /usr/lib/llvm13/bin/clang

c_lib.so: uring.cpp stb/main.cpp Nuklear.o
	clang -gdwarf-4 $(flags) -shared -lm -luring  $^ -o $@

NuklearGLFW.so : NuklearGLFW.c
	clang $(flags) -shared -lm -lglfw $^ -o $@

Nuklear.o : Nuklear.c
	clang $(flags) $^ -lm -c -o $@

download:
	mkdir stb
	(cd stb; git clone https://github.com/nothings/stb)
	git clone https://github.com/Immediate-Mode-UI/Nuklear
