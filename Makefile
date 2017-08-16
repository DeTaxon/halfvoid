Sors := $(wildcard *.py)
Sors := $(Sors) simpl.cp

a.out: out.ll
	clang out.ll -lglfw -lGL

out.ll: $(Sors)
	python main.py

.PHONY: test out.ll
