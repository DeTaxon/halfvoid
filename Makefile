ForcedLibs := -C0 "Libs/*.cp" 
TimeFlags := -f "time results: real - %E , user - %U user,system - %S ,memory %M KiB"

Libs := -ldl -lpthread

repair: $(wildcard CompilerReal2/*.cp) 
	./stable -C0 "Libs/*.cp" CompilerReal2/main.cp -o out3.ll; clang out3.ll $(Libs) -o c.out

cycle: $(wildcard CompilerReal2/*.cp)
	./c.out -C0 "Libs/*.cp" CompilerReal2/main.cp -o out3.ll; clang out3.ll $(Libs) -o c.out
ManyCycle:
	for i in {1..10}; do make cycle; done

test2: main2.cp
	./c.out   main2.cp -o test2.ll; clang test2.ll -g $(Libs) -o test2
test2t: main2.cp
	./c.out main2.cp $(ForcedLibs) --tree --vk vk.xml -o test2.ll; clang test2.ll -g $(Libs) -o test2
test2g: main2.cp
	gdb  --args ./c.out main2.cp $(ForcedLibs) --vk vk.xml -o test2.ll; clang test2.ll -g $(Libs) -o test2

FLibs := -f FLibs/glfw.cp -f FLibs/gl.cp 

TempDir/CompilerData.zip: Mach.m Priority2.pr
	mkdir -p TempDir;zip -u TempDir/CompilerData.zip Mach.m; zip -u TempDir/CompilerData.zip Priority2.pr

halfvoid: cycle TempDir/CompilerData.zip
	clang -s -O2 out3.ll -ldl -o PreHalf ; ./stable --ZipGlue PreHalf TempDir/CompilerData.zip halfvoid; chmod 777 halfvoid

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
