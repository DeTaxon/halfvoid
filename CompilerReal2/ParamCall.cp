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
					//preRes.inhAttrs = may->{ObjParam^}.GetInhPar()
					preRes.inhAttrs = may.inhAttrs
					if preRes.inhAttrs == null and may.Line?.itAttrs.Size() != 0
					{
						if ob.Line != null
						{
							printf("bad %s %i\n",ob.Line.inFile.Get(),ob.Line.LinePos)
						}else{
							printf("bad\n")
						}
					}
					preRes.Line = ob.Line
					return preRes
				}
			}else
			if may.GetValue() == "i:=2"
			{
					return new ParamNaturalCall(dynCast.MyStr,may)
			}
			if may.IsConst
			{
				newConst := may.Clone()
				newConst.Line = ob.Line
				return newConst
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
	PrintPointPre := virtual !(TIOStream f) -> void
	{
		ToCall->{MemParamCommon^}.PrepareMainPtr(f,TempId,-1)
	}
	PrintPointUse := virtual !(TIOStream f) -> void
	{
		f << ToCall.GetType().GetName() << "* "
		f << ToCall->{MemParamCommon^}.GetMainPtr(TempId)
	}
	GetPointName := virtual !() -> char^
	{
		return ToCall->{MemParam^}.GetMainPtr(TempId)
	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		tn := ToCall.GetType().GetName()
		PrintPointPre(f)
		f << "%TRes" << TempId << " = load " << tn << "* , " << tn << "** " << GetPointName() << "\n"
	}
	PrintUse := virtual !(TIOStream f) -> void
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
	PrintInBlock := virtual !(TIOStream f) -> void
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
	
	PrintPointPre := virtual !(TIOStream f) -> void
	{
		ToCall.PrintPointPre(f,TempId,debugId)
	}
	PrintPointUse := virtual !(TIOStream f) -> void
	{
		ToCall.PrintPointUse(f,TempId,debugId)
	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		ToCall.PrintPre(f,TempId,debugId)
	}
	PrintUse := virtual !(TIOStream f) -> void
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
	PrintUse := virtual !(TIOStream f) -> void
	{
		f << ResultType.GetName() << " "
		f << "@" << OutName
	}
}

