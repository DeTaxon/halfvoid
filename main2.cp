//printf := !(char^ a,...) -> int declare


test := !() -> bool
{
	return false
}


main := !() -> int
{
	assert(test())
	return 0
	selfF := new ZipFile() ; $temp
	if selfF.AnalizeFile("ziptest.zip")
	{
		for itF : selfF^
		{
			k := itF.Map()->{char^}
			for i : itF.Size()
			{
				printf("%c",k[i])
			}
		}
		printf("yes\n")
	}else{
		printf("no\n")
	}
	return 0
}

