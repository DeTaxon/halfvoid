Sors := $(wildcard *.py)
Files := $(wildcard *.cp)

Sors := $(Sors) $(Files)


a.out: out.ll
	clang out.ll -lglfw -lGL -lm

all : clean a.out a.exe

a.exe: out.ll
	clang out.ll -target x86_64-pc-windows-gnu -c -o WinObj.o ; x86_64-w64-mingw32-gcc WinObj.o -lopengl32 -L. -lglfw3 -mwindows -o a.exe

out.ll: $(Sors)
	python main.py $(Files)
clean:
	rm -f out.ll WinObj.o a.exe a.out

.PHONY: test out.ll All clean
