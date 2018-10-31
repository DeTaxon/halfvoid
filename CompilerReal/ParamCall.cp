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
				{
					may2 := may->{ObjParam^}
					may3 := may2.Down
					//if may3.GetValue() != "!()" return null
					return new ParamFuncCall(dynCast.MyStr,may2)
				}

				if may.Down.GetValue() == "i:=2"
				{
					return new ParamNaturalCall(dynCast.MyStr,may.Down)
				}
			}
			if may.GetValue() == "i:=2"
			{
					return new ParamNaturalCall(dynCast.MyStr,may)
			}
			if may.IsConst()
			{
				return may.Clone()
			}
		}else{	
			ItPars := Queue.{ObjParam^}()
			CollectParamsAllByName(dynCast.MyStr,ob,ItPars)
			
			iterF := ItPars.Start
			while iterF != null
			{
				itType := iterF.Data.GetType()
				if itType != null
				{
					if itType.GetType() == "function"
					{
						return new ParamFuncCall(dynCast.MyStr,iterF.Data)
					}
				}
				iterF = iterF.Next
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
	DoTheWork := virtual !(int pri) -> void
	{
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
	IsRef := virtual !() -> bool
	{
		wut := ToCall->{Object^}
		return wut.IsRef()
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
	OutName := string
	this := !(string Name, ObjParam^ Par) -> void
	{
		BeforeName = Name
		ResultType = Par.ObjType.GetPoint()

		asNeed := ((Par.Down)->{BoxFunc^})
		if asNeed.GetValue() == "!()" asNeed.ParseBlock()
		OutName = asNeed.OutputName
	}
	GetName := virtual !() -> string
	{
		return "@" + OutName
	}
	PrintUse := virtual !(sfile f) -> void
	{
		f << ResultType.GetName() << " "
		f << "@" << OutName
	}
}

