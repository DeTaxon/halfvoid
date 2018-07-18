
SLambda := class extend ObjResult
{
	parsedStart := bool
	applyed := bool
	Names := string^
	fastUse := TypeFunc^
	ItId := int
	inAlloc := int
	this := !() -> void
	{
		WorkBag.Push(this&,State_Start)
		ItId = GetNewId()
		inAlloc = -1
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start and not parsedStart
		{
			parsedStart = true
			WorkBag.Push(this&,State_Syntax)
			names := Queue.{string}()

			pars := Queue.{Type^}()

			if Down.GetValue() == "()"
			{
				iter := Down
				while iter != null
				{
					if iter.GetValue() == "~ind"
					{
						asN := iter->{ObjIndent^}
						names.Push(asN.MyStr)
						pars.Push(null->{Type^})
					}else{
						if iter.GetValue() != ","
							EmitError("incorrect input of lambda\n")
					}
				}
			}else{
				if Down.GetValue() == "~ind"
				{
					asN := Down->{ObjIndent^}
					names.Push(asN.MyStr)
					pars.Push(null->{Type^})
				}else{
					EmitError("incorrect input items of lambda\n")
				}
			}
			Names = names.ToArray()

			asFunc := GetFuncType(pars,null->{bool^},null->{Type^},false,false)

			MakeItBlock(Down.Right.Right)
			ResultType = asFunc.GetLambda()
			PopOutNode(Down)
			PopOutNode(Down)
		}
		if pri == State_PostGetUse
		{
			inAlloc = GetAlloc(Up,ResultType.Base.GetPoint())
		}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		Down.PrintGlobal(f)
		
		if applyed
		{
			PrintFuncBodySkobs(f,fastUse,Names,"lambda" + ItId)
			f << "\n{\n"
			Down.PrintInBlock(f)
			f << "}\n"
		}

	}
	PrintPre := virtual !(sfile f) -> void
	{
		if applyed
		{
			f << "store " + ResultType.Base.GetPoint().GetName() + " @lambda" + ItId + ", " + ResultType.GetName() + " %T" + inAlloc + "\n"
		}
	}
	PrintUse := virtual !(sfile f) -> void
	{	
		ResultType.PrintType(f)
		f << " %T" << inAlloc
	}
	GetName := virtual !() -> string
	{
		return "%T" + inAlloc
	}
	ApplyFunc := !(Type^ lambTyp) -> void
	{
		asFType := ((lambTyp.Base)->{TypeFunc^})

		if asFType.RetType != GetType("void")
		{
			UNext(Down.Down,new ObjObj("return()"),1)
		}
		fastUse = asFType
		applyed = true
		ResultType = lambTyp
		WorkBag.Push(Down,State_Start)
		WorkBag.Push(this&,State_PostGetUse)

	}
	GetValue := virtual !() -> string
	{
		return "x=>x"
	}
}
