TimeFlags := time -f "time results: real - %E , user - %U user,system - %S ,memory %M KiB"

TempFolder := /tmp/
TempFile := /tmp/out.ll

Libs := -ldl -lpthread -mf16c -lm

ifeq ($(grind),yes)
vgrind := valgrind --leak-check=full --log-file=grind.txt
endif

Stati := -static
ifeq ($(nostatic),yes)
	undefine Stati
endif
undefine Stati

ifeq ($(callgrind),yes)
	cg := valgrind --tool=callgrind
endif
ifeq ($(helgrind),yes)
	hg := valgrind --tool=helgrind --log-file=grind.txt
endif

ifeq ($(tm),yes)
	TimeF := $(TimeFlags)
endif

ifeq ($(tui),yes)
	gdb_tui := gdb --tui --args 
endif

MainTarget: test

test:
	$(gdb_tui) $(vgrind)  $(cg)  ./halfvoid -g -C0 libs2 main2.hv -o out.ll -cache /tmp/TestCache.zip
	clang -g out.ll -o test
win.exe:
	$(gdb_tui) ./halfvoid -win32 -g -C0 libs2 main2.hv -o out.ll
	clang --target=x86_64-w64-mingw32-gnu -g out.ll -o win.exe

Objs/CompilerData.zip: Mach.m $(wildcard *.pr)
	mkdir -p TempDir;zip -u Objs/CompilerData.zip Mach.m $(wildcard *.pr)
Objs/hres.zip: $(wildcard ./hres/*)
	zip -ur Objs/hres.zip hres

$(CurrentLex): LexBuilder/main.hv $(wildcard *.pr)
	$(CurrentWork) $(TargetPlatform) $(ForcedLibs)  -C1 "Source/" LexBuilder/main.hv  -g -o Objs/Lex.ll; clang Objs/Lex.ll -g -ldl -lpthread -o $(CurrentLex)
Mach.m: $(CurrentLex)
	$(CurrentLex)
SizeCheck:
	nm --print-size --size-sort --radix=d ./halfvoid  | less

halfvoid:
	$(TimeF) $(gdb_tui) ./ver3_2_stable -g -C0 libs2/ -C1 Source/ -o out.ll
	clang -gdwarf-4 out.ll -lm -ldl -o halfvoid
halfvoid.exe:
	$(TimeF) $(gdb_tui) ./ver3_2_stable -win32 -g -C0 libs2/ -C1 Source/ -o out.ll
	clang  --target=x86_64-w64-mingw32-gnu -gdwarf-4 out.ll -lm -ldl -o halfvoid
cycle:
	$(TimeF) $(gdb_tui)  $(vgrind) $(hg)  $(cg) ./halfvoid -g -C0 libs2/ -C1 Source/ -o $(TempFile) -cache /tmp/HVCache.zip
	clang -gdwarf-4 $(TempFile) -lm -ldl -o halfvoid
clean: 
	rm -f out.ll WinObj.o a.exe a.out 

.PHONY:  cycle ver3_2 test halfvoid win.exe
