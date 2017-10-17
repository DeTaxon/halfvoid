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

StrCopy := !(char^ a) -> char^
{
	Si := StrSize(a)
	Pre := new char[Si+1]
	for Si Pre[it] = a[it]
	Pre[Si] = 0
	return Pre
}

"in" := !(int chr, char^ str) -> bool
{
	return ChrInStr(chr,str)
}

ChrInStr := !(int chr,char^ str) -> bool
{
	i := 0
	while str[i] 
	{
		if chr == str[i] return true
		i += 1
	}
	return false
}

DivideStr := !(char^ what,int to,Queue.{char^} B) -> void
{
	Buf := char[1024]
	i := 0
	j := 0
	while what[j]
	{
		if what[j] == to
		{
			Buf[i] = 0 
			if i B.Push(StrCopy(Buf))
			i = 0
			j += 1
		} else
		{
			Buf[i] = what[j]
			i += 1
			j += 1
		}
	}
	if i 
	{
		Buf[i] = 0
		B.Push(StrCopy(Buf))
	}		
}
DivideStr := !(char^ what,char^ to,Queue.{char^} B) -> void
{
	Buf := char[1024]
	i := 0
	j := 0
	while what[j]
	{
		if ChrInStr(what[j],to)
		{
			Buf[i] = 0
			if i B.Push(StrCopy(Buf))
			i = 0
			j += 1
		} else
		{
			Buf[i] = what[j]
			i += 1
			j += 1
		}
	}
	if i 
	{
		Buf[i] = 0
		B.Push(StrCopy(Buf))
	}		
}

StrToInt := !(char^ a) -> int 
{
	Sum := 0
	i := 0
	Min := false
	if ( a[0] == '-')
	{
		i = 1
		Min = true
	}
	while a[i] != 0 and a[i] <= '9' and a[i] >= '0'
	{
		Sum *= 10
		Sum += a[i] - '0'
		i += 1	
	}
	if Min return -Sum
	return Sum
}

StrToFloat := !(char^ a) -> float
{
	Sum := 0.0
	i := 0
	Min := false
	if (a[0] == '-')
	{
		i += 1
		Min = true
	}
	while a[i] != 0 and a[i] <= '9' and a[i] >= '0'
	{
		Sum *= 10.0
		Sum += a[i] - '0'
		i += 1	
	}
	if (a[i] == '.')
	{
		i += 1
		Q := 1.0
		while a[i] != 0 and a[i] <= '9' and a[i] >= '0'
		{
			Q *= 0.1
			Sum += Q*(a[i] - '0')
			i += 1
		}
	}
	if Min return 0.0-Sum
	return Sum
}
//"()" := !(char^ this,args...) -> char^ 
//{
//}

