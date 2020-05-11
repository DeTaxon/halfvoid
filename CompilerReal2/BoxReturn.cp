BoxReturn := class extend Object
{
	IsRetRef := bool
	IsRetComplex := bool
	IsRetVoid := bool

	IsYield := bool
	YieldId := int
	ResetYield := bool

	OutPathName := string
	this := !(Object^ toUse) -> void
	{
		IsYield = toUse.Down.GetValue() == "yield"
		if toUse.Down.Right != null PopOutNode(toUse.Down)
		Down = toUse.Down
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_DestructGet)
			WorkBag.Push(this&,State_Syntax)
			Down.SetUp(this&)
		}
		if pri == State_Syntax
		{
			if IsYield
			{
				if Up != null 
					AllocSetStruct(Up)
				ii := Up
				while ii != null
				{
					if ii is SLambda
					{
						ii->{SLambda^}.Yodlers.Push(this&)
						YieldId = ii->{SLambda^}.Yodlers.Size()
						break
					}
					if ii is BoxFuncBody
					{
						asFunc := ii->{BoxFuncBody^}
						asFunc.YieldCheck()
						asFunc.Yodlers.Push(this&)
						YieldId = asFunc.Yodlers.Size()
						break
					}
					ii = ii.Up
				}
			}
			WorkBag.Push(this&,State_PreGetUse)
			if Down is ObjIndent
			{
				asNeed := Down->{ObjIndent^}
				if asNeed.MyStr == "void"
					IsRetVoid = true
			}
			if not IsRetVoid
			{
				WorkBag.Push(Down,State_Start)
				WorkBag.Push(this&,State_GetUse)
			}
		}
		if pri == State_PreGetUse
		{
			if not IsYield
			{
				ii := Up
				while ii != null
				{
					if ii is SLambda
					{
						if ii->{SLambda^}.Yodlers.Size() != 0
							ResetYield = true
						break
					}
					if ii is BoxFuncBody
					{
						if ii->{BoxFuncBody^}.Yodlers.Size() != 0
							ResetYield = true
						break
					}
					ii = ii.Up
				}
			}
		}
		if pri == State_GetUse
		{
			Down.SetUp(this&)

			iterF := Up

			askedDefer := false
			while iterF?.GetValue() != "!()" and iterF.GetValue() != "x=>x"
			{
				if iterF.GetValue() == "{d}"
				{
					asBl := iterF->{BoxBlock^}
					askedDefer = askedDefer or asBl.callDeferStuf
				}
				iterF = iterF.Up
			}

			if iterF != null
			{
				RetFunc := TypeFunc^

				if askedDefer
				{
					toSetUse := iterF.Down->{BoxBlock^}
					toSetUse.callDeferStuf = askedDefer
				}

				if iterF.GetValue() == "!()"{
					asNeed := iterF->{BoxFunc^}
					RetFunc = asNeed.MyFuncType
					IsRetComplex = asNeed.IsRetComplex
				}
				if iterF.GetValue() == "x=>x"{
					asNeed2 := iterF->{SLambda^}
					RetFunc = asNeed2.fastUse
					IsRetComplex = false
					if not RetFunc.RetRef IsRetComplex = IsComplexType(RetFunc.RetType)
				}

				IsRetRef = RetFunc.RetRef
				if Down.GetType() != null
				{
					PreType := RetFunc.RetType
					if PreType == null
					{
						asNeed := iterF->{BoxFunc^}
						asNeed.SetReturnType(Down.GetType())
					}else
					{
						if PreType != Down.GetType()
						{
							if TypeCmp(Down.GetType(),PreType) != 255
							{
								Down = BoxExc(Down,PreType,RetFunc.RetRef)
								Down.Up = this&
							}else{
								printf("from %s to %s\n",Down.GetType().GetName(),PreType.GetName())
								EmitError("Can not return value\n")
							}
						}
					}
				}else{
					EmitError("recursion detected\n")
				}
			}else{
				EmitError("impossible state\n")
			}
		}
		if pri == State_DestructGet
		{
			if Up == null
			{
				EmitError("error to get return path 24135\n")
			}else{
				OutPathName = Up.GetOutPath(this&,PATH_RETURN,0)
			}
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		debId := -1
		if DebugMode
			debId = CreateDebugCall(this&)
		if (not IsRetComplex) and (not IsRetVoid)
		{

			retTypeName := Down.GetType().GetName()
			if IsRetRef retTypeName = (""sbt + retTypeName + "*" <-)
			if IsRetRef Down.PrintPointPre(f) else	Down.PrintPre(f)
			f << "store " 
			if IsRetRef Down.PrintPointUse(f) else Down.PrintUse(f)
			f << " , " << retTypeName << "* %Result"
			if debId != -1
				f << ", !dbg !" << debId
			f << "\n"
			
		}else{
			if not IsRetVoid Down.PrintInBlock(f)
			//f << "ret void\n"
		}
		if IsYield
		{
			f << "store i32 " << YieldId << ", i32* %Yodler"
			if debId != -1
				f << ", !dbg !" << debId
			f << "\n"
		}
		if ResetYield
		{
			f << "store i32 0,i32* %Yodler\n"
			if debId != -1
				f << ", !dbg !" << debId
			f << "\n"
		}
		f << "br label %" << OutPathName 

		f << "\n"
		if IsYield
		{
			f << "Yield" << YieldId << ":\n"
		}
	}
	GetOutPath := virtual !(Object^ ob, int typ, int size) -> string
	{
		return Up.GetOutPath(this&,typ,size)
	}
	GetValue := virtual !() -> string
	{
		return "~Return()"
	}
}
