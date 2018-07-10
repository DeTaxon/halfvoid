
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
		someF := FindFunc(".",iter,funcData,consts,true)

		if someF != null
		{
			return MakeSimpleCall(someF,null->{Object^})
		}
	}

	iter = ToParse.Down
	if iter == null return null

	if iter.GetValue() == "!"
	{
		if iter.Right != null
		if iter.Right.GetValue() == "[]"
		{
			Pars := Queue.{Type^}()

			iterY := iter.Right.Down

			while iterY != null
			{
				if iterY.GetValue() != ","
				{
					Pars.Push(iterY.GetType())
				}
				iterY = iterY.Right
			}
			f := FindFunc("![]",iter,Pars,false)

			if f != null
			{
				TrimCommas(iter.Right)
				return MakeSimpleCall(f,iter.Right.Down)
			}
			
		}
		return null
	}

	fastType := Type^
	fastType = null
	
	if iter.Right != null
	{
		if iter.Right.GetValue() == "()"
		{	
			fastType = ParseType(iter)
		}
	}
	if iter.GetValue() == "~type"
	{
		asNeed := iter->{ObjType^}
		fastType = asNeed.MyType
	}

	if fastType != null
	{
		//asNeed := iter->{ObjType^}
		asNeed2 := fastType

		if asNeed2.GetType() == "class" and iter.Right.GetValue() == "()"
		{
			asNeed3 := asNeed2->{TypeClass^}
			asNeed4 := asNeed3.ToClass

			Pars := Queue.{Type^}()
			Pars.Push(asNeed2)
			TrimCommas(iter.Right)

			cc := Queue.{Object^}()

			iter2 := iter.Right.Down

			while iter2 != null
			{
				Pars.Push(iter2.GetType())
				iter2 = iter2.Right
			}

			func := asNeed4.GetFunc("this",Pars,cc)
			if func != null return new ConstructCall(func,iter.Right.Down)
		}


		if iter.Right.GetValue() == "->"
		{
			if iter.Right.Right.GetValue() == "~ind"
			{
				tt := iter.Right.Right
				asNeed := tt->{ObjIndent^}

				pars := Queue.{Type^}()

				cc := Queue.{Object^}()
				cc.Push(iter)
				cc.Push(new ObjStr(asNeed.MyStr))
				
				func := FindFunc("->",iter,pars,cc,false)
				if func != null return MakeSimpleCall(func,null->{Object^})
			}
		}

		if asNeed2.GetType() == "standart"
		{
			if iter.Right != null
			{
				if iter.Right.GetValue() == "()"
				{
					Cs := Queue.{Object^}()
					Cs.Push(new ObjType(asNeed2))
					return OneCall(". this",iter.Right,Cs,false)
				}
			}
		}
		return null
	}
	if iter.GetType() != null
	{
		iter = iter.Right
		if iter == null return null

		consts := null->{Object^}
		if iter.GetValue() == "()"
		{
			if iter.Left.GetValue() == "(d)"
			{
				dynCast := (iter.Left)->{ParamCall^}

				if iter.Left.IsRef() //iter.Left.GetType().GetType() == "point"
				{
					if iter.Left.GetType().Base.GetType() == "function"
					{
						iterL := iter.Left
						iterLT := iterL.GetType()
						TrimCommas(iter)
						return new PointFuncCall((iterLT.Base)->{TypeFunc^},iter.Down,iterL)
					}
				}else{
					consts := null->{Object^}
					if iter.Right != null
					{
						if iter.Right.GetValue() == "."
						{
							if iter.Right.Right != null
							{
								if iter.Right.Right.GetValue() == "{}"
								{
									consts = iter.Right.Right
								}
							}
						}
					}
					return OneCall(dynCast.BeforeName, iter,consts)
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
			return OneCall("[]",iter.Up,null->{Object^})
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
		if iter.GetValue() == "." or iter.GetValue() == "->"
		{
			if iter.Right.GetValue() == "{}"
			{
				SyntaxCompress(iter.Right,PriorityData)
				typ := ParseType(iter.Right.Down)
				if typ == null return null

				fun := GetExchange(iter.Left,iter.Left,typ,iter.Left.IsRef())
				if fun == null return null
				iter = iter.Left
				iter.Right.Left = null
				iter.Right = null
				return MakeSimpleCall(fun,iter)
			}
			if iter.Right.GetValue() == "~ind"
			{
				asIndent := (iter.Right)->{ObjIndent^} 
				asName := asIndent.MyStr

				if iter.GetValue() == "->"
				{
					consts := Queue.{Object^}()
					pars := Queue.{Type^}()
					pars.Push(iter.Left.GetType())
					consts.Push(new ObjStr(asName))

					fun := FindFunc("->",iter,pars,consts,false)
					iter = iter.Left
					iter.Right.Left = null
					iter.Right = null
					return MakeSimpleCall(fun,iter)
				}

				if iter.Right.Right != null
				{
					LL := iter.Left

					if iter.Left.GetType() == "standart"
					{
						return null
					}

					GotClass := true
					if  LL.GetType().GetType() != "class" 
					{
						if LL.GetType().GetType() == "point"	
						{
							if LL.GetType().Base.GetType() != "class"
								GotClass = false
						}else	GotClass = false
					}
					asClass := BoxClass^
					if GotClass
					{
						if LL.GetType().GetType() == "point"
						{
							asClass = ((iter.Left.GetType().Base)->{TypeClass^}).ToClass
						}else
						{
							asClass = ((iter.Left.GetType())->{TypeClass^}).ToClass
						}
					}else{
						asClass = null
					}
					
					gg := Queue.{Type^}()
					gg.Push(iter.Left.GetType())
					iterK := iter.Right.Right.Down
					while iterK != null
					{
						if iterK.GetValue() != ","
							gg.Push(iterK.GetType())
						iterK = iterK.Right
					}

					func := FindFunc(asName,iter,gg,true)
				
					if func == null and GotClass and asClass != null
					{
						gg.Pop()
						func = asClass.GetFunc(asName,gg)
					}

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

					if LT.GetType() == "standart"
					{
						pars := Queue.{Type^}()
						pars.Push(LT)
						cc := Queue.{Object^}()
						cc.Push(new ObjStr(asName))
						func := FindFunc(".",iter,pars,cc,false)
						if func == null return null
						return MakeSimpleCall(func,iter.Left)
					}

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
				return OperFunc(oper,iter.Up.Down) //OneCall(oper,iter.Up,true)

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
					return  OperFunc(oper,iter.Up.Down) //OneCall(oper,iter.Up)
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
				if iter.Right.Right.GetValue() == "()" return new NewCallOne(useType,iter.Right.Right) 
				return new NewCall(useType,iter.Right.Right)
			}else{
				return new NewCallOne(useType,null->{Object^})
			}
		}else{
			if IsOper(iter.GetValue())
			{
				if iter.Right != null
				{
					if iter.Right.Right == null
					{
						name := ". " + iter.GetValue()
						iter = iter.Right
						PopOutNode(iter.Left)
						return OneCall(name,iter.Up,null->{Object^})
					}
				}
			}
		}
	}
	return null
}
OperFunc := !(string oper,Object^ pars) -> Object^
{
	preRet := OneCall(oper,pars.Up,null->{Object^},true)

	if preRet == null
	{
		if pars.GetType() != null
		{
			if pars.GetType().GetType() == "class"
			{
				asNeedPre := pars.GetType()
				asNeed := asNeedPre->{TypeClass^}
				cls := asNeed.ToClass

				Pars := Queue.{Type^}()

				iter := pars
				while iter != null
				{
					Pars.Push(iter.GetType())
					iter = iter.Right
				}

				newPre := cls.GetFunc(oper,Pars)
				if newPre == null return null
				return MakeSimpleCall(newPre,pars)
			}
		}
	}else{
		return preRet
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

TrimCommas := !(Object up) -> void
{
	if up.Down != null
	{
		Temp := up
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
}

OneCall := !(string Name, Object^ G,Object^ consts) -> Object^
{
	return OneCall(Name,G,consts,false)
}
OneCall := !(string Name, Object^ G,Object^ constsPre,bool ignoreNull) -> Object^
{
	Cs := Queue.{Object^}()

	if constsPre != null
	{
		H := constsPre.Down

		while H != null
		{
			if H.GetValue() != ","
			{
				if H.IsConst()
				{
					Cs.Push(H)
				}else{
					tp := ParseType(H)
					if tp == null
					{
						ErrorLog.Push("can not parse type in .{}\n")
					}else{
						Cs.Push(new ObjType(tp))
					}
				}
			}
			H = H.Right
		}
	}

	return OneCall(Name,G,Cs,ignoreNull)
}
OneCall := !(string Name, Object^ G,Queue.{Object^} consts,bool ignoreNull) -> Object^
{
	Ps := Queue.{Type^}()

	TrimCommas(G)
	P := G.Down

	iterT := P

	while iterT != null
	{
		Ps.Push(iterT.GetType())
		iterT = iterT.Right
	}

	SomeFunc := FindFunc(Name,G,Ps,consts,false)

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

	if SomeFunc == null 
	{
		if not ignoreNull ErrorLog.Push("Function <" + Name + "> not found\n") //TODO:  PointCall and closestFunc
	}
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

	IsConstr := bool
	gotAlloc := bool
	InAlloc := int

	TName := string
	TEName := string


	UseCall := virtual !(sfile f) -> void
	{
	}
	PrintPointPre := virtual !(sfile f) -> void
	{
		UseCall(f)
	}
	IsRef := virtual !() -> bool
	{
		if gotAlloc return true
		if ToCall.MyFuncType.RetRef return true
		return false
	}
	CheckReturn := virtual !() -> void
	{
		if ToCall != null and not gotAlloc
		{
			gotAlloc = ToCall.IsRetComplex
			if ToCall.MyFuncType.RetType != null
			{
				if not gotAlloc gotAlloc = ToCall.MyFuncType.RetType.GetType() == "class"
				if not gotAlloc gotAlloc = ToCall.MyFuncType.RetType.GetType() == "arr"
			}
			if ToCall.IsRetRef gotAlloc = false
			if ToCall.MyFuncType.RetRef gotAlloc = false //TOD: WHAT THE FUK?
			if gotAlloc
			{
				if Up.GetValue() == "~Return()"
				{
					TName = "%ToRet"
				}else{
					InAlloc = GetAlloc(this&,ToCall.MyFuncType.RetType)
					TName = "%T" + InAlloc
				}
				TEName = "%TE" + RetId
			}else
			{
				TName = "%T" + RetId
				TEName = "%TE" + RetId
			}
		}else{
			if not gotAlloc
			{
				TName = "%T" + RetId
				TEName = "%TE" + RetId
			}
		}
	}
	PrintPre := virtual !(sfile f) -> void
	{
		UseCall(f)
		if ToCall != null //TODO: ??? ToCall can not be null 
		{
			if FType.RetRef
			{
				f << TEName << " = load "
				f << FType.RetType.GetName()
				f << " , "
				f << FType.RetType.GetPoint().GetName()
				f << " " << TName << "\n"
			}
		}
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
		FType.RetType.GetPoint().PrintType(f)
		f << " " << TName
	}
	PrintUse := virtual !(sfile f) -> void
	{
		FType.RetType.PrintType(f)
		if FType.RetRef
		{
			f << " " << TEName
		}else{
			f << " " << TName
		}
	}
	GetItAllocId := !() -> int
	{
		if not gotAlloc return -1
		return InAlloc
	}
	GetPointName := virtual !() -> string
	{
		return TName
	}
	GetName := virtual !() -> string
	{
		if ToCall != null
		{
			if FType.RetRef
			{
				return TEName
			}
		}
		return TName
	}
	GetOutputName := virtual !() -> string
	{
		return TName
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
		ToCall.ParseBlock()
		FType = ToCall.MyFuncType
		if Pars != null Pars.SetUp(this&)
		ExchangeParams()
		WorkBag.Push(this&,State_GetUse)
	}
	ExchangeParams := !() -> void
	{

		iter := Down
		//if ToCall != null 
		//	if ToCall.IsRetComplex 
		//		iter = iter.Right //???
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
					EmitError("compiler bug\n")
				}else{
					iter = preRet
				}
			}
			i += 1
			iter = iter.Right
		}

		while iter != null
		{
			itType := iter.GetType()
			if itType != null
			{
				if itType == GetType("float")
				{
					iter = BoxExc(iter,GetType("double"),false)
				}
				if itType.GetType() == "arr"
				{
					iter = BoxExc(iter,iter.GetType().Base.GetPoint(),false)
				}
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
		if gotAlloc and not IsConstr
		{
			f << ToCall.MyFuncType.RetType.GetName() << "* "
			f << TName
			if iter != null f << " , "
			//i += 1
		}
		while iter != null
		{
			if i > 0  f << " , "
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

		if (FType.RetType != GetType("void") and not gotAlloc and TName != null)
		{
			f << TName <<" = "	
		}
		f << "call "
		ToCall.MyFuncType.PrintType(f)
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
		if pri == State_GetUse
		{
			CheckReturn()
		}
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
		ParamCal = pCall
		ExchangeParams()
		WorkBag.Push(this&,State_GetUse)
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		UseCall(f)
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
		f << "call "
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
	GetType := virtual !() -> Type^
	{
		return FType.RetType
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_GetUse
		{
			CheckReturn()
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
		WorkBag.Push(this&,State_GetUse)
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		PrintPre(f)
	}
	GetType := virtual !() -> Type^
	{
		return FType.RetType
	}
	UseCall := virtual !(sfile f) -> void
	{
		PrintPreFuncName(f)
		RefsArr := FType.ParsIsRef

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
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_GetUse
		{
			CheckReturn()
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

NewCallOne := class extend SomeFuncCall
{
	newItm := SomeFuncCall^
	newType := Type^
	ItId := int
	useConstr := bool
	this := !(Type^ nT,Object^ DW) -> void
	{
		useConstr = DW != null
		if DW != null	Down = DW.Down
		if Down != null
		{
			Down.Up.Down = null
			Down.SetUp(this&)
		}
		WorkBag.Push(this&,State_GetUse)

		ItId = GetNewId()
		newType = nT	
		ResultType = nT.GetPoint()

		if Down != null TrimCommas(this&->{Object^})
	}
	IsRef := virtual !() -> bool
	{
		return false
	}
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
	GetName := virtual !() -> string
	{
		//return newItm.GetName()
		return "%Pre" + ItId
	}
	PrintUse := virtual !(sfile f) -> void
	{
		//newItm.PrintUse(f)
		f << ResultType.GetName() << " %Pre" << ItId
	}
	PrintPre := virtual !(sfile f) -> void
	{
		newItm.PrintInBlock(f)
		f << "%Pre" << ItId << " = bitcast "
		newItm.PrintUse(f)
		f << " to " << ResultType.GetName() << "\n"

		if newType.GetType() == "class"
		{
			asNeed := newType->{TypeClass^}
			asNeed.ToClass.ApplyConstants(f,this&)
		}

		if Down != null Down.PrintInBlock(f)

	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		PrintPre(f)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_GetUse	
		{
			pars := Queue.{Type^}()
			cc := Queue.{Object^}()
			cc.Push(new ObjType(newType))
			func := FindFunc("new",this&,pars,cc,false)

			if func == null 
			{
				//TODO: Class.Get
				ErrorLog.Push("cant create type\n")
			}else{
				newItm = MakeSimpleCall(func,null->{Object^})

				if useConstr
				{
					if newType.GetType() == "class"
					{
						parsC := Queue.{Type^}()
						empCon := Queue.{Object^}()

						parsC.Push(newType)
						iter3 := Down

						while iter3 != null
						{
							parsC.Push(iter3.GetType())
							iter3 = iter3.Right
						}

						asNeed := newType->{TypeClass^}


						constrFunc := asNeed.ToClass.GetFunc("this",parsC,empCon)

						if constrFunc == null 
						{
							EmitError("can not find constructor\n")
						}else{
							extraF := new LinkForThis(this&->{Object^},newType)
							extraF.Right = Down
							extraF.Up = this&
							if Down != null Down.Left = extraF
							Down = extraF

							Down = MakeSimpleCall(constrFunc,Down)
						}
					}
				}
			}
		}
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
		//ResultType = toCreate.GetPoint()
		Down = new TypeSizeCall(toCreate)
		Down.Right = toCr
		Down.SetUp(this&)

		outT := Queue.{Type^}()
		outT.Push(GetType("int"))
		outT.Push(GetType("int"))
		outC := Queue.{Object^}()
		outC.Push(new ObjType(toCreate))
		fun := (GlobalNew^.GetFunc(outT,outC))
		ResultType = fun.MyFuncType.RetType
		ExtraFunc = MakeSimpleCall(fun,Down)
		ExtraFunc.Down.SetUp(ExtraFunc)
		CheckReturn()
	}
	UseCall := virtual !(sfile f) -> void
	{
		ExtraFunc.UseCall(f)
		if ResultType.Base.GetType() == "class"
		{
			asNeed := (((ResultType.Base)->{TypeClass^}).ToClass)
			asNeed.ApplyConstants(f,ExtraFunc)
		}
		if Constr != null
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

ConstructCall := class extend NaturalCall
{
	this := !(BoxFunc^ func, Object^ Pars) -> void 
	{
		IsConstr = true
		Down = new LinkForThis(this&->{Object^},func.MyFuncType.Pars[0])
		Down.Right = Pars
		if Pars != null Pars.Left = Down

		RetId = GetNewId()
		ToCall = func
		FType = ToCall.MyFuncType
		Down.SetUp(this&)
		ExchangeParams()
		WorkBag.Push(this&,State_GetUse)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_GetUse
		{
			
			if Up.GetValue() == "~Return()"
			{
				rights := Down.Right
				itm := GetItem("ToRet",this&)
				ReplaceNode(Down,new ParamNaturalCall("this",itm))
			}else
			{
				if not ToCall.IsRetRef 
				{
					gotAlloc = true
					InAlloc = GetAlloc(this&,ToCall.MyFuncType.Pars[0])
					TName = "%T" + InAlloc
					//TEName = "%TE" + RetId
				}
			}
		}
	}
	FillConsts := !(sfile f) -> void
	{
		asNeed3 :=  ToCall.MyFuncType.Pars[0]
		if asNeed3.GetType() == "class"
		{
			asNeed2 := asNeed3->{TypeClass^}
			asNeed := asNeed2.ToClass

			asNeed.ApplyConstants(f,this&)
		}
	}
	GetType := virtual !() -> Type^
	{
		return ToCall.MyFuncType.Pars[0]
	}
	PrintPointPre := virtual !(sfile f) -> void
	{
		FillConsts(f)
		UseCall(f)
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
		ToCall.MyFuncType.Pars[0].PrintType(f)
		f << "* " << TName
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		FillConsts(f)
		UseCall(f)
	}

	PrintPre := virtual !(sfile f) -> void
	{
		ErrorLog.Push("compiler error in func call, class asked by value")
	}
	PrintUse := virtual !(sfile f) -> void
	{
		//ErrorLog.Push("compiler error in func call, class asked by value")
		PrintPointUse(f) // жирный костыль
	}
	GetName := virtual !() -> string
	{
		ErrorLog.Push("compiler error in func call, class asked by value")
		return ""
	}
	GetPointName := virtual !() -> string
	{
		return TName
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
//MethodPointCall
//Operator()


