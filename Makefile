Sors := $(wildcard CompilerPython/*.py)
Files := $(wildcard CompilerReal/*.cp)

Sors := $(Sors) $(Files)

a.out: out.ll 
	clang out.ll   -target x86_64-pc-linux-gnu -lm 

all : clean a.out a.exe

out2.ll: a.out 
	./a.out -f Libs/lib.cp main2.cp -o out2.ll
xdg.o: wayland-c/xdg.c
	gcc wayland-c/xdg.c -c -o xdg.o
xdg6.o: wayland-c/xdg.c
	gcc wayland-c/xdg6.c -c -o xdg6.o
b : out2.ll #xdg.o xdg6.o 
	clang++ -O0 $<  -target x86_64-pc-linux-gnu -d -o b  -ldl #-lglfw #-lwayland-client -lwayland-egl #-lEGL -lGLESv2
c.out: c.ll
	clang c.ll -o c.out
c.ll:  $(wildcard CompilerReal2/*.cp) a.out
	./a.out -f Libs/lib.cp -f Libs/Path.cp -f Libs/file.cp -f Libs/arrs.cp CompilerReal2/main.cp -o c.ll
cycle: $(wildcard CompilerReal2/*.cp) c.out
	./c.out -f Libs/lib.cp -f Libs/Path.cp -f Libs/file.cp -f Libs/arrs.cp CompilerReal2/main.cp -o out3.ll

test2.ll: c.out main2.cp
	./c.out main2.cp -f Libs/lib.cp -o test2.ll
test2: test2.ll
	clang test2.ll	-o test2
gdbc: $(wildcard CompilerReal2/*.cp) a.out
	nemiver ./a.out -f Libs/lib.cp -f Libs/Path.cp -f Libs/file.cp -f Libs/arrs.cp -f Libs/string.cp CompilerReal2/main.cp -o c.ll

a.exe: out.ll WinMain.cpp
	clang out.ll -target x86_64-pc-windows-gnu -c -o WinObj.o ; x86_64-w64-mingw32-g++   WinMain.o  -mwindows -L.  -o a.exe

	
out.ll: $(Sors)
	python2.7 CompilerPython/main.py $(Files)
clean:
	rm -f out.ll WinObj.o a.exe a.out

.PHONY: test All clean ou2.ll b a.out gdbc cycle 
