ForcedLibs := -C0 "Libs/$$" -C0 "StandartLib/$$" 
TimeFlags := time -f "time results: real - %E , user - %U user,system - %S ,memory %M KiB"

TempFolder := /tmp/

Libs := -ldl -lpthread

CurrentStable := ./stable
CurrentWork := ./c.out
CurrentTest := ./test2
CurrentLex := ./Objs/Lex
HW := ./halfvoid

TargetPlatform := -p posix

ifeq ($(OS),Windows_NT)
	HW := ./halfvoid.exe
	CurrentStable := ./stable.exe
	CurrentWork := ./c.exe
	CurrentTest := ./test2.exe
	CurrentLex := ./Objs/Lex.exe
	TargetPlatform := -p win32
	Libs := -O0
endif

TargetStable := $(CurrentStable)
TargetWork := $(CurrentWork)

ifeq ($(tui),yes)
	gdb_tui := gdb --tui --args 
endif

MainOut := ./Objs/out3.ll
ProgSrc := -C1 "CompilerReal2/$$" CompilerReal2/main.cp

CmplOptm := clang -static $(MainOut) -s -O2 $(Libs) -o $(TargetStable)
CmplDeb := clang -static -g -O0 $(MainOut) $(Libs) -o $(TargetWork)
CmplTest := clang -static -g $(MainOut) $(Libs) -o $(CurrentTest)
ifeq ($(cross),win32)
	HW := ./halfvoid.exe
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
	$(gdb_tui) $(CurrentWork) -g $(TargetPlatform) $(ForcedLibs) $(ProgSrc) -o $(MainOut); $(CmplDeb)
twice: repair cycle

ManyCycle:
	for i in {1..10}; do make cycle; done

stable:
	$(gdb_tui) $(CurrentStable) $(TargetPlatform) $(ForcedLibs) $(ProgSrc) -o $(MainOut); $(CmplOptm)

test2: main2.cp
	$(gdb_tui) $(CurrentWork) --clib libjit.clib -g $(TargetPlatform) main2.cp $(ForcedLibs) -o $(MainOut); $(CmplTest)

Objs/CompilerData.zip: Mach.m Priority.pr
	mkdir -p TempDir;zip -u Objs/CompilerData.zip Mach.m Priority.pr
Objs/hres.zip: $(wildcard ./hres/*)
	zip -ur Objs/hres.zip hres

halfvoid: stable Objs/CompilerData.zip Objs/hres.zip
	$(TargetStable) --ZipGlue $(CurrentStable) Objs/CompilerData.zip $(HW)
	$(TargetStable) --ZipAppend $(HW) Objs/hres.zip

$(CurrentLex): LexBuilder/main.cp Priority.pr
	$(CurrentStable) $(TargetPlatform) $(ForcedLibs)  LexBuilder/main.cp  -o Objs/Lex.ll; clang Objs/Lex.ll -o $(CurrentLex)
Mach.m: $(CurrentLex) Libs/RegExpBuilder.cp
	$(CurrentLex)
json.m:  LexBuilder/json.cp
	$(CurrentStable) $(TargetPlatform) $(ForcedLibs)  LexBuilder/json.cp  -o Objs/Lex.ll; clang Objs/Lex.ll -o json.exe
	./json.exe

	
clean: 
	rm -f out.ll WinObj.o a.exe a.out 

.PHONY: clean cycle repair test2 test2g stable
