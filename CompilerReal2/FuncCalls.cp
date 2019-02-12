#import "Tree.cp"
#import "GetExc.cp"
#import "ParamCall.cp"
#import "PtrToRef.cp"
#import "FuncInputBox.cp"
#import "DebugStuf.cp"


FillAttrs := !(FuncInputBox itBox,Object^ itm) -> void
{	
	if itm != null{
		for v,k : itm.Line.itAttrs
		{
			//printf("adding %s\n",k)
			itBox.itAttrs[k] = v
		}
	}

	itr := itm
	while itr != null
	{
		if itr is BoxFuncBody
			break
		itr = itr.Up
	}
	if itr == null return void

	asFunc := itr->{BoxFuncBody^}

	for val,key : asFunc.ItAttrs
	{	
		if itBox.itAttrs.TryFind(key) == null
			itBox.itAttrs[key] = val
	}
}

GetFuncCall := !(Object^ ToParse) -> Object^
{
	iter := ToParse

	if iter == null return null

	if iter is ObjIndent //iter.GetValue() == "~ind"
	{
		asInd := iter->{ObjIndent^}
		box := new FuncInputBox() ; $temp 
		box.itConsts.Push(new ObjStr(asInd.MyStr)) 
		someF := FindFunc(".",iter,box^,true)

		if someF != null
		{
			return MakeSimpleCall(someF,null->{Object^})
		}
	}

	if iter.Down != null and iter.Down.Right != null and iter.Down.Right.GetValue() == "is"
	{
		asCl1 := iter.Down.GetType()
		if not(asCl1 == null or asCl1 is TypePoint or asCl1.Base is TypeClass) return null
		asCl2 := ParseType(iter.Down.Right.Right)
		if asCl2 == null or not asCl2 is TypeClass return null

		asNeed := asCl1.Base->{TypeClass^}.ToClass
		
		itBox := new FuncInputBox()  ; $temp

		itBox.itPars.Emplace(asCl1.Base,true)
		itBox.itConsts.Push(new ObjType(asCl2)) 
		
		func := asNeed.VirtualCheck.GetFunc(itBox^)
		if func == null return null
		return MakeSimpleCall(func,iter.Down)

	}

	iter = ToParse.Down
	if iter == null return null

	if iter.GetValue() == "!"
	{
		if iter.Right != null
		if iter.Right.GetValue() == "[]"
		{
			tryCmp := TryCompute(ToParse)
			if tryCmp != null{
				return tryCmp
			}
			box := new FuncInputBox()  ; $temp

			iterY := iter.Right.Down

			while iterY != null
			{
				if iterY.GetValue() != ","
				{
					box.itPars.Emplace(iterY.GetType(),iterY.IsRef())
				}
				iterY = iterY.Right
			}
			f := FindFunc("![]",iter,box^,false)

			if f != null
			{
				TrimCommas(iter.Right)
				return MakeSimpleCall(f,iter.Right.Down)
			}
			
		}
		//if iter.Right.GetValue() == "{}" or iter.Right.GetValue() == "{d}"
		//{
		//	box := new FuncInputBox() ; $temp

		//	iterY := iter.Right.Down

		//	while iterY != null
		//	{
		//		if iterY.GetValue() != ","
		//		{
		//			box.itPars.Emplace(iterY.GetType(),iterY.IsRef())
		//		}
		//		iterY = iterY.Right
		//	}

		//	CTT2 := CTT->{BoxTemplate^}

		//	f := CTT2.GetFunc(box^)
		//	if f != null
		//	{
		//		TrimCommas(iter.Right)
		//		return MakeSimpleCall(f,iter.Right.Down)
		//	}
		//}
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
	if iter is ObjType //iter.GetValue() == "~type"
	{
		asNeed := iter->{ObjType^}
		fastType = asNeed.MyType
	}

	if fastType != null
	{
		//asNeed := iter->{ObjType^}
		asNeed2 := fastType

		if asNeed2 is TypeClass and iter.Right.GetValue() == "()"
		{
			asNeed3 := asNeed2->{TypeClass^}
			asNeed4 := asNeed3.ToClass
			
			box := new FuncInputBox()  ; $temp
			FillAttrs(box^,iter)
			box.itPars.Emplace(asNeed2,true) 
			TrimCommas(iter.Right)

			iter2 := iter.Right.Down

			while iter2 != null
			{
				box.itPars.Emplace(iter2.GetType(),iter2.IsRef())
				iter2 = iter2.Right
			}

			func := asNeed4.GetFunc("this",box^,true)
			if func != null return new ConstructCall(func,iter.Right.Down)
		}


		if iter.Right.GetValue() == "->"
		{
			if iter.Right.Right is ObjIndent //iter.Right.Right.GetValue() == "~ind"
			{
				tt := iter.Right.Right
				asNeed := tt->{ObjIndent^}

				if iter is ObjType
				{
					box := new FuncInputBox()  ; $temp

					objT := iter->{ObjType^}

					box.itConsts.Push(objT)
					box.itConsts.Push(new ObjStr(asNeed.MyStr)) 
					func := FindFunc("->",iter,box^,false)
					if func != null {
						return MakeSimpleCall(func,null->{Object^})
					}
				}

				box := new FuncInputBox()  ; $temp


				box.itConsts.Push(iter)
				box.itConsts.Push(new ObjStr(asNeed.MyStr)) 
				
				func := FindFunc("->", iter,box^,false)
				if func != null return MakeSimpleCall(func,null->{Object^})
			}
		}

		if asNeed2 is TypeStandart or asNeed2 is TypePoint or asNeed2 is TypeFatArr
		{
			if iter.Right != null
			{
				if iter.Right.GetValue() == "()"
				{
					Cs := Queue.{Object^}()
					Cs.Push(new ObjType(asNeed2)) ; $temp 
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
		iterPre := iter

		consts := null->{Object^}
		if iter.GetValue() == "()"
		{
			if iter.Left.GetValue() == "(d)"
			{
				dynCast := (iter.Left)->{ParamCall^}

				if iter.Left.IsRef()  and iter.Left.GetType() is TypePoint
				{
					if iter.Left.GetType().Base is TypeFunc
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
					preRet := OneCall(dynCast.BeforeName, iter,consts,true)
					if preRet != null return preRet
				}
			}
			if true
			{
				iterL := iterPre.Left
				iterD := iterPre.Down

				box := new FuncInputBox()  ; $temp
				FillAttrs(box^,iter)

				box.itPars.Emplace(iterPre.Left.GetType(),true)

				while iterD != null
				{
					if iterD.GetValue() != ","
						box.itPars.Emplace(iterD.GetType(),iterD.IsRef()) 
					iterD = iterD.Right
				}

				//TODO: add consts
				
				plsF := FindFunc("()",iter,box^,false)

				if plsF != null
				{
					iterL.Right = iter.Down
					iter.Down.Left = iterL
					iter.SetUp(iterL.Up)
					return MakeSimpleCall(plsF,iterL)
				}
				return null
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
		if iter is ObjSuffix //iter.GetValue() == "~suffix"
		{
			if iter.Left.GetType() == null return null
			sBox := new FuncInputBox()  ; $temp

			AsSuf := iter->{ObjSuffix^}
			sBox.itPars.Emplace(iter.Left.GetType(),false)

			Func := FindSuffix(AsSuf.MyStr,iter,sBox^)
			if Func == null return null
			
			iter = iter.Left
			PopOutNode(iter.Right)
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
			if iter.Right is ObjIndent or iter.Right is ObjStr
			{
				asName := ""
				if iter.Right is ObjIndent
				{
					asIndent := (iter.Right)->{ObjIndent^} 
					asName = asIndent.MyStr
				}else{
					asStr := (iter.Right)->{ObjStr^}
					asName = asStr.GetString()
				}

				if iter.GetValue() == "->"
				{
					box := new FuncInputBox() ; $temp 
					box.itPars.Emplace(iter.Left.GetType(),iter.Left.IsRef())
					box.itConsts.Push(new ObjStr(asName))

					fun := FindFunc("->",iter,box^,false)
					iter = iter.Left
					iter.Right.Left = null
					iter.Right = null
					return MakeSimpleCall(fun,iter)
				}

				if iter.Right.Right != null
				{
					LL := iter.Left

					//if iter.Left.GetType().Ge == "standart"
					//{
					//	return null
					//}

					GotClass := true
					if  not LL.GetType() is TypeClass
					{
						if LL.GetType() is TypePoint	
						{
							if not LL.GetType().Base is TypeClass
								GotClass = false
						}else	GotClass = false
					}
					asClass := BoxClass^
					if GotClass
					{
						if LL.GetType() is TypePoint
						{
							asClass = ((iter.Left.GetType().Base)->{TypeClass^}).ToClass
						}else
						{
							asClass = ((iter.Left.GetType())->{TypeClass^}).ToClass
						}
					}else{
						asClass = null
					}

					box := new FuncInputBox()  ; $temp
					FillAttrs(box^,iter)

					if LL.inhAttrs != null
					{
						for v,k : LL.inhAttrs^
						{
							if box.itAttrs.TryFind(k) == null
								box.itAttrs[k] = v
						}
					}
					
					box.itPars.Emplace(iter.Left.GetType(),iter.Left.IsRef()) 
					iterK := iter.Right.Right.Down
					while iterK != null
					{
						if iterK.GetValue() != ","
							box.itPars.Emplace(iterK.GetType(),iterK.IsRef())
						iterK = iterK.Right
					}

					func := FindFunc(asName,iter,box^,true)
				
					if func == null and GotClass and asClass != null
					{
						box.itPars[0].first = asClass.ClassType
						func = asClass.GetFunc(asName,box^,false)
					}

					if func != null
					{
						TrimCommas(iter.Right.Right)
						iter = iter.Left
						iter.Right = iter.Right.Right.Right.Down
						if iter.Right != null iter.Right.Left = iter
						iter.SetUp(iter.Up)

						if iter.GetType() is TypePoint
						{
							iter = new PtrToRef(iter)
						}
						return  MakeSimpleCall(func,iter)
					}

					return null
				}else
				{

					asClass := BoxClass^
					asClass = null
					LT := iter.Left.GetType()


					if LT is TypeStandart
					{
						box1 := new FuncInputBox()  ; $temp
						box1.itPars.Emplace(LT,iter.Left.IsRef())
						box1.itConsts.Push(new ObjStr(asName)) 
						func := FindFunc(".",iter,box1^,false)
						if func == null return null
						return MakeSimpleCall(func,iter.Left)
					}

					if LT is TypeClass
					{
						asClass = ((LT->{TypeClass^}).ToClass)
					}else{
						if LT is TypePoint  and LT.Base is TypeClass
						{
							asClass = (((LT.Base)->{TypeClass^}).ToClass)
						}else{
							return null
						}
					}

					box2 := new FuncInputBox()  ; $temp

					box2.itPars.Emplace(asClass.ClassType,true)
					box2.itConsts.Push(new ObjStr(asName))
					pru := (asClass.UnrollTemplate^.GetPriority(box2^))
					roll := null->{BoxFunc^}
					if pru == 255 
					{
						roll = asClass.GetVirtualParamFunc(asName)
						if roll == null return null //TODO: check for user functions
					}else{
						roll =  (asClass.UnrollTemplate^.GetFunc(box2^))
					}
					if roll == null return null

					iter = iter.Left
					if iter.GetType() is TypePoint
					{
						iter = new PtrToRef(iter)
					}
					preRes :=  MakeSimpleCall(roll,iter)
					if preRes != null {
						preRes.inhAttrs = iter.inhAttrs
					}
					return preRes
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
					iter.Up.Print(0)
					iter.EmitError("no more then binary allowed\n")
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
		if iter.GetValue() == "delete"
		{
			return new DeleteCall(iter.Right)
		}
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
				if iter.Right.Right.GetValue() != "()"
				{
					irr := iter.Right.Right
					itB := new FuncInputBox()  ; $temp
					itB.itPars.Emplace(irr.GetType(),irr.IsRef())
					itB.itConsts.Push(new ObjType(useType)) 
					
					
					FillAttrs(itB^,iter)

					func := FindFunc("new",iter,itB^,true)
					if func != null
					{
						irr.Up.Down = irr
						itF := MakeSimpleCall(func,irr)
						return itF
					}
				}
				if iter.Right.Right.GetValue() == "()" return new NewCallOne(useType,iter.Right.Right) 
				//return new NewCall(useType,iter.Right.Right)
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
		if iter is ObjIndent //iter.GetValue() == "~ind"
		{
			if iter.Right == null return null

			if iter.Right.GetValue() == "()"
			{
				asInd := iter->{ObjIndent^}
				itName := asInd.MyStr

				return OneCall(itName,iter.Right,null->{Object^})
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
			if pars.GetType() is TypeClass
			{
				asNeedPre := pars.GetType()
				asNeed := asNeedPre->{TypeClass^}
				cls := asNeed.ToClass

				oBox := new FuncInputBox()  ; $temp
				FillAttrs(oBox^,pars)


				iter := pars
				while iter != null
				{
					oBox.itPars.Emplace(iter.GetType(),iter.IsRef())
					iter = iter.Right
				}

				newPre := cls.GetFunc(oper,oBox^,true)
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
	return OpersTree.Contain(may)
	iter := PriorityData.Opers.Start

	while iter != null
	{
		if iter.Data == may return true
		iter = iter.Next
	}
	return false

}

TrimCommas := !(Object^ up) -> void
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
				if H.IsConst
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
	box := new FuncInputBox()  ; $temp
	FillAttrs(box^,G)

	TrimCommas(G)
	P := G.Down

	iterT := P

	while iterT != null
	{
		box.itPars.Emplace(iterT.GetType(),iterT.IsRef()) 
		iterT = iterT.Right
	}
	for it : consts box.itConsts.Push(it) 

	SomeFunc := FindFunc(Name,G,box^,false)

	//if SomeFunc == null{
	//	inClass := GetUpClass(G)
	//	if inClass != null
	//	{	
	//		box2 := new FuncInputBox() ; $temp
	//		box2.itPars
	//		Ps.PushFront(inClass.ClassType)
	//		funcH := inClass.GetFunc(Name,box^,true)
	//		if funcH != null
	//		{
	//			daFunc := GetFuncBlock(G)
	//			if daFunc != null
	//			{
	//				thisParamCallPre := new FuncParam("this",(inClass.ClassType)->{Type^},true)
	//				thisParamCall := new ParamNaturalCall("this",thisParamCallPre->{Object^})


	//				if P != null
	//				{
	//					thisParamCall.Right = P
	//					P.Left = thisParamCall
	//					thisParamCall.Up = P.Up
	//					P.Up.Down = thisParamCall
	//					P = P.Left
	//				}else
	//				{
	//					P = thisParamCall
	//				}
	//				return MakeSimpleCall(funcH,P)
	//			}
	//		}

	//	}
	//}

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
		if FType == null {
			EmitError("compiler error?\n")
			Print(0)
			return false
		}
		if FType.RetRef return true
		return false
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		iter := Down
		while iter != null
		{
			iter.PrintGlobal(f)
			iter = iter.Right
		}
	}

	checkedReturn := bool
	CheckReturn := virtual !() -> void
	{
		if checkedReturn return void
		checkedReturn = true
		if ToCall != null and not gotAlloc
		{
			gotAlloc = ToCall.IsRetComplex
			if ToCall.MyFuncType.RetType != null
			{
				if not gotAlloc gotAlloc = ToCall.MyFuncType.RetType is TypeClass
				if not gotAlloc gotAlloc = ToCall.MyFuncType.RetType is TypeArr
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
			if FType.RetRef or ToCall.IsRetRef
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
		if ToCall != null
		{
			if FType.RetRef or ToCall.IsRetRef
			{
				f << " " << TEName
			}else{
				f << " " << TName
			}
		
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
			if FType.RetRef or ToCall.IsRetRef
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
		if Pars != null
		{
			Line = Pars.Line
			//TODO:set up line for empty params func()
		}
		Down = Pars
		
		RetId = GetNewId()
		ToCall = func
		ToCall.ParseBlock()
		FType = ToCall.MyFuncType
		if Pars != null Pars.SetUp(this&)
		if Pars != null TrimCommas(Down.Up)
		ExchangeParams()
		WorkBag.Push(this&,State_GetUse)
	}
	ExchangeParams := !() -> void
	{
		itLiner := this.Line
		lnr := this&->{Object^}

		if itLiner == null{
			wl := Down
			while wl != null{
				if wl.Line != null {
					itLiner = wl.Line
					lnr = wl
				}
				wl = wl.Right
			}
		}

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
					msg := "compiler bug at param " + (i+1) + " "
					msg = msg + " object " + iter.GetValue()
					if iter.GetType() != null{
						msg = msg + " from " + iter.GetType().GetName()
					}else {
						msg = msg + " from null "
					}
					if FType.Pars[i] != null {
						msg = msg + " to " + FType.Pars[i].GetName()
					}
				
					if itLiner != null lnr.EmitError(msg + "\n")
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
				if itType == GTypeFloat
				{
					iter = BoxExc(iter,GTypeDouble,false)
				}
				if itType is TypeArr
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

		if (FType.RetType != GTypeVoid and not gotAlloc and TName != null) or ToCall.IsRetRef
		{
			f << TName <<" = "	
		}
		f << "call "
		//ToCall.MyFuncType.PrintType(f)
		base := ToCall.MyFuncType
		base2 := base->{Type^}
		f << base2.GetName()
		PrintFuncName(f)
		f << "("
		PrintParamUses(f)
		f << ")"
		if DebugMode {
			newId := CreateDebugCall(this&) 
			if newId != -1{
				f << ", !dbg !" << newId
			}
		}
		if Line != null
		{
			f << "; Line: " << Line.LinePos << " File: " << Line.inFile.itStr 

		}
		f << "\n"
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

		if (FType.RetType != GTypeVoid)
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

						if miniIter == null
						{
							printf("nope %s\n",RealCall.FuncName)
						}

						ToAdd := string
						if RefsArr[num] ToAdd = miniIter.GetPointName()
						else ToAdd = miniIter.GetName()

						f << ToAdd
					}
				}
				if AsmLine[j] == 'd'
				{
					j += 1
					if DebugMode {
						newId := CreateDebugCall(this&)
						if newId != -1
						{
							f << ", !dbg !" << newId
						}
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
		ResultType = GTypeInt
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
			box := new FuncInputBox()  ; $temp
			box.itConsts.Push(new ObjType(newType)) 
			FillAttrs(box^,this&)

			if Line != null {
				for v,k : Line.itAttrs box.itAttrs[k] = v
			}

			func := BoxFunc^
			func = null

			if newType is TypeClass
			{
				asTPre := newType->{TypeClass^}
				asT := asTPre.ToClass
				func = asT.GetFunc("new",box^,true)
			}

			if func == null{
				func = FindFunc("new",this&,box^,false)
			}

			if func == null 
			{
				//TODO: Class.Get
				EmitError("cant create type\n")
			}else{
				func.Line = Line
				newItm = MakeSimpleCall(func,null->{Object^})
				newItm.Line = Line
				newItm.Up = this&

				if useConstr
				{
					if newType is TypeClass
					{
						box := new FuncInputBox()  ; $temp

						box.itPars.Emplace(newType,true)
						iter3 := Down

						while iter3 != null
						{
							box.itPars.Emplace(iter3.GetType(),iter3.IsRef())
							iter3 = iter3.Right
						}

						asNeed := newType->{TypeClass^}


						constrFunc := asNeed.ToClass.GetFunc("this",box^,false)

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
							Down.Line = Line
							Down.Up = this&
						}
					}
				}
			}
		}
	}
}

DeleteCall := class extend SomeFuncCall
{
	DestructFuncCall := Object^
	DeleteFuncCall := Object^	
	this := !(Object^ itm) -> void
	{
		PopOutNode(itm)
		Down = itm
		itm.SetUp(this&)
		if not (itm.GetType() is TypePoint or itm.GetType() is TypeFatArr)
		{
			EmitError("only pointer could be deleted\n")
		}else{
			WorkBag.Push(this&,State_GetUse)
			WorkBag.Push(this&,State_PreGetUse)
		}
		Line = itm.Line
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_GetUse
		{
			box := new FuncInputBox()  ; $temp

			box.itPars.Emplace(VoidPType,false)
			box.itConsts.Push(new ObjType(Down.GetType())) 

			func := FindFunc("delete",Up,box^,false)

			if func != null
			{
				func.Line = Line
				DeleteFuncCall = MakeSimpleCall(func,new LinkForThis(Down,Down.GetType(),true))
				DeleteFuncCall.Up = this&

				//if Down.GetType().Base.GetType() == "class" and Down.GetType().GetType() == "point"
				//{
				//	itTypPre := Down.GetType().Base
				//	itTyp := itTypPre->{TypeClass^}

				//	pars2 := Queue.{Type^}()
				//	pars2.Push(itTypPre)
				//	func2 := itTyp.ToClass.GetFunc("~this",pars2)

				//	if func2 != null
				//	{
				//		halfCall := new LinkForThis(Down,Down.GetType(),true)
				//		ptrCall := new PtrToRef(halfCall->{Object^})
				//		DestructFuncCall = MakeSimpleCall(func2,ptrCall)
				//		DestructFuncCall.Up = this&
				//	}else{
				//		EmitError("Software error 4251354\n") 
				//	}
				//}else {
				//	if Down.GetType().GetType() != "point"
				//	{
					if UseDestructors
					{
						box2 := new FuncInputBox()  ; $temp
						box2.itPars.Emplace(Down.GetType(),Down.IsRef())
						func2 := FindFunc("~this",this&,box2^,true)
						if func2 != null
						{
							halfCall := new LinkForThis(Down,Down.GetType(),true)
							DestructFuncCall = MakeSimpleCall(func2,halfCall)
							DestructFuncCall.Line = Line
							DestructFuncCall.Up = this&
						}
					}
					//}
				//}
			}else{
				EmitError("can not delete item\n")
			}
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		if DeleteFuncCall != null
		{
			Down.PrintPre(f)
			if DestructFuncCall != null
			{
				DestructFuncCall.PrintInBlock(f)
			}
			DeleteFuncCall.PrintInBlock(f)
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

			
			if toCreate is TypeClass
			{
				b := new FuncInputBox()  ; $temp
				b.itPars.Emplace(toCreate,true)
				FillAttrs(b^,toCr)

				iter := ConstrPars
				while iter != null
				{
					b.itPars.Emplace(iter.GetType(),iter.IsRef())
					iter = iter.Right
				}
				asClass := toCreate->{TypeClass^}
				func := asClass.ToClass.GetFunc("this",b^,true)
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
		oldUp := toCr.Up
		box := new FuncInputBox()  ; $temp
		//ResultType = toCreate.GetPoint()
		//Down = new TypeSizeCall(toCreate)
		Down = toCr
		Down.SetUp(this&)

		box.itPars.Emplace(GTypeInt,false)
		box.itConsts.Push(new ObjType(toCreate))
		FillAttrs(box^,oldUp)

		fun := FindFunc("new",this&,box^,false)
		if fun == null
		{
			toCr.EmitError("can not create new obj\n")
			return void
		}
		ResultType = fun.MyFuncType.RetType
		ExtraFunc = MakeSimpleCall(fun,Down)
		ExtraFunc.Down.SetUp(ExtraFunc)
		CheckReturn()
	}
	UseCall := virtual !(sfile f) -> void
	{
		ExtraFunc.UseCall(f)
		if ResultType.Base is TypeClass
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
				TName = "%ToRet"
			}else
			{
				//if not ToCall.IsRetRef wut???
				//{
					gotAlloc = true
					InAlloc = GetAlloc(this&,ToCall.MyFuncType.Pars[0])
					TName = "%T" + InAlloc
					//TEName = "%TE" + RetId
				//}
			}
		}
	}
	FillConsts := !(sfile f) -> void
	{
		asNeed3 :=  ToCall.MyFuncType.Pars[0]
		if asNeed3 is TypeClass
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
	keepClean := bool
	this := !(Object^ toCopy,Type^ tp,bool isKeep) -> void
	{
		Link = toCopy
		ResultType = tp
		keepClean = isKeep
	}
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
		if keepClean
		{
			Link.PrintPointUse(f)
		}else{
			Link.PrintUse(f)
		}
	}
	PrintPre := virtual !(sfile f) -> void
	{
		//if keepClean Link.PrintPre(f)
	}
	PrintUse := virtual !(sfile f) -> void
	{
		if keepClean Link.PrintUse(f)
	}
	GetName := virtual !() -> string
	{
		if keepClean
		{
			return Link.GetName()
		}
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


