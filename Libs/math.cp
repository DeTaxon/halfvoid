"<*>" := !(quantf a,quantf b) -> quantf
{
	x := (a * b.wzyx) <+> quantf( 1.0f, 1.0f,-1.0f, 1.0f)
	y := (a * b.zwxy) <+> quantf(-1.0f, 1.0f, 1.0f, 1.0f)
	z := (a * b.yxwz) <+> quantf( 1.0f,-1.0f, 1.0f, 1.0f)
	w := (a * b.xyzw) <+> quantf(-1.0f,-1.0f,-1.0f, 1.0f)
	return quantf(x,y,z,w)
}
"*" := !(quantf a, vec4f b) -> vec4f
{
	c := quantf(b.x,b.y,b.z,0.0f)
	revA := quantf(-(a.x),-(a.y),-(a.z),a.w)
	preRes := (a <*> c) <*>revA
	return vec4f(preRes.x,preRes.y,preRes.z,b.w)
}
quantfAt := !(float x, float y, float z, float de) -> quantf
{
	halfAn := de * 0.5f
	size := sqrtf(x*x + y*y + z*z)
	size = 1.0f / size
	sinA := sinf(halfAn)
	size *= sinA

	return quantf(	x*size,
			y*size,
			z*size,
			cosf(halfAn))
}

centf := class
{
	ang := quantf
	pos := vec4f
	this := !() -> void
	{
		ang = quantf(0.0f,0.0f,0.0f,1.0f)
		pos = vec4f(0.0f,0.0f,0.0f,1.0f)
	}
	FillMatr := !(float[16] toSet) -> void
	{
		for 16 toSet[it] = 0.0f

		toSet[0] = 1.0 - 2.0f*ang.y*ang.y - 2.0f*ang.z*ang.z
		toSet[5] = 1.0 - 2.0f*ang.x*ang.x - 2.0f*ang.z*ang.z
		toSet[10] = 1.0 - 2.0f*ang.x*ang.x - 2.0f*ang.y*ang.y

		toSet[1] = 2.0f*ang.x*ang.y - 2.0f*ang.z*ang.w
		toSet[2] = 2.0f*ang.x*ang.z + 2.0f*ang.y*ang.w
		toSet[4] = 2.0f*ang.x*ang.y + 2.0f*ang.z*ang.w
		toSet[6] = 2.0f*ang.y*ang.z - 2.0f*ang.x*ang.w
		toSet[8] = 2.0f*ang.x*ang.z - 2.0f*ang.y*ang.w
		toSet[9] = 2.0f*ang.y*ang.z + 2.0f*ang.x*ang.w

		toSet[15] = 0.0f
		toSet[12] = pos.x
		toSet[13] = pos.y
		toSet[14] = pos.z
	}
	"<*>" := !(centf toAdd) -> centf
	{
		result.ang = ang <*> toAdd.ang
		result.pos = (toAdd.ang * this.pos) + pos
	}
	"=" := !(centf toSet) -> void
	{	
		ang = toSet.ang
		pos = toSet.pos
	}
}
