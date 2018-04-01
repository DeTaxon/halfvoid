
GetFuncCall := !(Object^ ToParse) -> Object^
{
	iter := ToParse

	if iter == null return null

	if iter.GetValue() == "~ind"
	{
		asInd := iter->{ObjIndent^}
		
		funcData := Queue.{Type^}()
		consts := Queue.{Object^}()
		consts.Push(new ObjStr(asInd.MyStr)) 
		someF := FindFunc(".",iter,funcData,consts)

		if someF != null
		{
			return MakeSimpleCall(someF,null->{Object^})
		}

	}

	iter = ToParse.Down
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

				if iter.Left.GetType().GetType() == "point"
				{
					if iter.Left.GetType().Base.GetType() == "function"
					{
						iterL := iter.Left
						iterLT := iterL.GetType()
						printf("wow\n")
						return new PointFuncCall((iterLT.Base)->{TypeFunc^},iter.Down,iterL)
					}
				}else{
					return OneCall(dynCast.BeforeName, iter)
				}
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

				if iter.Right.Right != null
				{
					LL := iter.Left
					if  LL.GetType().GetType() != "class" 
					{
						if LL.GetType().GetType() == "point"	
						{
							if LL.GetType().Base.GetType() != "class"
								return null
						}else	return null
					}
					asClass := BoxClass^
					if LL.GetType().GetType() == "point"
					{
						asClass = ((iter.Left.GetType().Base)->{TypeClass^}).ToClass
					}else
					{
						asClass = ((iter.Left.GetType())->{TypeClass^}).ToClass
					}
					
					gg := Queue.{Type^}()
					//gg.Push(iter.Left.GetType())
					iterK := iter.Right.Right.Down
					while iterK != null
					{
						if iterK.GetValue() != ","
							gg.Push(iterK.GetType())
						iterK = iterK.Right
					}

					func := asClass.GetFunc(asName,gg)

					if func != null
					{
						iter = iter.Left
						iter.Right = iter.Right.Right.Right.Down
						if iter.Right != null	iter.Right.Left = iter
						iter.SetUp(iter.Up)

						if iter.GetType().GetType() == "point"
						{
							iter = new PtrToRef(iter)
						}
						return MakeSimpleCall(func,iter)
					}

					return null
				}else
				{

					asClass := BoxClass^
					asClass = null
					LT := iter.Left.GetType()

					if LT.GetType() == "class"
					{
						asClass = ((LT->{TypeClass^}).ToClass)
					}else{
						if LT.GetType() == "point" and LT.Base.GetType() == "class"
						{
							asClass = (((LT.Base)->{TypeClass^}).ToClass)
						}else{
							return null
						}
					}

					pars := Queue.{Type^}
					pars.Start = null
					pars.Push(asClass.ClassType)
					Consts := Queue.{Object^}()
					Consts.Push(new ObjStr(asName))
					pru := (asClass.UnrollTemplate^.GetPriority(pars,Consts))
					if pru == 255 return null //TODO: check for user functions
					roll :=  (asClass.UnrollTemplate^.GetFunc(pars,Consts))
					if roll == null return null

					iter = iter.Left
					if iter.GetType().GetType() == "point"
					{
						iter = new PtrToRef(iter)
					}
					return MakeSimpleCall(roll,iter)
				}
			}
		}
		if IsOper(iter.GetValue())
		{
			oper := iter.GetValue()
			if iter.Right == null
			{
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
		if iter.GetValue() == "new"
		{
			useType := ParseType(iter.Right)

			if useType == null
			{
				ErrorLog.Push("Incorrect new type\n")
				return null
			}
			if iter.Right.Right != null
			{
				return new NewCall(useType,iter.Right.Right)
			}else{
				return new NewCall(useType)
			}
		}
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

	if SomeFunc == null{
		inClass := GetUpClass(G)
		if inClass != null
		{
			Ps.PushFront(inClass.ClassType)
			funcH := inClass.GetFunc(Name,Ps)
			if funcH != null
			{
				daFunc := GetFuncBlock(G)
				if daFunc != null
				{
					thisParamCallPre := new FuncParam("this",(inClass.ClassType)->{Type^},true)
					thisParamCall := new ParamNaturalCall("this",thisParamCallPre->{Object^})


					if P != null
					{
						thisParamCall.Right = P
						P.Left = thisParamCall
						thisParamCall.Up = P.Up
						P.Up.Down = thisParamCall
						P = P.Left
					}else
					{
						P = thisParamCall
					}
					return MakeSimpleCall(funcH,P)
				}
			}

		}
	}

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
	FType := TypeFunc^

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
		if ToCall != null
		{
			if FType.RetRef
			{
				f << "%TE" << RetId << " = load "
				f << FType.RetType.GetName()
				f << " , "
				f << FType.RetType.GetPoint().GetName()
				f << " %T" <<RetId << "\n"
			}
		}
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
		FType.RetType.GetPoint().PrintType(f)
		f << " %T" << RetId
	}
	PrintUse := virtual !(sfile f) -> void
	{
		FType.RetType.PrintType(f)
		if FType.RetRef
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
		if ToCall != null
		{
			if FType.RetRef
			{
				return "%TE" + RetId
			}
		}
		return "%T" + RetId
	}
	GetOutputName := virtual !() -> string
	{
		return "%T" + RetId
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		PrintPre(f)
	}
	GetValue := virtual !() -> string
	{
		return "d()"
	}
	GetType := virtual !() -> Type^
	{
		return FType.RetType
	}
}

NaturalCall := class extend SomeFuncCall
{
	this := !(BoxFunc^ func, Object^ Pars) -> void 
	{
		Down = Pars
		
		RetId = GetNewId()
		ToCall = func
		FType = ToCall.MyFuncType
		if Pars != null Pars.SetUp(this&)
		ExchangeParams()
	}
	ExchangeParams := !() -> void
	{

		iter := Down
		i := 0

		while iter != null and i < FType.ParsCount 
		{
			if iter.GetType() != FType.Pars[i]
			{
				RetR := false
				if FType.ParsIsRef != null
					RetR = FType.ParsIsRef[i]
					
	
				preRet := BoxExc(iter,FType.Pars[i],RetR)
	
				if preRet == null
				{
					ErrorLog.Push("compiler bug\n")
				}else{
					iter = preRet
				}
			}
			i += 1
			iter = iter.Right
		}

		while iter != null
		{
			if iter.GetType() == GetType("float")
			{
				iter = BoxExc(iter,GetType("double"),false)
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
		RefsArr := FType.ParsIsRef
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
		RefsArr := FType.ParsIsRef
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

	UseCall := virtual !(sfile f) -> void
	{
		PrintPreFuncName(f)
		PrintParamPres(f)


		if (FType.RetType != GetType("void"))
		{
			f << "%T" << RetId <<" = "	
		}
		f << " call "
		FType.PrintType(f)
		PrintFuncName(f)
		f << "("
		PrintParamUses(f)
		f << ")\n"
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

PointFuncCall := class extend NaturalCall
{
	ParamCal := Object^
	this := !(TypeFunc^ funcT, Object^ Pars, Object^ pCall) -> void 
	{
		Down = Pars
		
		RetId = GetNewId()
		FType = funcT
		if Pars != null Pars.SetUp(this&)
		ExchangeParams()
	}
	UseCall := virtual !(sfile f) -> void
	{
		PrintPreFuncName(f)
		PrintParamPres(f)
		ParamCal.PrintPre(f)


		if (FType.RetType != GetType("void"))
		{
			f << "%T" << RetId <<" = "	
		}
		f << " call "
		FType.PrintType(f)
		f << ParamCal.GetName()
		f << "("
		PrintParamUses(f)
		f << ")\n"
	}
	Print := virtual !(int s) -> void {
		for s printf("->")
		printf("item: %s \n",GetValue())
		End := this.Down
		while End != null
		{
			End.Print(s+1)
			End = End.Right
		}
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
		FType = ToCall.MyFuncType
		if Pars != null Pars.SetUp(this&)
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
					num := 0

					while AsmLine[j] in '0'..'9'
					{
						num *= 10
						num += AsmLine[j] - '0'
						j += 1
					}

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
				if AsmLine[j] == '^'
				{
					numC := 0
					j += 1

					while AsmLine[j] in '0'..'9'
					{
						numC *= 10
						numC += AsmLine[j] - '0'
						j += 1
					}

					iterR := Down
					
					numC -= 1
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
					j += 1
				}
			}
		}

	}
}

TypeSizeCall := class extend SomeFuncCall
{
	ToCmp := Type^
	this := !(Type^ toCmp) ->void
	{
		RetId = GetNewId()
		ToCmp = toCmp
		ResultType = GetType("int")
	}
	PrintPointPre := virtual !(sfile f) -> void {	}
	PrintPre := virtual !(sfile f) -> void
	{
		f << "%TPre" << RetId << " = getelementptr "<<ToCmp.GetName()<< " , "<<ToCmp.GetPoint().GetName()<< " null, i32 1\n"
		f << "%T" << RetId << " = ptrtoint "<<ToCmp.GetPoint().GetName() << " %TPre" <<RetId<< " to i32\n"
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
	}
	PrintUse := virtual !(sfile f) -> void
	{
		f << " %T" << RetId
	}
	GetPointName := virtual !() -> string
	{
		return "%T" + RetId
	}
	GetName := virtual !() -> string
	{
		return "%T" + RetId
	}
	GetOutputName := virtual !() -> string
	{
		return "%T" + RetId
	}
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
}

NewCall := class extend SomeFuncCall
{
	ExtraFunc := SomeFuncCall^
	Constr := SomeFuncCall^
	ConstrPars := Object^

	this := !(Type^ toCreate) -> void
	{
		This2(toCreate,new ObjInt(1))
	}
	this := !(Type^ toCreate,Object^ toCr) -> void
	{
		if toCr.GetValue() == "()"
		{
			This2(toCreate,new ObjInt(1))
			ConstrPars = toCr.Down

			if ConstrPars != null
				ConstrPars.SetUp(null->{Object^})

			pars := Queue.{Type^}()
			
			if toCreate.GetType() == "class"
			{	
				pars.Push(toCreate)

				iter := ConstrPars
				while iter != null
				{
					pars.Push(iter.GetType())
					iter = iter.Right
				}
				asClass := toCreate->{TypeClass^}
				func := asClass.ToClass.GetFunc("this",pars)
				if func != null
				{
					Temp := ConstrPars
					ConstrPars = new LinkForThis(this&->{Object^},toCreate->{Type^})
					if Temp != null
					{
						ConstrPars.Right = Temp
						Temp.Left = ConstrPars
					}
					Constr = MakeSimpleCall(func,ConstrPars)
				}else{
					//ErrorLog.Push("constructor no found at " + Line.LinePos + "in file " +  Line.inFile)
					ErrorLog.Push("constructor no found")
				}

			}else{
			}
		}else{
			This2(toCreate,toCr)
		}
	}
	This2 := !(Type^ toCreate,Object^ toCr) -> void
	{
		ResultType = toCreate.GetPoint()
		Down = new TypeSizeCall(toCreate)
		Down.Right = toCr
		Down.SetUp(this&)

		outT := Queue.{Type^}()
		outT.Push(GetType("int"))
		outT.Push(GetType("int"))
		outC := Queue.{Object^}()
		outC.Push(new ObjType(toCreate))
		fun := (GlobalNew^.GetFunc(outT,outC))
		ExtraFunc = MakeSimpleCall(fun,Down)
		ExtraFunc.Down.SetUp(ExtraFunc)
	}
	UseCall := virtual !(sfile f) -> void
	{
		ExtraFunc.UseCall(f)
		Constr.PrintInBlock(f)
	}
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
	GetName := virtual !() -> string
	{
		return ExtraFunc.GetName()
	}
	PrintUse := virtual !(sfile f) -> void
	{
		ExtraFunc.PrintUse(f)
	}
}

LinkForThis := class extend Object
{
	Link := Object^
	ResultType := Type^
	this := !(Object^ toCopy,Type^ tp) -> void
	{
		Link = toCopy
		ResultType = tp
	}
	IsRef := virtual !() -> bool
	{
		return Link.IsRef()
	}
	GetValue := virtual !() -> char^
	{
		return Link.GetValue()
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
	}
	PrintPointPre := virtual !(sfile f) -> void
	{
		//Link.PrintPre(f)
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
		Link.PrintUse(f)
	}
	PrintPre := virtual !(sfile f) -> void
	{
	}
	PrintUse := virtual !(sfile f) -> void
	{
	}
	GetName := virtual !() -> string
	{
		return Link.GetPointName()
	}
	GetPointName := virtual !() -> string
	{
		return Link.GetPointName()
	}
	DoTheWork := virtual !(int st) -> void
	{
	}
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
}

//PointCall
//MethodCall
//VirtualMethodCall
//MethodPointCall
//Operator()


