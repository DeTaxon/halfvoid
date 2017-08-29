
Vec := class
{
	x,y,z := float
	vec := float[3] at x
	this := !() -> void
	{
		for vec it = 0.0
	}
	size := !() -> float
	{
	      return this*this
	}
	Set := !(float a,float b,float c) -> void
	{
		x = a
		y = b
		z = c
	}
	"+=" := !(Vec ToAdd) -> void
	{
		for 3 vec[it] += ToAdd.vec[it]
	}
	"*=" := !(float ToAdd) -> void
	{
		for 3 vec[it] *= ToAdd
	}
	"*=" := !(Vec ToAdd) -> void
	{
		for it,i : vec it *= ToAdd.vec[i]
	}	
	"*" := !(Vec ToAdd) -> float
	{
		Sum := 0.0
		//Sum += vec[^i]*ToAdd.vec[i]
		//for it,i vec += it*ToAdd.vec[i]
		for 3 Sum += vec[it]*ToAdd.vec[it]
		return Sum
	}
}

Quant := class
{
	Pos,Ang := Vec
}
