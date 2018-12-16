ForcedLibs := -f Libs/WordParser.cp -f Libs/lib.cp -f Libs/Path.cp -f Libs/file.cp -f Libs/arrs.cp -f Libs/FatArray.cp -f Libs/Pair.cp -f Libs/AVLMap.cp -f Libs/MemoryPool.cp -f Libs/HybridQueue.cp -f Libs/Memory.cp

out2.ll: a.out 
	./a.out -f Libs/lib.cp main2.cp -o out2.ll
xdg.o: wayland-c/xdg.c
	gcc wayland-c/xdg.c -c -o xdg.o
xdg6.o: wayland-c/xdg.c
	gcc wayland-c/xdg6.c -c -o xdg6.o
b : out2.ll #xdg.o xdg6.o 
	clang++ -O0 $<  -target x86_64-pc-linux-gnu -d -o b  -ldl #-lglfw #-lwayland-client -lwayland-egl #-lEGL -lGLESv2
cycle: $(wildcard CompilerReal2/*.cp)
	time ./c.out $(ForcedLibs) CompilerReal2/main.cp -o out3.ll; clang out3.ll -o c.out
repair: $(wildcard CompilerReal2/*.cp) 
	time ./stable $(ForcedLibs) CompilerReal2/main.cp -o out3.ll; clang out3.ll -o c.out

test2: main2.cp
	./c.out main2.cp $(ForcedLibs) -o test2.ll; clang test2.ll -o test2
test2g: main2.cp
	nemiver ./c.out main2.cp $(ForcedLibs) -o test2.ll; clang test2.ll -o test2
gdbc: $(wildcard CompilerReal2/*.cp) 
	nemiver ./c.out $(ForcedLibs) CompilerReal2/main.cp -o c.ll

a.exe: out.ll WinMain.cpp
	clang out.ll -target x86_64-pc-windows-gnu -c -o WinObj.o ; x86_64-w64-mingw32-g++   WinMain.o  -mwindows -L.  -o a.exe
Objs/Lex: LexBuilder/main.cp Priority.pr
	./c.out LexBuilder/main.cp $(ForcedLibs)  -o lex.ll; clang lex.ll -o Objs/Lex;
Mach.m: Objs/Lex Libs/RegExpBuilder.cp
	./Objs/Lex
Objs/LexTester: LexBuilder/test.cp Libs/RegExpBuilder.cp
	./c.out LexBuilder/test.cp $(ForcedLibs) -o Objs/LexTester.ll; clang Objs/LexTester.ll -o Objs/LexTester
LexTest: Objs/LexTester
	./Objs/LexTester

	
clean:
	rm -f out.ll WinObj.o a.exe a.out

.PHONY: clean gdbc cycle repair test2 test2g LexTest
