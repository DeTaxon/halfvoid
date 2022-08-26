
flags :=  -O2 -m64 -D_SQ64 -fno-exceptions -fno-rtti -Wall -fno-strict-aliasing -fPIC

c_lib.so: uring.cpp stb/main.cpp Nuklear.o
	clang $(flags) -shared -lm -luring  $^ -o $@

Nuklear.o : Nuklear.c
	clang $(flags) $^ -lm -c -o $@

download:
	mkdir stb
	(cd stb; git clone https://github.com/nothings/stb)
	git clone https://github.com/Immediate-Mode-UI/Nuklear
