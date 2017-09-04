
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
	      return sqrt(this*this)
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
	      return sqrt(this*this)
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
	SetAng := !(float Val,Vec3 Ang) -> void
	{
		Half := Val*0.5
		Ang.Normalize()
		w = cosf(Half)
		Half = sinf(Half)
		for 3 vec[it] = Ang.vec[it]*Half
	}
	"*=" := !(Quant q1) -> void
	{
	    	nw := this.w * q1.w - this.x * q1.x - this.y * q1.y - this.z * q1.z
    		nx := this.w * q1.x + this.x * q1.w + this.y * q1.z - this.z * q1.y
    		ny := this.w * q1.y - this.x * q1.z + this.y * q1.w + this.z * q1.x
    		nz := this.w * q1.z + this.x * q1.y - this.y * q1.x + this.z * q1.w

		x = nx
		y = ny
		z = nz
		w = nw

		//A := (this.w + this.x) * (q1.w + q1.x)
		//B := (this.z - this.y) * (q1.y - q1.z)
		//C := (this.x - this.w) * (q1.y + q1.z)
		//D := (this.y + this.z) * (q1.x - q1.w)
		//E := (this.x + this.z) * (q1.x + q1.y)
		//F := (this.x - this.z) * (q1.x - q1.y)
		//G := (this.w + this.y) * (q1.w - q1.z)
		//H := (this.w - this.y) * (q1.w + q1.z)

		//w =  H - E - F + G
		//x =  E + F + G + H 
		//y =  E - F + G - H 
		//z =  E - F - G + H 
		//for vec it *= 0.5
		//w = w + B
		//x = A - x
		//y = y - C
		//z = z - D
	}
	"=" := !(Quant q1) -> void
	{
		for 4 vec[it] = q1.vec[it]
	}	
	Move2 := !(Vec4 ToM) -> void
	{
		Some := Quant
	    	Some.w = 0.0 - this.x * ToM.x - this.y * ToM.y - this.z * ToM.z
    		Some.x = this.w * ToM.x + this.y * ToM.z - this.z * ToM.y
    		Some.y = this.w * ToM.y - this.x * ToM.z + this.z * ToM.x
    		Some.z = this.w * ToM.z + this.x * ToM.y - this.y * ToM.x
		
    		ToM.x = Some.w * this.x - Some.x * this.w - Some.y * this.z + Some.z * this.y
    		ToM.y = Some.w * this.y + Some.x * this.z - Some.y * this.w - Some.z * this.x
    		ToM.z = Some.w * this.z - Some.x * this.y + Some.y * this.x - Some.z * this.w
		ToM.Print()
	}
	Move := !(Vec4 ToM) -> void
	{
		q2 := Quant
		Temp := Quant
		Sum := Quant

		for 3 Temp.vec[it] = ToM.vec[it]
		Temp.w = 0.0
		Sum = this
		q2.Set(-x,-y,-z,w)
		Sum *= Temp
		//Sum *= q2
		Sum.Print()
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
}

Mat4 := class
{
	row := Vec4[4] 
	vec := float[16] at row
	this := !() -> void
	{
		this.Indent()
	}
	GetP := !() -> float^
	{
		return vec
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
		this.Indent()
		//for 3 Ang.Move(row[it])	
		
		vec[0] = 1.0 - 2.0 * (Ang.y*Ang.y + Ang.z*Ang.z)
		vec[5] = 1.0 - 2.0 * (Ang.x*Ang.x + Ang.z*Ang.z)
		vec[10] = 1.0 - 2.0 * (Ang.x*Ang.x + Ang.y*Ang.y)
		
		//vec[0]  = Ang.w*Ang.w + Ang.x*Ang.x - Ang.y*Ang.y - Ang.z* Ang.z
		//vec[5]  = Ang.w*Ang.w - Ang.x*Ang.x + Ang.y*Ang.y - Ang.z* Ang.z
		//vec[10] = Ang.w*Ang.w - Ang.x*Ang.x - Ang.y*Ang.y + Ang.z* Ang.z
		
		//vec[0]  = Ang.w*Ang.w + Ang.x*Ang.x - Ang.y*Ang.y - Ang.z* Ang.z
		//vec[5]  = Ang.w*Ang.w - Ang.x*Ang.x + Ang.y*Ang.y - Ang.z* Ang.z
		//vec[10] = Ang.w*Ang.w - Ang.x*Ang.x - Ang.y*Ang.y + Ang.z* Ang.z

		vec[1] = 2.0 * (Ang.x*Ang.y - Ang.z*Ang.w)
		vec[2] = 2.0 * (Ang.x*Ang.z + Ang.y*Ang.w)

		vec[4] = 2.0 * (Ang.x*Ang.y + Ang.z*Ang.w)
		vec[6] = 2.0 * (Ang.y*Ang.z - Ang.x*Ang.w)

		vec[8] = 2.0 * (Ang.x*Ang.z - Ang.y*Ang.w)
		vec[9] = 2.0 * (Ang.y*Ang.z + Ang.x*Ang.w)
		this.Print()
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


