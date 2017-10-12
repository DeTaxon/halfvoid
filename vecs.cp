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
	      return sqrtf(this*this)
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
		Si := Size()
		if Si >= 0.001 {
			Si = 1.0 / Si
			for vec it *= Si
		}
	}
	"=" := !(Vec3 s) -> void
	{
		for 3 vec[it] = s.vec[it]
	}
}
Vec4 := class
{
	x,y,z,w := float
	vec := float[4] at x
	less := Vec3 at x
	this := !(float a,float b,float c,float d) -> void
	{
		Set(a,b,c,d)
	}
	this := !(float a,float b,float c) -> void
	{
		Set(a,b,c)
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
		return sqrtf(this*this)
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
	"=" := !(Vec4 ToSet) -> void
	{
		for 4 vec[it] = ToSet.vec[it]
	}
	Normalize := !() -> void
	{
		Si := Size()
		if Si >= 0.001 {
			Si = 1.0 / Si
			for vec it *= Si
		}
	}
	Print := !() -> void
	{
		for vec printf("%f ",it)
		printf("\n")
	}
}

Quant := class extend Vec4
{
	this := !() -> void
	{
		for 3 vec[it] = 0.0
		w = 1.0
	}
	this := !(float Val,Vec3 Ang) -> void
	{
		SetAng(Val,Ang)
	}
	SetAng := !(float Val,Vec3 Ang) -> void
	{
		Half := Val
		while Half >  6.28 Half -= 6.28
		while Half < -6.28 Half += 6.28
		Half *= 0.5
		Ang.Normalize()
		w = cosf(Half)
		Half = sinf(Half)
		for 3 vec[it] = Ang.vec[it]*Half
	}
	"*=" := !(Quant q1) -> void
	{
		Old := this
	    	//w = (Old.w * q1.w) + ( - Old.x * q1.x) + ( - Old.y * q1.y) + ( - Old.z * q1.z)
    		//x = (Old.w * q1.x) + (   Old.x * q1.w) + (   Old.y * q1.z) + ( - Old.z * q1.y)
    		//y = (Old.w * q1.y) + ( - Old.x * q1.z) + (   Old.y * q1.w) + (   Old.z * q1.x)
    		//z = (Old.w * q1.z) + (   Old.x * q1.y) + ( - Old.y * q1.x) + (   Old.z * q1.w)
	    	w = Old.w * q1.w - Old.x * q1.x - Old.y * q1.y - Old.z * q1.z
    		x = Old.w * q1.x + Old.x * q1.w + Old.y * q1.z - Old.z * q1.y
    		y = Old.w * q1.y - Old.x * q1.z + Old.y * q1.w + Old.z * q1.x
    		z = Old.w * q1.z + Old.x * q1.y - Old.y * q1.x + Old.z * q1.w
	    	//w = Old.vec[3] * q1.w - Old.vec[0] * q1.x - Old.vec[1] * q1.y - Old.vec[2] * q1.z
    		//x = Old.vec[3] * q1.x + Old.vec[0] * q1.w + Old.vec[1] * q1.z - Old.vec[2] * q1.y
    		//y = Old.vec[3] * q1.y - Old.vec[0] * q1.z + Old.vec[1] * q1.w + Old.vec[2] * q1.x
    		//z = Old.vec[3] * q1.z + Old.vec[0] * q1.y - Old.vec[1] * q1.x + Old.vec[2] * q1.w
	}
	"=" := !(Quant q1) -> void
	{
		for 4 vec[it] = q1.vec[it]
	}	
	Move := !(Vec4 ToM) -> void
	{
		Sum := this
		Temp := Quant
		Temp = ToM
		Temp.w = 0.0
		Sum *= Temp
		Temp.Set(-x,-y,-z,w)
		Sum *= Temp
		for 3 ToM.vec[it] = Sum.vec[it]
	}
	UnMove := !(Vec4 ToM) -> void
	{
		Sum := this
		Temp := Quant
		Temp = ToM
		Temp.w = 0.0
		Sum.Set(-x,-y,-z,w)
		Sum *= Temp	
		Sum *= this
		for 3 ToM.vec[it] = Sum.vec[it]
	}
}

Cent := class
{
	Pos := Vec4
	Ang := Quant
	SetAng := !(float An,Vec3 At) -> void
	{
		Ang.SetAng(An,At)
	}
	this := !(Cent In) -> void
	{
		Ang = In.Ang
		Pos = In.Pos
	}
	SetPos := !(float x,float y,float z) -> void
	{
		Pos.x = x
		Pos.y = y
		Pos.z = z
	}
	"=" := !(Cent In) -> void
	{
		Pos = In.Pos 
		Ang = In.Ang
	}
	"*=" := !(Cent In) -> void
	{
		Temp := In.Pos
		Temp.w = 0.0
		Ang.Move(Temp)
		Ang *= In.Ang
		Pos += Temp
	}
	Clean := !() -> void
	{
		for 4 { Pos.vec[it] = 0.0 Ang.vec[it] = 0.0 }
		Ang.w = 1.0
	}
	//"*" := !(Cent In) -> void
	//{
	//	ToRet := centf(this)
	//	ToRet *= In
	//	return ToRet
	//}
	//centf operator=(const centf& In)
	//{
	//	ang.vec[4.^i] = In.ang.vec[i]
	//	pos.vec[4.^i] = In.pos.vec[i]
	//	return this^
	//}
}

Mat4 := class
{
	row := Vec4[4] 
	vec := float[16] at row
	this := !() -> void
	{
		Indent()
	}
	Indent := !() -> void
	{
		for vec it = 0.0
		for 4 vec[5*it] = 1.0
	}
	Rot := !() -> void
	{
		Temp := float
		for i : 4
		{
			j := 0
			while j < i
			{	
				Temp = vec[4*j + i]
				vec[4*j + i] = vec[4*i + j]
				vec[4*i + j] = Temp
				j += 1
			}
		}
	}
	"=" := !(Quant Ang) -> void
	{
		Indent()
		Ang.Normalize()
		for 3 Ang.Move(row[it])
		//for row it.Normalize()
		
		//vec[0] = 1.0 - 2.0 * (Ang.y*Ang.y + Ang.z*Ang.z)
		//vec[5] = 1.0 - 2.0 * (Ang.x*Ang.x + Ang.z*Ang.z)
		//vec[10] = 1.0 - 2.0 * (Ang.x*Ang.x + Ang.y*Ang.y)
		//
		//vec[1] = 2.0 * (Ang.x*Ang.y - Ang.z*Ang.w)
		//vec[2] = 2.0 * (Ang.x*Ang.z + Ang.y*Ang.w)

		//vec[4] = 2.0 * (Ang.x*Ang.y + Ang.z*Ang.w)
		//vec[6] = 2.0 * (Ang.y*Ang.z - Ang.x*Ang.w)

		//vec[8] = 2.0 * (Ang.x*Ang.z - Ang.y*Ang.w)
		//vec[9] = 2.0 * (Ang.y*Ang.z + Ang.x*Ang.w)
	}
	"=" := !(Cent Cn) -> void
	{
		this = Cn.Ang
		for 3 vec[it + 12] = Cn.Pos.vec[it]
	}
	Persp := !(float aspect,float near ,float far,float ang) -> void
	{
        	f := 1.0 / tanf(ang * 3.14 / 360.0)
        	A := (far + near) / (  near - far)
        	B := (2.0 * far * near) / (near - far)

        	vec[ 0] = f / aspect   vec[ 1] =  0.0 vec[ 2] =  0.0 vec[ 3] =  0.0
        	vec[ 4] = 0.0          vec[ 5] =  f   vec[ 6] =  0.0 vec[ 7] =  0.0
        	vec[ 8] = 0.0          vec[ 9] =  0.0 vec[10] =  A   vec[11] = -1.0
        	vec[12] = 0.0          vec[13] =  0.0 vec[14] =  B   vec[15] =  0.0
	}
	Print := !() -> void
	{
		for i : 4
		{
			for j : 4 
			{
				printf("%f ",vec[i*4 + j])
			}
			printf("\n")
		}
	}
	"=" := !(Mat4 Pr) -> void
	{
		for 16 vec[it] = Pr.vec[it]
	}
	"*=" := !(Mat4 Ml) -> void
	{
		Old := this
		for vec it = 0.0

		for i : 4 for j : 4 for k : 4
		{
			this.vec[i + j*4] += Old.vec[i + k*4] * Ml.vec[k + j*4]
		}
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
		this.Rot()
	}
}


