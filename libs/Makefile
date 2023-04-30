
flags :=  -s -O2 -m64 -D_SQ64 -fno-exceptions -fno-rtti -Wall -fno-strict-aliasing -fPIC
CLANG := /usr/lib/llvm13/bin/clang

stb_truetype.so : stb/truetype.cpp
	clang -gdwarf-4 $(flags) -shared -lm  $^ -o $@
stb_truetype.dll : stb/truetype.cpp
	 x86_64-w64-mingw32-g++ -gdwarf-4 $(flags) -shared -lm  $^ -o $@

stb_image.so : stb/image.cpp
	clang -gdwarf-4 $(flags) -shared -lm  $^ -o $@
stb_image.dll : stb/image.cpp
	x86_64-w64-mingw32-g++  $(flags) -shared -lm  $^ -o $@

Nuklear.so : Nuklear.c
	clang $(flags) $^ -lm -shared -o $@

Nuklear.dll : Nuklear.c
	clang -m64 -fno-exceptions -fno-rtti -fno-strict-aliasing -O2 -static $^ -lm -shared -o $@

download:
	mkdir stb
	(cd stb; git clone https://github.com/nothings/stb)
	git clone https://github.com/Immediate-Mode-UI/Nuklear
	git clone https://github.com/wolfpld/tracy
	git clone https://chromium.googlesource.com/webm/libwebp
