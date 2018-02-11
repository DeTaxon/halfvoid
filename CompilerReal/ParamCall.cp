ParseParamCall := !(Object^ ob) -> Object^
{
	if ob.GetValue() == "~ind"
	{
		dynCast := ob->{ObjIndent^}

		may := GetItem(dynCast.MyStr,ob)

		if may != null
		{
			if may.GetValue() == "i:=1"
			{
				itType := may.GetType()
				if itType == null	return null

				if itType.GetType() == "function"
					return new ParamFuncCall(dynCast.MyStr,may->{ObjParam^})

				if may.Down.GetValue() == "i:=2"
				{
					return new ParamNaturalCall(dynCast.MyStr,may.Down)
				}
				printf("here\n")
			}
			if may.GetValue() == "i:=2"
			{
					return new ParamNaturalCall(dynCast.MyStr,may)
			}
		}
	}
	return null
}

ParamCall := class extend ObjResult
{
	BeforeName := string
	GetValue := virtual !() -> string
	{
		return "(d)"
	}
}

ParamNaturalCall := class extend ParamCall
{
	TempId := int
	ToCall := MemParam^
	this := !(string Name , Object^ par) -> void
	{
		BeforeName = Name
		TempId = GetNewId()
		
		ToCall = par->{MemParam^}
	}
	GetType := virtual !() -> Type^
	{
		if ToCall == null return null
		return ToCall.GetType()
	}
	
	PrintPointPre := virtual !(sfile f) -> void
	{
		ToCall.PrintPointPre(f,TempId)
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
		ToCall.PrintPointUse(f,TempId)
	}
	PrintPre := virtual !(sfile f) -> void
	{
		ToCall.PrintPre(f,TempId)
	}
	PrintUse := virtual !(sfile f) -> void
	{
		ToCall.PrintUse(f,TempId)
	}
	GetName := virtual !() -> string
	{
		return ToCall.GetName(TempId)
	}
	GetPointName := virtual !() -> string
	{
		return ToCall.GetPointName(TempId)
	}
}

ParamFuncCall := class extend ParamCall
{
	this := !(string Name, ObjParam^ Par) -> void
	{
		BeforeName = Name
		ResultType = Par.ObjType
	}
}

