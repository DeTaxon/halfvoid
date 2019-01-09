#import "Tree.cp"

StringContainer := class 
{
	Strs := Map.{string,int}

	GetStringValue := !(string St) -> int
	{
		//#critical_atomic
		if not Strs.Exist(St)
		{
			SomId := GetNewId()
			strToSet := ref Strs[St]
			strToSet = SomId
			return SomId
		}
		return Strs[St]
	}
	GetString := !(int id) -> string
	{
		iter := Strs.Start
		while iter != null
		{
			if iter.Value == id return iter.Key
			iter = iter.Next
		}
		return ""
	}
	PrintGlobal := !(sfile f) -> void
	{
		iter := Strs.Start
		while iter != null
		{
			f << "@Str" << iter.Value << " = constant [" << StrSize(iter.Key) +  1 << " x i8] c\""
			k := 0
			SomeBuf := char[2]
			SomeBuf[1] = 0
			while iter.Key[k] != 0
			{
				SomeBuf[0] = iter.Key[k]
				if iter.Key[k] == "\n"[0]
				{
					f << "\\0A"
				}else
				{
					if iter.Key[k] == "\""[0]
					{
						f << "\\22"
					}else
					{
						if iter.Key[k] == "\\"[0]
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
			iter = iter.Next
		}
		
	}
}

StrContainer := StringContainer
