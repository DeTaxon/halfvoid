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
				if itType == null return null

				if itType.GetType() == "function"
					return new ParamFuncCall(dynCast.MyStr,may->{ObjParam^})
				return new ParamCall(dynCast.MyStr,may->{ObjParam^})
			}
		}
	}
	return null
}

ParamCall := class extend ObjResult
{
	BeforeName := string
	this := !(string Name , ObjParam^ par) -> void
	{
		BeforeName = Name
	}
	GetValue := virtual !() -> string
	{
		return "(d)"
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

