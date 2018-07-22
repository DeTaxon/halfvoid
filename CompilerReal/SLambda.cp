
SLambda := class extend ObjResult
{
	parsedStart := bool
	applyed := bool
	ABox := AllocBox
	Names := string^
	parms := LocalParam^^
	InAlloc := int^
	fastUse := TypeFunc^
	ItId := int
	inAlloc := int

	this := !() -> void
	{
		ABox.ItId = GetNewId()
		WorkBag.Push(this&,State_Start)
		ItId = GetNewId()
		inAlloc = -1
	}
	ApplyParams := !(int count,string^ names, Type^^ pars,bool^ isRef) -> void
	{
		if count != 0
		{
			parms = new LocalParam^[count]
			InAlloc = new int[count]
		}
		for i : count
		{
			InAlloc[i] = ABox.GetAlloc(pars[i])
			parms[i] = new LocalParam(pars[i],InAlloc[i])
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start and not parsedStart
		{
			parsedStart = true
			WorkBag.Push(this&,State_Syntax)
			names := Queue.{string}()
			names.Push("lambdaParam" + ItId)

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
		ABox.PrintGlobal(f)
		Down.PrintGlobal(f)
		
		if applyed
		{
			PrintFuncBodySkobs(f,fastUse,Names,"lambda" + ItId,null->{string})

			f << "\n{\n"
			ABox.PrintAlloc(f)
			for i : fastUse.ParsCount
			{
				f << "store "
				f << fastUse.Pars[i].GetName()
				if fastUse.ParsIsRef[i] f << "*"
				f << " %" << Names[i] << " , "
				f << fastUse.Pars[i].GetName()
				if fastUse.ParsIsRef[i] f << "*"
				f << "* %T" << InAlloc[i] << "\n"
			}
			Down.PrintInBlock(f)
			f << "}\n"
		}

	}
	PrintPre := virtual !(sfile f) -> void
	{
		if applyed
		{
			asL := ResultType->{TypeFuncLambda^}
			f << "store " + asL.GetPointName() + " @lambda" + ItId + ", " + ResultType.GetName() + " %T" + inAlloc + "\n"
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

		ApplyParams(fastUse.ParsCount,Names,fastUse.Pars,fastUse.ParsIsRef)
	}
	GetItem := virtual !(string name) -> Object^
	{
		for i : fastUse.ParsCount 
		{
			if Names[i] == name
			{
				printf("return %s %p\n",name,parms[i])
				return parms[i]
			}
		}
		return null
	}
	GetValue := virtual !() -> string
	{
		return "x=>x"
	}
}
