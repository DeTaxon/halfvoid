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

ifeq ($(massif),yes)
	mass_if := valgrind --tool=massif
endif

ifeq ($(opt),yes)
	opt_mode := -O
endif
ifeq ($(callgrind),yes)
	cg := valgrind --tool=callgrind --dump-instr=yes
endif
ifeq ($(helgrind),yes)
	hg := valgrind --tool=helgrind --log-file=grind.txt
endif

ifeq ($(UseCache),yes)
	CacheFlags := -cache /tmp/HVCache.zip
endif

ifeq ($(scary),yes)
	NoScary := -no-scary
endif

ifeq ($(tm),yes)
	TimeF := $(TimeFlags)
endif

ifeq ($(tracy),yes)
	trc := -var Tracy
endif

ifeq ($(tui),yes)
	gdb_tui := gdb --tui --args 
endif

ifeq ($(exp),yes)
	Exper := -C0 ExperimentalLibrary
endif


flags := $(NoScary) $(trc) $(Exper) $(opt_mode)

MainTarget: test

test:
	$(gdb_tui) $(TimeF) $(vgrind)  $(cg)  ./halfvoid $(flags) -g -C0 StandardHVLibrary -C0 ExperimentalLibrary main2.hv -o out.ll
	clang -g out.ll -mfsgsbase -lm -o test
testground:
	$(gdb_tui) $(TimeF) $(vgrind)  $(cg)  ./halfvoid $(flags) -g -O -C0 StandardHVLibrary -C0 ExperimentalLibrary TestGround.hv -o /tmp/out.ll
	clang -g /tmp/out.ll -mfsgsbase -lm -o test
test.exe:
	$(gdb_tui) $(TimeF) $(vgrind)  $(cg)  ./halfvoid.exe -emulate-tls -win32 -g -C0 StandardHVLibrary/ main2.hv -o out.ll
	clang -g out.ll -mfsgsbase -o test.exe
test2.exe:
	$(gdb_tui) $(TimeF) $(vgrind)  $(cg)  ./halfvoid -emulate-tls -win32 -g -C0 StandardHVLibrary/ main2.hv -cache /tmp/TestCache.zip -o /tmp/out.ll
	clang --target=x86_64-w64-mingw32-gnu -g /tmp/out.ll -mfsgsbase -o test.exe -lws2_32
jit:
	$(gdb_tui) $(vgrind)  $(cg)  ./halfvoid -jit -g -C0 StandardHVLibrary main2.hv -cache /tmp/TestCache.zip
run:
	$(TimeF) $(gdb_tui) $(vgrind)  $(cg)  ./halfvoid -run main $(NoScary) -g $(AddExtra) -C0 StandardHVLibrary main2.hv -cache /tmp/TestCache.zip
run.exe:
	$(TimeF) $(gdb_tui) $(vgrind)  $(cg)  ./halfvoid.exe -run RunTest $(NoScary) -g -C0 StandardHVLibrary main2.hv -cache /tmp/TestCache.zip

run_nonstop:
	$(TimeF) $(gdb_tui) $(vgrind)  $(cg)  ./halfvoid -run main -C0 ExperimentalLibrary/ $(NoScary) -nonstop -g -C0 StandardHVLibrary main2.hv -cache /tmp/TestCache.zip
run_nonstop.exe:
	$(TimeF) $(gdb_tui) $(vgrind)  $(cg)  ./halfvoid.exe -run RunTest $(NoScary) -nonstop -g -C0 StandardHVLibrary main2.hv
win.exe:
	$(gdb_tui) ./halfvoid -win32 -g -C0 StandardHVLibrary main2.hv -o out.ll
	clang --target=x86_64-w64-mingw32-gnu -g out.ll -o win.exe

win2.exe:
	$(gdb_tui) ./halfvoid_win.exe -win32 -g -C0 StandardHVLibrary main2.hv -o out.ll
	clang -g out.ll -o win.exe
Objs/CompilerData.zip: Mach.m $(wildcard *.pr)
	mkdir -p TempDir;zip -u Objs/CompilerData.zip Mach.m $(wildcard *.pr)
Objs/hres.zip: $(wildcard ./hres/*)
	zip -ur Objs/hres.zip hres

$(CurrentLex):
	$(CurrentWork) $(TargetPlatform) $(ForcedLibs)  -C1 "Source/" LexBuilder/main.hv  -g -o Objs/Lex.ll; clang Objs/Lex.ll -g -ldl -lpthread -o $(CurrentLex)
Mach.m: LexBuilder/main.hv $(wildcard *.pr)
	./halfvoid -C0 StandardHVLibrary Source/Lex/PriorityRead.hv Source/Lex/Lex.hv Source/Utils.hv LexBuilder/main.hv -g -o /tmp/lex.ll
	clang /tmp/lex.ll -g -lm -lpthread -o lex
	./lex
SizeCheck:
	nm --print-size --size-sort --radix=d --reverse-sort ./halfvoid  | less

halfvoid:
	$(TimeF) $(gdb_tui) ./ver3_2_stable -g -C0  StandardHVLibrary/ -C1 Source/ -o out.ll
	clang -gdwarf-4 out.ll -lm -ldl -o halfvoid
halfvoid.exe:
	$(TimeF) $(gdb_tui) ./ver3_2_stable -emulate-tls -win32 -g -C0  StandardHVLibrary/ -C1 Source/ -o out.ll
	clang  --target=x86_64-w64-mingw32-gnu -gdwarf-4 out.ll -lm -ldl -o halfvoid.exe
cycle:
	$(TimeF) $(gdb_tui) $(vgrind) $(mass_if) $(hg)  $(cg) ./halfvoid $(flags)  -g -C0 StandardHVLibrary/ -C1 Source/ -o $(TempFile) $(CacheFlags)
	clang -g -mfsgsbase -gdwarf-4 $(TempFile) -lm -ldl -o halfvoid
cycle_release:
	$(TimeF) $(gdb_tui)  $(vgrind) $(hg)  $(cg) ./halfvoid  $(flags) -C0 StandardHVLibrary/ -C1 Source/ -o $(TempFile) $(CacheFlags)
	clang -mfsgsbase -O2 -s $(TempFile) -lm -ldl -o halfvoid
cycle.exe:
	$(TimeF) $(gdb_tui)  $(vgrind) $(hg)  $(cg) ./halfvoid.exe -emulate-tls $(trc) -win32 -g -C0 StandardHVLibrary/ -C1 Source/ -o out.exe.ll
	clang++ --target=x86_64-w64-mingw32-gnu -gdwarf-4 -static out.exe.ll -g -o halfvoid.exe
cycle_release.exe:
	$(TimeF) $(gdb_tui)  $(vgrind) $(hg)  $(cg) ./halfvoid.exe -win32 -C0 StandardHVLibrary/ -C1 Source/ $(CacheFlags)
	clang++ -O2 -femulated-tls -static out.exe.ll -o halfvoid.exe
clean: 
	rm -f out.ll WinObj.o a.exe a.out 

.PHONY:  cycle ver3_2 test halfvoid win.exe halfvoid.exe win2.exe test.exe Mach.m
