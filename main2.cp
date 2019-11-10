
PrintIt := !(vec4f v) -> void
{
	printf("vec4f %f %f %f %f\n",v.x,v.y,v.z,v.w)
}
PrintIt := !(vec3f v) -> void
{
	//printf("vec3f %f %f %f\n",v.x,v.y,v.z)
	PrintIt(v.xyz0)
}

main := !() -> int
{
	c := vec2f(1.0f,2.0f)
	PrintIt((c*7.0f).xyxy)
	printf("sum %f\n",c.sum)

	return 0
}

