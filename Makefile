ForcedLibs := -C0 "Libs/" -C0 "StandardHVLibrary/" 
TimeFlags := time -f "time results: real - %E , user - %U user,system - %S ,memory %M KiB"

TempFolder := /tmp/

Libs := -ldl -lpthread -mf16c -lm

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
ProgSrc := -C1 "CompilerReal2/" -C1 "Source/"

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

MainTarget: ver3_2

repair: $(wildcard CompilerReal2/*.hv) 
	$(gdb_tui) $(CurrentStable) -g $(TargetPlatform) $(ForcedLibs) $(ProgSrc) -o $(MainOut);$(CmplDeb)

cycle: $(wildcard CompilerReal2/*.hv)
	$(vgrind) $(cg) $(TimeF) $(gdb_tui) $(CurrentWork) -g $(TargetPlatform) $(ForcedLibs) $(ProgSrc) -o $(MainOut); $(CmplDeb)

unit: $(wildcard UnitTests/*.hv)
	$(gdb_tui) $(CurrentWork) -g $(TargetPlatform) $(ForcedLibs) -C1 "UnitTests/" UnitTests/main.hv -o $(MainOut); clang++ -g $(MainOut) -lpthread -ldl -o unit

twice: repair cycle

ManyCycle:
	for i in {1..10}; do make cycle; done

stable:
	$(gdb_tui) $(CurrentStable)  -g $(TargetPlatform) $(ForcedLibs) $(ProgSrc) -o $(MainOut); $(CmplDeb)

Objs/CompilerData.zip: Mach.m $(wildcard *.pr)
	mkdir -p TempDir;zip -u Objs/CompilerData.zip Mach.m $(wildcard *.pr)
Objs/hres.zip: $(wildcard ./hres/*)
	zip -ur Objs/hres.zip hres

halfvoid: stable Objs/CompilerData.zip Objs/hres.zip
	$(CurrentStable) --ZipGlue $(TargetWork) Objs/CompilerData.zip $(HW)
	$(CurrentStable) --ZipAppend $(HW) Objs/hres.zip

$(CurrentLex): LexBuilder/main.hv $(wildcard *.pr)
	$(CurrentWork) $(TargetPlatform) $(ForcedLibs)  -C1 "Source/" LexBuilder/main.hv  -g -o Objs/Lex.ll; clang Objs/Lex.ll -g -ldl -lpthread -o $(CurrentLex)
Mach.m: $(CurrentLex)
	$(CurrentLex)
json.m:  LexBuilder/json.hv
	$(CurrentStable) $(TargetPlatform) $(ForcedLibs)  LexBuilder/json.cp  -o Objs/Lex.ll; clang Objs/Lex.ll -ldl -lpthread -o json.exe
	./json.exe
SizeCheck:
	nm --print-size --size-sort --radix=d ./c.out  | less

ver3_2:
	$(TimeFlags) ./ver3_2_stable -g -C0 libs2/ -C1 Ver3_2/ -o out.ll
	clang -gdwarf-4 out.ll -lm -ldl -o ver3_2
same:
	$(TimeFlags) ./ver3_2 -g -C0 libs2/ -C1 Ver3_2/ -o out.ll
	clang -gdwarf-4 out.ll -lm -ldl -o ver3_2
clean: 
	rm -f out.ll WinObj.o a.exe a.out 

.PHONY: clean cycle repair test2 test2g stable unit same ver3_2
