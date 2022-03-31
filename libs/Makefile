
flags :=  -O2 -m64 -D_SQ64 -fno-exceptions -fno-rtti -Wall -fno-strict-aliasing -fPIC -shared

c_lib.so: uring.cpp stb/main.cpp
	clang++ $(flags) -lm -luring  $^ -o $@

download:
	mkdir stb
	(cd stb; git clone https://github.com/nothings/stb)
