Sors := $(wildcard *.py)
Sors := $(Sors) simpl.cp

test: out.ll
	lli out.ll

out.ll: $(Sors)
	python main.py

.PHONY: test out.ll
