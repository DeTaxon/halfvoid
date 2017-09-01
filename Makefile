Sors := $(wildcard *.py)
Files := $(wildcard *.cp)
Sors := $(Sors) $(Files)

a.out: out.ll
	clang out.ll -lglfw -lGL -lm

out.ll: $(Sors)
	python main.py $(Files)

.PHONY: test out.ll
