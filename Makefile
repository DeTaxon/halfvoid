ForcedLibs := -C0 "Libs/$$" -C0 "StandardLib/$$"
TimeFlags := time -f "time results: real - %E , user - %U user,system - %S ,memory %M KiB"

TempFolder := /tmp/

Libs := -ldl -lpthread

CurrentStable := ./stable
CurrentWork := ./c.out
CurrentTest := ./test2
CurrentLex := ./Objs/Lex
HW := ./halfvoid

TargetPlatform := -p posix

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

ifeq ($(OS),Windows_NT)
	HW := ./halfvoid.exe
	CurrentStable := ./stable.exe
	CurrentWork := ./c.exe
	CurrentTest := ./test2.exe
	CurrentLex := ./Objs/Lex.exe
	TargetPlatform := -p win32
	Libs := -O0
endif

ifeq ($(tm),yes)
	TimeF := $(TimeFlags)
endif
TargetStable := $(CurrentStable)
TargetWork := $(CurrentWork)

ifeq ($(tui),yes)
	gdb_tui := gdb --tui --args 
endif

MainOut := ./Objs/out3.ll
ProgSrc := -C1 "CompilerReal2/$$" CompilerReal2/main.cp

AddStatic := $(Stati)
ifeq ($(OS),Windows_NT)
	AddStatic := -static
endif

CmplOptm := clang $(AddStatic) $(MainOut) -s -O1 $(Libs) -o $(TargetStable)
CmplDeb := clang $(AddStatic) -g -O0 $(MainOut) $(Libs) -o $(TargetWork)
CmplTest := clang $(AddStatic) -g $(MainOut) $(Libs) -o $(CurrentTest)
ifeq ($(cross),win32)
	HW := ./halfvoid.exe
	TargetStable := stable.exe
	TargetWork := stable.exe
	TargetPlatform := -p win32
	Triplet := --target=x86_64-w64-mingw32-gnu 
	CmplOptm := clang -g $(MainOut) $(Triplet) -c -s -O2 $(Libs) -o ./Objs/mdl.o ; x86_64-w64-mingw32-g++ ./Objs/mdl.o -o $(TargetStable)
	CmplDeb := clang -g $(MainOut) $(Triplet) -c -g $(Libs) -o ./Objs/mdl.o ; x86_64-w64-mingw32-g++ -g ./Objs/mdl.o -o $(TargetWork)
	CmplTest := clang -g $(MainOut) $(Triplet) -c -g $(Libs) -o ./Objs/mdl.o ; x86_64-w64-mingw32-g++ -g ./Objs/mdl.o -o ./test2.exe

	#winecmp := wine "c:\LLVM\bin\clang.exe"
	#CmplOptm := $(winecmp) -g $(MainOut) $(Triplet)  -s -O2 $(Libs) -o ./stable.exe
	#CmplDeb := $(winecmp) -g $(MainOut) $(Triplet)  -g $(Libs) -o ./c.exe 
endif

repair: $(wildcard CompilerReal2/*.cp) 
	$(gdb_tui) $(CurrentStable) -g $(TargetPlatform) $(ForcedLibs) $(ProgSrc) -o $(MainOut);$(CmplDeb)

cycle: $(wildcard CompilerReal2/*.cp)
	$(vgrind) $(cg) $(TimeF) $(gdb_tui) $(CurrentWork) -g $(TargetPlatform) $(ForcedLibs) $(ProgSrc) -o $(MainOut); $(CmplDeb)

unit: $(wildcard UnitTests/*.cp)
	$(CurrentWork) -g $(TargetPlatform) $(ForcedLibs) -C1 "UnitTests/$$" UnitTests/main.cp -o $(MainOut); clang++ -g $(MainOut) -lpthread -ldl -o unit

twice: repair cycle

ManyCycle:
	for i in {1..10}; do make cycle; done

stable:
	$(gdb_tui) $(CurrentStable) --notask -g $(TargetPlatform) $(ForcedLibs) $(ProgSrc) -o $(MainOut); $(CmplDeb)

JIT: main2.cp
	$(gdb_tui) $(CurrentWork) --notask -g $(TargetPlatform) main2.cp $(ForcedLibs) --jit
test2: main2.cp
	$(gdb_tui) $(CurrentWork) -g $(TargetPlatform) main2.cp -C0 "StandardLib/$$" -o $(MainOut); $(CmplTest)

Objs/CompilerData.zip: Mach.m Priority.pr
	mkdir -p TempDir;zip -u Objs/CompilerData.zip Mach.m Priority.pr
Objs/hres.zip: $(wildcard ./hres/*)
	zip -ur Objs/hres.zip hres

halfvoid: stable Objs/CompilerData.zip Objs/hres.zip
	$(CurrentStable) --ZipGlue $(TargetStable) Objs/CompilerData.zip $(HW)
	$(CurrentStable) --ZipAppend $(HW) Objs/hres.zip

$(CurrentLex): LexBuilder/main.cp Priority.pr CompilerReal2/PriorityRead.cp
	$(CurrentStable) --notask $(TargetPlatform) $(ForcedLibs)  LexBuilder/main.cp  -g -o Objs/Lex.ll; clang Objs/Lex.ll -g -ldl -lpthread -o $(CurrentLex)
Mach.m: $(CurrentLex)
	$(CurrentLex)
json.m:  LexBuilder/json.cp
	$(CurrentStable) $(TargetPlatform) $(ForcedLibs)  LexBuilder/json.cp  -o Objs/Lex.ll; clang Objs/Lex.ll -ldl -lpthread -o json.exe
	./json.exe

	
clean: 
	rm -f out.ll WinObj.o a.exe a.out 

.PHONY: clean cycle repair test2 test2g stable
