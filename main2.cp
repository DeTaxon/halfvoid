

main := !(int argc, char^^ argv) -> int
{
	c := vec4f(1.0f,1.0f,1.0f,0.0f)
	c.x += 1.0f
	printf("wow %f\n",c.xyzw.x)
	return 0
}

