
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
				return OneCall(dynCast.BeforeName, iter)
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
					return OneCall(oper,iter.Up)
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

OneCall := !(string Name, Object^ G) -> Object^
{
	Ps := Queue.{Type^}()

	P := G.Down
	if P != null
	{
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
	}

	iterT := P

	while iterT != null
	{
		Ps.Push(iterT.GetType())
		iterT = iterT.Right
	}
	SomeFunc := FindFunc(Name,G,Ps)

	if SomeFunc == null ErrorLog.Push("Function <" + Name + "> not found\n") //TODO:  PointCall and closestFunc
	else
	{
		return MakeSimpleCall(SomeFunc,P)
	}
	return null	
}


MakeSimpleCall := !(BoxFunc^ func, Object^ pars) -> NaturalCall^
{
	if func.IsAssembler() return new AssemblerCall(func,pars)
	return new NaturalCall(func,pars)
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
		if Pars != null Pars.SetUp(this&)
		ExchangeParams()
	}
	ExchangeParams := !() -> void
	{
		FType := ToCall.MyFuncType

		iter := Down
		i := 0

		while iter != null and i < FType.ParsCount 
		{
			if iter.GetType() != FType.Pars[i]
			{
				PreIter := iter
				iter = BoxExc(iter,FType.Pars[i])
			}
			i += 1
			iter = iter.Right
		}

		while iter != null
		{
			if iter.GetType() == GetType("float")
			{
				BoxExc(iter,GetType("double"))
				iter = iter.Up
			}
			iter = iter.Right
		}
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
		PrintPre(f)
	}
	PrintPre := virtual !(sfile f) -> void
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
	GetName := virtual !() -> string
	{
		return "%T" + RetId
	}
	GetValue := virtual !() -> string
	{
		return "d()"
	}
	GetType := virtual !() -> Type^
	{
		return ToCall.MyFuncType.RetType
	}
	Print := virtual !(int s) -> void {
		for s printf("->")
		printf("item: %s %s\n",GetValue(),ToCall.FuncName)
		End := this.Down
		while End != null
		{
			End.Print(s+1)
			End = End.Right
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
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
		Pars.SetUp(this&)
		ExchangeParams()
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

		if not RealCall.IsSelfPre
			PrintParamPres(f)
		
		AsmLine := RealCall.ToExe


		buf := char[2]
		buf[1] = 0

		thisName := GetName()
		j := 0
		while AsmLine[j] != 0
		{
			if AsmLine[j] != '#'
			{
				buf[0] = AsmLine[j]
				f << buf
				j += 1
			}else{
				j+= 1
				if AsmLine[j] in '0'..'9'
				{
					num := AsmLine[j] - '0'

					if num == 0
					{
						f << GetName()
					}else{
						num -= 1
						miniIter := Down
						for num miniIter = miniIter.Right

						ToAdd := string
						if RefsArr[num] ToAdd = miniIter.GetPointName()
						else ToAdd = miniIter.GetName()

						f << ToAdd
					}
				}
				if AsmLine[j] in 'A'..'Z'
				{
					numC := AsmLine[j] - 'A'
					iterR := Down
					
					k := numC
					while k  != 0
					{
						iterR = iterR.Right
						k -= 1
					}

					if RealCall.MyFuncType.ParsIsRef[numC]
						iterR.PrintPointPre(f)
					else	iterR.PrintPre(f)

				}
				if AsmLine[j] == '#'
				{
					f << RetId
				}
				j += 1
			}
		}

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


