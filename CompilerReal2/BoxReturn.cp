BoxReturn := class extend Object
{
	IsRetRef := bool
	IsRetComplex := bool
	IsRetVoid := bool

	IsYield := bool
	YieldId := int
	ResetYield := bool
	RetFuncType := Type^

	returnLabel := BoxLabel^
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
					if IsBoxFuncContainer(ii)
					{
						cntr := ii->{BoxFuncContainer^}
						YieldId = cntr.AddYodler(this&)
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
					if IsBoxFuncContainer(ii)
					{
						if ii->{BoxFuncContainer^}.Yodlers.Size() != 0
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

			while iterF?.GetValue() != "!()" and iterF.GetValue() != "x=>x"
			{
				iterF = iterF.Up
			}

			if iterF != null
			{
				RetFunc := TypeFunc^


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
								if RetFunc.RetRef and Down is ObjNULL
								{
									//BoxExc(Down,PreType.GetPoint(),true)
									RetFuncType = RetFunc.RetType

								}else{
									printf("from %s to %p\n",Down.GetType().GetName(),PreType)
									EmitError("Can not return value\n")
								}
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
				if IsYield
				{
					returnLabel = Up.GetOutPath(this&,PATH_YIELD,0)
				}else{
					returnLabel = Up.GetOutPath(this&,PATH_RETURN,0)
				}
				assert(returnLabel != null)
			}
		}
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		debId := -1
		if DebugMode
			debId = CreateDebugCall(this&)
		if (not IsRetComplex) and (not IsRetVoid)
		{

			retTypeName := Down.GetType().GetName()
			if IsRetRef 
			{
				if RetFuncType != null
				{
					retTypeName = (""sbt + RetFuncType.GetName() + "*" <-)
				}else{
					retTypeName = (""sbt + retTypeName + "*" <-)
				}
			}
			if IsRetRef Down.PrintPointPre(f) else	Down.PrintPre(f)
			f << "store " 
			if IsRetRef {
				if RetFuncType != null
				{
					f << retTypeName << " null"
				}else{
					Down.PrintPointUse(f) 
				}
			}else {
				Down.PrintUse(f)
			}
			f << " , " << retTypeName << "* %Result"
			if debId != -1
				f << ", !dbg !" << debId
			f << "\n"
			
			tp := Down.GetType()
			if tp is TypePoint and tp.Base is TypeClass and tp.Base->{TypeClass^}.ToClass.IsGC
			{
				id := GetNewId()
				f << "%AsVoidP" << id << " = bitcast " << retTypeName << "* %Result to i8**\n"
				f << "%AsVoid" << id << " = load  i8*,i8** %AsVoidP"<< id <<"\n"
				f << "call void@" << gcIncRefFunc.OutputName << "(i8* %AsVoid"<< id <<")"
				if debId != -1
					f << ", !dbg !" << debId
				f << "\n"
			}
			
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
		f << "br label %" << returnLabel.GetLabel()

		f << "\n"
		if IsYield
		{
			f << "Yield" << YieldId << ":\n"
		}
	}
	GetOutPath := virtual !(Object^ ob, int typ, int size) -> BoxLabel^
	{
		return Up.GetOutPath(this&,typ,size)
	}
	GetValue := virtual !() -> string
	{
		return "~Return()"
	}
}
