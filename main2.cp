//printf := !(char^ a,...) -> int declare



main := !() -> int
{
	selfF := new ZipFile() ; $temp
	if selfF.AnalizeFile("halfvoid")
	{
		printf("yes\n")
	}else{
		printf("no\n")
	}
	return 0
}

