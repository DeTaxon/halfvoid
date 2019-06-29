ForcedLibs := -C0 "Libs/*.cp" 
TimeFlags := -f "time results: real - %E , user - %U user,system - %S ,memory %M KiB"

Libs := -ldl -lpthread

out2.ll: a.out 
	./a.out -f Libs/lib.cp main2.cp -o out2.ll
xdg.o: wayland-c/xdg.c
	gcc wayland-c/xdg.c -c -o xdg.o
xdg6.o: wayland-c/xdg.c
	gcc wayland-c/xdg6.c -c -o xdg6.o
b : out2.ll #xdg.o xdg6.o 
	clang++ -O0 $<  -target x86_64-pc-linux-gnu -d -o b  -ldl #-lglfw #-lwayland-client -lwayland-egl #-lEGL -lGLESv2
cycle: $(wildcard CompilerReal2/*.cp)
	./c.out -C0 "Libs/*.cp" -Z0 SLib.zip CompilerReal2/main.cp -o out3.ll; clang out3.ll $(Libs) -g  -o c.out
ManyCycle:
	for i in {1..30}; do make cycle; done
cycleg: $(wildcard CompilerReal2/*.cp)
	gdb --args ./c.out $(ForcedLibs) CompilerReal2/main.cp -o out3.ll; clang out3.ll $(Libs) -g  -o c.out
repair: $(wildcard CompilerReal2/*.cp) 
	./stable -C0 "Libs/*.cp" CompilerReal2/main.cp -o out3.ll; clang out3.ll $(Libs) -g  -o c.out

test2: main2.cp
	./c.out  -F "Libs/*.cp" main2.cp -o test2.ll; clang test2.ll -g $(Libs) -o test2
test2t: main2.cp
	./c.out main2.cp $(ForcedLibs) --tree --vk vk.xml -o test2.ll; clang test2.ll -g $(Libs) -o test2
test2g: main2.cp
	gdb  --args ./c.out main2.cp $(ForcedLibs) --vk vk.xml -o test2.ll; clang test2.ll -g $(Libs) -o test2
gdbc: $(wildcard CompilerReal2/*.cp) 
	nemiver ./c.out $(ForcedLibs) CompilerReal2/main.cp -o c.ll

FLibs := -f FLibs/glfw.cp -f FLibs/gl.cp 

engi: 
	./c.out Mecha/main.cp $(FLibs) $(ForcedLibs) --vk vk.xml -o Objs/engi.ll; clang++ Objs/engi.ll $(Libs) -lGL -lglfw -o engi

a.exe: out.ll WinMain.cpp
	clang out.ll -target x86_64-pc-windows-gnu -c -o WinObj.o ; x86_64-w64-mingw32-g++   WinMain.o  -mwindows -L.  -o a.exe
Objs/Lex: LexBuilder/main.cp Priority.pr
	./c.out LexBuilder/main.cp $(ForcedLibs)  -o Objs/Lex.ll; clang Objs/Lex.ll -o Objs/Lex;
Mach.m: Objs/Lex Libs/RegExpBuilder.cp
	./Objs/Lex
Objs/LexTester: LexBuilder/test.cp Libs/RegExpBuilder.cp
	./stable LexBuilder/test.cp $(ForcedLibs) -o Objs/LexTester.ll; clang Objs/LexTester.ll -o Objs/LexTester
LexTest: Objs/LexTester
	./Objs/LexTester

	
clean: 
	rm -f out.ll WinObj.o a.exe a.out ManyCycle

.PHONY: clean gdbc cycle repair test2 test2g LexTest engi
