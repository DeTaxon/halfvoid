#import "Tree.cp"

StringContainer := class 
{
	Strs := AVLMap.{string,int}

	this := !() -> void
	{
		Strs."this"()
	}

	GetStringValue := !(string St) -> int
	{
		inMap := Strs.TryFind(St)
		if inMap != null
			return inMap^
		//#critical_atomic
		SomId := GetNewId()
		Strs[St] = SomId
		return SomId
	}
	GetString := !(int id) -> string
	{
		//iter := Strs.Start
		//while iter != null
		//{
		//	if iter.Value == id return iter.Key
		//	iter = iter.Next
		//}
		for v,k : Strs
		{
			if v == id
				return k
		}
		return ""
	}
	PrintGlobal := !(sfile f) -> void
	{
		for v,key : Strs
		{
			f << "@Str" << v << " = constant [" << StrSize(key) +  1 << " x i8] c\""
			k := 0
			SomeBuf := char[2]
			SomeBuf[1] = 0
			while key[k] != 0
			{
				SomeBuf[0] = key[k]
				if key[k] == "\n"[0]
				{
					f << "\\0A"
				}else
				{
					if key[k] == "\""[0]
					{
						f << "\\22"
					}else
					{
						if key[k] == "\\"[0]
						{
							f << "\\5C"
						}else{
							f << SomeBuf
						}
					}
				}
				k += 1
			}
			f << "\\00\"\n"
		}
		
	}
}

StrContainer := StringContainer^


YStrContainer := AVLMap.{StringSpan,char^}
GetConstString := !(StringSpan itSt) -> char^
{
	inMap := YStrContainer.TryFind(itSt)
	if inMap == null
	{
		newStr := itSt.Str()
		newSpan := StringSpan(newStr,itSt.Size())
		YStrContainer[newSpan] = newStr
		return newStr
	}
	return inMap^
}
PutConstString := !(char^ newStr) -> void
{
	YStrContainer[StringSpan(newStr)] = newStr
}


