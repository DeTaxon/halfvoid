
Vec3 := class
{
	x,y,z := float
	vec := float[3] at x
	this := !(float a,float b,float c) -> void
	{
		this.Set(a,b,c)
	}
	this := !(float c) -> void
	{
		for vec it = c
	}
	this := !(float^ c) -> void
	{
		for 3 vec[it] = c[it]
	}
	this := !() -> void
	{
		for vec it = 0.0
	}
	Size := !() -> float
	{
	      return this*this
	}
	Set := !(float a,float b,float c) -> void
	{
		x = a
		y = b
		z = c
	}
	"+=" := !(Vec3 ToAdd) -> void
	{
		for 3 vec[it] += ToAdd.vec[it]
	}
	"*=" := !(float ToAdd) -> void
	{
		for 3 vec[it] *= ToAdd
	}
	"*=" := !(Vec3 ToAdd) -> void
	{
		for it,i : vec it *= ToAdd.vec[i]
	}	
	"*" := !(Vec3 ToAdd) -> float
	{
		Sum := 0.0
		for 3 Sum += vec[it]*ToAdd.vec[it]
		return Sum
	}
	Normalize := !() -> void
	{
		Si := this.Size()
		if Si >= 0.001 {
			Si = 1.0 / Si
			for vec it *= Si
		}
	}
}
Vec4 := class
{
	x,y,z,w := float
	vec := float[4] at x
	less := Vec3 at x
	this := !(float a,float b,float c) -> void
	{
		this.Set(a,b,c)
	}
	this := !(float c) -> void
	{
		for vec it = c
	}
	this := !(float^ c) -> void
	{
		for 4 vec[it] = c[it]
	}
	this := !() -> void
	{
		for vec it = 0.0
	}
	Size := !() -> float
	{
	      return this*this
	}
	Set := !(float a,float b,float c,float d) -> void
	{
		x = a
		y = b
		z = c
		w = d
	}
	Set := !(float a,float b,float c) -> void
	{
		x = a
		y = b
		z = c
	}
	"+=" := !(Vec4 ToAdd) -> void
	{
		for 4 vec[it] += ToAdd.vec[it]
	}
	"*=" := !(float ToAdd) -> void
	{
		for 4 vec[it] *= ToAdd
	}
	"*=" := !(Vec4 ToAdd) -> void
	{
		for it,i : vec it *= ToAdd.vec[i]
	}	
	"*" := !(Vec4 ToAdd) -> float
	{
		Sum := 0.0
		for 4 Sum += vec[it]*ToAdd.vec[it]
		return Sum
	}
	Normalize := !() -> void
	{
		Si := this.Size()
		if Si >= 0.001 {
			Si = 1.0 / Si
			for vec it *= Si
		}
	}
}

Quant := class extend Vec4
{
	this := !() -> void
	{
		for 3 vec[it] = 0.0
		w = 1.0
	}
	SetAng := !(float Val,Vec3 Ang) -> void
	{
		Half := Val*0.5
		Ang.Normalize()
		w = cosf(Half)
		Half = sinf(Half)
		for 3 vec[it] = Ang.vec[it]*Half
		this.Normalize()
	}
	
}

Mat4 := class
{
	row := Vec4[4] 
	vec := float[16] at row
	this := !() -> void
	{
		for vec it = 0.0
		for 4 vec[4*it + it*4] = 1.0
	} 
	SetAng := !(Vec3 ang) -> void
	{
		A := cosf(ang.x)
		B := sinf(ang.x)
		C := cosf(ang.y)
		D := sinf(ang.y)
		E := cosf(ang.z)
		F := sinf(ang.z)

		AD :=   A * D
		BD :=   B * D

		vec[0] =   C * E
		vec[1] =  -C * F
		vec[2] =  -D
		vec[4] = -BD * E + A * F
		vec[5] =  BD * F + A * E
		vec[6] =  -B * C
		vec[8] =  AD * E + B * F
		vec[9] = -AD * F + B * E
		vec[10]=   A * C

		vec[3] =  0.0 
		vec[7] =  0.0
		vec[11] = 0.0 
		vec[12] = 0.0 
		vec[13] = 0.0 
		vec[14] = 0.0
		vec[15]=  1.0
	}
}


