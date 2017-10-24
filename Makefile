Sors := $(wildcard *.py)
Files := $(wildcard *.cp)

Sors := $(Sors) $(Files)

a.out: out.ll LinXCBMain.o
	clang out.ll LinXCBMain.o -lwayland-client -lGL -lm -lxcb

LinXCBMain.o: LinXCBMain.cpp
	clang -c LinXCBMain.cpp
all : clean a.out a.exe

a.exe: out.ll WinMain.cpp
	clang out.ll -target x86_64-pc-windows-gnu -c -o WinObj.o ; x86_64-w64-mingw32-g++  WinMain.cpp WinMain.o  -mwindows -L.  -o a.exe

	

out.ll: $(Sors)
	python main.py $(Files)
clean:
	rm -f out.ll WinObj.o a.exe a.out

.PHONY: test out.ll All clean
