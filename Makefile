Sors := $(wildcard CompilerPython/*.py)
Files := $(wildcard CompilerReal/*.cp)

Sors := $(Sors) $(Files)

a.out: out.ll 
	clang out.ll   -target x86_64-pc-linux-gnu -lm 

all : clean a.out a.exe

out2.ll: a.out 
	./a.out
xdg.o: wayland-c/xdg.c
	gcc wayland-c/xdg.c -c -o xdg.o
xdg6.o: wayland-c/xdg.c
	gcc wayland-c/xdg6.c -c -o xdg6.o
b : out2.ll #xdg.o xdg6.o 
	clang++ -O0 $<  -target x86_64-pc-linux-gnu -d -o b  -ldl #-lglfw #-lwayland-client -lwayland-egl #-lEGL -lGLESv2

a.exe: out.ll WinMain.cpp
	clang out.ll -target x86_64-pc-windows-gnu -c -o WinObj.o ; x86_64-w64-mingw32-g++   WinMain.o  -mwindows -L.  -o a.exe

	
out.ll: $(Sors)
	python2.7 CompilerPython/main.py $(Files)
clean:
	rm -f out.ll WinObj.o a.exe a.out

.PHONY: test All clean ou2.ll b a.out
