
"==" := !(char^ a,char^ b) -> bool
{
	i := 0
	while a[i] != 0 and b[i] != 0 and a[i] == b[i] i += 1
	return a[i] == b[i]
}

StrSize := !(char^ a) -> int
{
	Si := 0
	while a[Si] Si += 1
	return Si
}

StrCopy := !(char ^ a) -> char^
{
	Si := StrSize(a)
	Pre := new char[Si+1]
	for Si Pre[it] = a[it]
	Pre[Si] = 0
	return Pre
}

//"()" := !(char^ this,args...) -> char^ 
//{
//}

