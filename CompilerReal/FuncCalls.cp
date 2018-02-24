
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
		if iter.GetValue() == "[]"
		{
			iter = iter.Left
			iter.Right = iter.Right.Down
			iter.Right.Left = iter
			iter.SetUp(iter.Up)
			return OneCall("[]",iter.Up)
		}
		if iter.GetValue() == "~suffix"
		{
			if iter.Left.GetType() == null return null

			AsSuf := iter->{ObjSuffix^}
			Pars := Queue.{Type^}()
			Pars.Push(iter.Left.GetType())

			Func := FindSuffix(AsSuf.MyStr,iter,Pars)
			if Func == null return null
			
			iter = iter.Left
			PopOutNode(iter.Right)
			Pars.Clean()
			return MakeSimpleCall(Func,iter) 
		}
		if iter.GetValue() == "."
		{
			if iter.Right.GetValue() == "~ind"
			{
				asIndent := (iter.Right)->{ObjIndent^} 
				asName := asIndent.MyStr
				pars := Queue.{Type^}
				pars.Start = null
				pars.Push(iter.Left.GetType())
				roll :=  GlobalUnroll.GetFunc(pars,asName)
				if roll == null return null

				iter = iter.Left
				return MakeSimpleCall(roll,iter)
			}
		}
		if IsOper(iter.GetValue())
		{
			oper := iter.GetValue()
			if iter.Right == null
			{
				//TODO: x++ x^ x[]
				iter = iter.Left
				PopOutNode(iter.Right)
				return OneCall(oper,iter.Up)
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

SomeFuncCall := class extend ObjResult
{
	RetId := int
	ToCall := BoxFunc^

	UseCall := virtual !(sfile f) -> void
	{
	}
	PrintPointPre := virtual !(sfile f) -> void
	{
		UseCall(f)
	}
	PrintPre := virtual !(sfile f) -> void
	{
		UseCall(f)
		if ToCall.MyFuncType.RetRef
		{
			f << "%TE" << RetId << " = load "
			f << ToCall.MyFuncType.RetType.GetName()
			f << " , "
			f << ToCall.MyFuncType.RetType.GetPoint().GetName()
			f << " %T" <<RetId << "\n"
		}
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
		f << " %TE" << RetId
	}
	PrintUse := virtual !(sfile f) -> void
	{
		ToCall.MyFuncType.RetType.PrintType(f)
		if ToCall.MyFuncType.RetRef
		{
			f << " %TE" << RetId
		}else{
			f << " %T" << RetId
		}
	}
	GetPointName := virtual !() -> string
	{
		return "%T" + RetId
	}
	GetName := virtual !() -> string
	{
		if ToCall.MyFuncType.RetRef
		{
			return "%TE" + RetId
		}
		return "%T" + RetId
	}
	GetOutputName := virtual !() -> string
	{
		return "%T" + RetId
	}
	GetValue := virtual !() -> string
	{
		return "d()"
	}
}

NaturalCall := class extend SomeFuncCall
{
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
				iter = BoxExc(iter,FType.Pars[i])
			}
			i += 1
			iter = iter.Right
		}

		while iter != null
		{
			if iter.GetType() == GetType("float")
			{
				iter = BoxExc(iter,GetType("double"))
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
	UseCall := virtual !(sfile f) -> void
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
	GetType := virtual !() -> Type^
	{
		return ToCall.MyFuncType.RetType
	}
	UseCall := virtual !(sfile f) -> void
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
						f << GetOutputName()
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
}

//PointCall
//MethodCall
//VirtualMethodCall
//MethodPointCall
//Operator()


