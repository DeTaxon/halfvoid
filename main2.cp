
main := !() -> int
{
	ikko := () => {
		x := 0
		yield void
		x += 1
		printf("ikko1 %i\n",x)
		yield void
		x += 1
		printf("ikko2 %i\n",x)
		yield void
		x += 1
		printf("ikko3 %i\n",x)
		yield void
		x += 1
		printf("ikko4 %i\n",x)
		yield void
		x += 1
		printf("ikko5 %i\n",x)
		return void
	}
	bokko := () => {
		x := 0
		yield void
		x -= 1
		printf("bokko1 %i\n",x)
		yield void
		x -= 1
		printf("bokko2 %i\n",x)
		yield void
		x -= 1
		printf("bokko3 %i\n",x)
		yield void
		x -= 1
		printf("bokko4 %i\n",x)
		yield void
		x -= 1
		printf("bokko5 %i\n",x)
		return void
	}
	ikko()
	bokko()
	ikko()
	bokko()
	ikko()
	bokko()
	ikko()
	bokko()
	ikko()
	bokko()
	
	return 0
}

