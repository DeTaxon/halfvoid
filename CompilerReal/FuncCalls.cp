
GetFuncCall := !(Object^ ToParse) -> Object^
{
	iter := ToParse.Down

	if iter == null return null

	if iter.GetType() != null
	{
		iter = iter.Right
		if iter == null return null


		if iter.GetValue() == "()"
		{
			if iter.Left.GetValue() == "(d)"
			{
				dynCast := (iter.Left)->{ParamCall^}
				return OneCall(dynCast.BeforeName, iter.Down)
			}else{
				return null //TODO: operator()
			}
		}
		if IsOper(iter.GetValue())
		{
			oper := iter.GetValue()
			if iter.Right == null
			{
				//TODO: x++
				return null
			}else
			{
				iter = iter.Right

				if iter.Right != null
				{
					ErrorLog.Push("no more then binary allowed")
					return null
				} 
				if iter.GetType() != null
				{
					PopOutNode(iter.Left)
					return OneCall(oper,iter.Left)
				}
				return null
			}
		}
		
	}else
	{
	}
	return null
}

IsOper := !(string may) -> bool
{
	iter := PriorityData.Opers.Start

	while iter != null
	{
		if iter.Data == may return true
		iter = iter.Next
	}
	return false

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
		if SomeFunc.IsAssembler() return new AssemblerCall(SomeFunc,P)
		return new NaturalCall(SomeFunc,P)
	}
	return null	
}


NaturalCall := class extend ObjResult
{
	RetId := int
	ToCall := BoxFunc^
	this := !(BoxFunc^ func, Object^ Pars) -> void 
	{
		Down = Pars

		RetId = GetNewId()
		ToCall = func
	}
	PrintPreFuncName := virtual !(sfile f) -> void
	{

	}
	PrintFuncName := virtual !(sfile f) -> void
	{
		if ToCall != null
		f << "@" << ToCall.OutputName
	}

	PrintParamPres := virtual !(sfile f) -> void
	{
		iter := Down
		RefsArr := ToCall.MyFuncType.ParsIsRef
		i := 0
		while iter != null
		{
			if RefsArr[i] iter.PrintPointPre(f)
			else iter.PrintPre(f)
			i += 1
			iter = iter.Right
		}
	}
	PrintParamUses := virtual !(sfile f) -> void
	{
		iter := Down
		RefsArr := ToCall.MyFuncType.ParsIsRef
		i := 0
		while iter != null
		{
			if i > 0 f << " , "
			if RefsArr[i] iter.PrintPointUse(f)
				else iter.PrintUse(f)
			iter = iter.Right
			i += 1
		}
	}

	PrintInBlock := virtual !(sfile f) -> void
	{
		PrintPreFuncName(f)
		FType := ToCall.MyFuncType
		PrintParamPres(f)


		if (FType.RetType != GetType("void"))
		{
			f << "%T" << RetId <<" = "	
		}
		f << " call "
		ToCall.MyFuncType.PrintType(f)
		PrintFuncName(f)
		f << "("
		PrintParamUses(f)
		f << ")\n"
	}
	GetValue := virtual !() -> string
	{
		return "d()"
	}
	GetType := virtual !() -> Type^
	{
		return ToCall.MyFuncType.RetType
	}
}

AssemblerCall := class extend NaturalCall
{
	RealCall := BuiltInFunc^ at ToCall
	this := !(BoxFunc^ func, Object^ Pars) -> void 
	{
		Down = Pars

		RetId = GetNewId()
		ToCall = func
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		PrintPre(f)
	}
	PrintPre := virtual !(sfile f) -> void
	{
		PrintPreFuncName(f)
		FType := ToCall.MyFuncType
		RefsArr := ToCall.MyFuncType.ParsIsRef
		PrintParamPres(f)
		
		AsmLine := RealCall.ToExe

		buf := char[1024]
		thisName := GetName()
		i := 0
		j := 0
		while AsmLine[j] != 0
		{
			if AsmLine[j] != '#'
			{
				buf[i] = AsmLine[j]
				i += 1
				j += 1
			}else{
				j+= 1
				num := AsmLine[j] - '0'

				if num == 0
				{
					ToAdd := GetName()
					k := 0
					while ToAdd[k] != 0
					{
						buf[i] = ToAdd[k]
						i += 1
						k += 1
					}
				}else{
					num -= 1
					miniIter := Down
					for num miniIter = miniIter.Right

					ToAdd := string
					if RefsArr[num] ToAdd = miniIter.GetPointName()
					else ToAdd = miniIter.GetName()

					k := 0
					while ToAdd[k] != 0
					{
						buf[i] = ToAdd[k]
						i += 1
						k += 1
					}
				}
				j += 1
			}
		}
		buf[i] = 0
		f << buf

	}
	PrintUse := virtual !(sfile f) -> void
	{
		ToCall.MyFuncType.RetType.PrintType(f)
		f << " %T" << RetId
	}
	GetName := virtual !() -> string
	{
		b := char[256]
		sprintf(b,"%T%i",RetId)
		return b.Copy()
	}
}

//PointCall
//MethodCall
//VirtualMethodCall
//MethodPointCall
//Operator()


