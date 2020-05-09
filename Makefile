ForcedLibs := -C0 "Libs/*" -C0 "StandartLib/*" 
TimeFlags := time -f "time results: real - %E , user - %U user,system - %S ,memory %M KiB"

TempFolder := /tmp/

Libs := -ldl -lpthread


MainOut := $(TempFolder)/out3.ll
repair: $(wildcard CompilerReal2/*.cp) 
	./stable --rname result -p posix $(ForcedLibs) -C1 "CompilerReal2/*" CompilerReal2/main.cp -o $(MainOut); clang -g $(MainOut) $(Libs) -o c.out

cycle: $(wildcard CompilerReal2/*.cp)
	$(TimeFlags) ./c.out --rname result -p posix $(ForcedLibs) -C1 "CompilerReal2/*" CompilerReal2/main.cp -o $(MainOut); clang $(MainOut) $(Libs) -o c.out
cycleg: $(wildcard CompilerReal2/*.cp)
	./c.out -p posix -g $(ForcedLibs) -C1 "CompilerReal2/*" CompilerReal2/main.cp -o $(MainOut); clang -g $(MainOut) $(Libs) -o c.out
cyclen: $(wildcard CompilerReal2/*.cp)
	nemiver ./c.out -p posix -g -C0 "Libs/*" -C1 "CompilerReal2/*" CompilerReal2/main.cp -o $(MainOut); clang -g $(MainOut) $(Libs) -o c.out

MainOutW := $(TempFolder)/out3W.ll
wcycle: $(wildcard CompilerReal2/*.cp)
	./c.out -p win32 -g -C0 "Libs/*" -C1 "CompilerReal2/*" CompilerReal2/main.cp -o $(MainOutW); clang -g -c $(MainOutW) $(Libs) --target=x86_64-win32-gnu -o w.o

winlinux: $(wildcard CompilerReal2/*.cp) wcycle
	x86_64-w64-mingw32-gcc -g w.o -o c.exe

ManyCycle:
	for i in {1..10}; do make cycle; done

stable:
	clang -g $(MainOut) -s -O2 -ldl -o ./stable

test2: main2.cp
	./c.out -g --vk vk.xml -p posix main2.cp $(ForcedLibs) -o test2.ll; clang test2.ll -g $(Libs) -march=native -o test2
test2t: main2.cp
	./c.out -g main2.cp  -p posix --tree -C0 "Libs/*" -o test2.ll; clang test2.ll -g $(Libs) -march=native -o test2
test2l: main2.cp
	./c.out -g main2.cp  -o test2.ll; clang test2.ll -g $(Libs) -o test2
test2n: main2.cp
	gdb --args ./c.out -g main2.cp $(ForcedLibs) -o test2.ll
test2g: main2.cp
	gdb --tui ./test2

FLibs := -f FLibs/glfw.cp -f FLibs/gl.cp 

TempDir/CompilerData.zip: Mach.m Priority.pr
	mkdir -p TempDir;zip -u TempDir/CompilerData.zip Mach.m; zip -u TempDir/CompilerData.zip Priority.pr

halfvoid: cycle TempDir/CompilerData.zip
	clang -s -O2 $(MainOut) -ldl -o TempDir/PreHalf ; ./stable --ZipGlue TempDir/PreHalf TempDir/CompilerData.zip halfvoid; chmod 777 halfvoid
halfvoidg: cycleg TempDir/CompilerData.zip
	clang $(MainOut) -g -ldl -o TempDir/PreHalf ; ./stable --ZipGlue TempDir/PreHalf TempDir/CompilerData.zip halfvoid; chmod 777 halfvoid

Objs/Lex: LexBuilder/main.cp Priority.pr
	./c.out -p posix $(ForcedLibs)  LexBuilder/main.cp  -o Objs/Lex.ll; clang Objs/Lex.ll -o Objs/Lex;
Mach.m: Objs/Lex Libs/RegExpBuilder.cp
	./Objs/Lex
Objs/LexTester: LexBuilder/test.cp Libs/RegExpBuilder.cp
	./stable -p posix LexBuilder/test.cp $(ForcedLibs) -o Objs/LexTester.ll; clang Objs/LexTester.ll -o Objs/LexTester
LexTest: Objs/LexTester
	./Objs/LexTester

	
clean: 
	rm -f out.ll WinObj.o a.exe a.out ManyCycle

.PHONY: clean gdbc cycle repair test2 test2g LexTest stable
