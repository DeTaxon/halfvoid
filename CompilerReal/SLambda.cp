
SLambda := class extend ObjResult
{
	parsedStart := bool
	applyed := bool
	Names := Queue.{string}
	fastUse := TypeFunc^
	this := !() -> void
	{
		WorkBag.Push(this&,State_Start)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start and not parsedStart
		{
			parsedStart = true
			WorkBag.Push(this&,State_Syntax)

			pars := Queue.{Type^}()

			if Down.GetValue() == "()"
			{
				iter := Down
				while iter != null
				{
					if iter.GetValue() == "~ind"
					{
						asN := iter->{ObjIndent^}
						Names.Push(asN.MyStr)
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
					Names.Push(asN.MyStr)
					pars.Push(null->{Type^})
				}else{
					EmitError("incorrect input items of lambda\n")
				}
			}

			asFunc := GetFuncType(pars,null->{bool^},null->{Type^},false,false)

			MakeItBlock(Down.Right.Right)
			ResultType = asFunc.GetLambda()
			PopOutNode(Down)
			PopOutNode(Down)
		}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		Down.PrintGlobal(f)

		f << ";hello\n"
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
	}
	GetValue := virtual !() -> string
	{
		return "x=>x"
	}
}
