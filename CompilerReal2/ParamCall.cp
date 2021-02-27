ParseParamCall := !(Object^ ob) -> Object^
{
	if ob is ObjIndent
	{
		dynCast := ob->{ObjIndent^}

		may := GetItem(dynCast.MyStr,ob)
		if may == null
		{
			for Modules
			{
				may = it.GetItem(dynCast.MyStr)
				if may != null
					break
			}
		}

		if dynCast.MyStr[0] == '$'
		{
			if may == null {
				spn := dynCast.MyStr[1..0]
				inGVars := GlobalAttributes.TryFind(spn.StrTmp())
				if inGVars != null return inGVars^
				return new ObjBool(false)
			}	
			return may.Clone()
		}

		if may != null
		{
			if may is ObjParam //may.GetValue() == "i:=1"
			{
				itType := may.GetType()
				if itType == null	return null

				if itType is TypeFunc
				{
					may2 := may->{ObjParam^}
					may3 := may2.Down
					return new ParamFuncCall(dynCast.MyStr,may2)
				}

				if may.Down.GetValue() == "i:=2"
				{
					preRes := new ParamNaturalCall(dynCast.MyStr,may.Down)
					preRes.inhAttrs = may.inhAttrs
					return preRes
				}
			}else
			if may.GetValue() == "i:=2"
			{
					return new ParamNaturalCall(dynCast.MyStr,may)
			}
			if may.IsConst
			{
				return may.Clone()
			}
		}else{	
			ItPars := Queue.{ObjParam^}()
			CollectParamsAllByName(dynCast.MyStr,ob,ItPars)
			
			for itF : ItPars
			{
				itType := itF.GetType()
				if itType? is TypeFunc
				{
					return new ParamFuncCall(dynCast.MyStr,itF)
				}
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
	GetDebugValue := virtual !() -> string
	{
		if GetType() != null
			return "(d) "sbt + GetType().GetName()
		return "(d)"
	}
	DoTheWork := virtual !(int pri) -> void
	{
	}
}

ParamNaturalUnrefCall := class extend ParamNaturalCall
{
	GetType := virtual !() -> Type^
	{
		if ToCall == null return null
		tt := ToCall.GetType()
		tt = tt.GetPoint()
		return tt
	}
	PrintPointPre := virtual !(sfile f) -> void
	{
		ToCall->{MemParamCommon^}.PrepareMainPtr(f,TempId,-1)
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
		f << ToCall.GetType().GetName() << "* "
		f << ToCall->{MemParamCommon^}.GetMainPtr(TempId)
	}
	GetPointName := virtual !() -> char^
	{
		return ToCall->{MemParam^}.GetMainPtr(TempId)
	}
	PrintPre := virtual !(sfile f) -> void
	{
		tn := ToCall.GetType().GetName()
		PrintPointPre(f)
		f << "%TRes" << TempId << " = load " << tn << "* , " << tn << "** " << GetPointName() << "\n"
	}
	PrintUse := virtual !(sfile f) -> void
	{
		f << ToCall.GetType().GetName()
		f << "* %TRes" << TempId
	}
	GetName := virtual !() -> char^
	{
		return "%TRes"sbt + TempId
	}
}
ParamNaturalCall := class extend ParamCall
{
	TempId := int
	debugId := int
	ToCall := MemParam^
	this := !(string Name , Object^ par) -> void
	{
		BeforeName = Name
		TempId = GetNewId()
		
		ToCall = par->{MemParam^}
		inhAttrs = par.inhAttrs

		if DebugMode
		{
			debugId = -1
			WorkBag.Push(this&,State_PrePrint)
		}
	}
	DoTheWork := virtual !(int val) -> void
	{
		if val == State_PrePrint
		{
			if DebugMode
			{
				debugId = CreateDebugCall(this&)
			}
		}
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
	PrintInBlock := virtual !(sfile f) -> void
	{
		if ToCall is LocalParam
		{
			asLoc := ToCall->{LocalParam^}
			if asLoc.IsRef
			{
				asLoc.PrintPointPre(f,TempId,debugId)
			}
		}
	}
	
	PrintPointPre := virtual !(sfile f) -> void
	{
		ToCall.PrintPointPre(f,TempId,debugId)
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
		ToCall.PrintPointUse(f,TempId,debugId)
	}
	PrintPre := virtual !(sfile f) -> void
	{
		ToCall.PrintPre(f,TempId,debugId)
	}
	PrintUse := virtual !(sfile f) -> void
	{
		ToCall.PrintUse(f,TempId,debugId)
	}
	GetName := virtual !() -> string
	{
		return ToCall.GetName(TempId)
	}
	GetPointName := virtual !() -> string
	{
		return ToCall.GetPointName(TempId)
	}
	DoPointJIT := virtual !() -> void^
	{
		return ToCall->{Object^}.DoPointJIT()
	}
	DoJIT := virtual !() -> void^
	{
		return ToCall->{Object^}.DoJIT()
	}
}

ParamFuncCall := class extend ParamCall
{
	OutName := string
	MacroCreated := bool
	this := !(string Name, ObjParam^ Par) -> void
	{
		BeforeName = Name
		ResultType = Par.ObjType.GetPoint()

		asNeed := ((Par.Down)->{BoxFunc^})
		if asNeed.GetValue() == "!()" asNeed.ParseBlock()
		OutName = asNeed.OutputName
		inhAttrs = Par.inhAttrs
	}
	GetName := virtual !() -> string
	{
		return "@"sbt + OutName
	}
	PrintUse := virtual !(sfile f) -> void
	{
		f << ResultType.GetName() << " "
		f << "@" << OutName
	}
}

