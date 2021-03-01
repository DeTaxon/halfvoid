Hash := !(char^ str) -> u32
{
	initVal := u32()
	for c : str
	{
		initVal = initVal << 1
		initVal += c
		initVal += initVal >> 24
	}
	return initVal
}
