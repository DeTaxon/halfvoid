
GetFuncCall := !(Object^ ToParse) -> Object^
{
	iter := ToParse.Down

	if iter == null return null

	if iter.GetValue() == "(d)"
	{
		iter = iter.Right
		if iter == null return null


		if iter.GetValue() == "()"
		{
			dynCast := (iter.Left)->{ParamCall^}
			return OneCall(dynCast.BeforeName, iter.Down)
		}
		
	}else
	{
	}
	return null
}

OneCall := !(string Name, Object^ P) -> Object^
{
	Ps := Queue.{Type^}()

	Temp := P.Up
	iter := Temp.Down
	while iter != null
	{
		if iter.GetValue() == ","
		{
			PopOutNode(iter)
			iter = Temp.Down
		}else
		{
			iter = iter.Right
		}
	}

	iterT := P

	while iterT != null
	{
		Ps.Push(iterT.GetType())
		iterT = iterT.Right
	}
	SomeFunc := FindFunc(Name,P,Ps)

	if SomeFunc == null ErrorLog.Push("Function not found\n") //TODO:  PointCall and closestFunc
	else
	{
		return new NaturalCall(SomeFunc,P)
	}
	return null	
}


NaturalCall := class extend ObjResult
{
	RetId := int
	ToCall := BoxFunc^
	this := !(Object^ func, Object^ Pars) -> void 
	{
		Down = Pars

		RetId = GetNewId()
		ToCall = (func.Down)->{BoxFunc^}
	}
	PrintPreFuncName := virtual !(sfile f) -> void
	{

	}
	PrintFuncName := virtual !(sfile f) -> void
	{
		if ToCall != null
		f << "@" << ToCall.OutputName
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		PrintPreFuncName(f)
		FType := ToCall.MyFuncType

		iter := Down
		while iter != null
		{
			iter.PrintPre(f)
			iter = iter.Right
		}

		if (FType.RetType != GetType("void"))
		{
			f << "%T" << RetId <<" = "	
		}
		f << " call "
		ToCall.MyFuncType.PrintType(f)
		PrintFuncName(f)
		f << "("
		
		iter = Down
		i := 0
		while iter != null
		{
			if i > 0 f << " , "
			iter.PrintUse(f)
			iter = iter.Right
			i += 1
		}

		f << ")\n"
	}
	GetValue := virtual !() -> string
	{
		return "d()"
	}
}

//PointCall
//MethodCall
//VirtualMethodCall
//MethodPointCall
//Operator()


