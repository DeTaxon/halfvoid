Sors := $(wildcard *.py)
Sors := $(Sors) simpl.cp

a.out: out.ll
	clang out.ll -lglfw -O2

out.ll: $(Sors)
	python main.py

.PHONY: test out.ll
