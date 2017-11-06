Sors := $(wildcard CompilerPython/*.py)
Files := $(wildcard CompilerReal/*.cp)

Sors := $(Sors) $(Files)

a.out: out.ll 
	clang out.ll   -lm 

all : clean a.out a.exe

a.exe: out.ll WinMain.cpp
	clang out.ll -target x86_64-pc-windows-gnu -c -o WinObj.o ; x86_64-w64-mingw32-g++   WinMain.o  -mwindows -L.  -o a.exe

	

out.ll: $(Sors)
	python2.7 CompilerPython/main.py $(Files)
clean:
	rm -f out.ll WinObj.o a.exe a.out

.PHONY: test out.ll All clean
