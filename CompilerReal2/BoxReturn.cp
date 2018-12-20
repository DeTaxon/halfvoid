#import "Tree.cp"

BoxReturn := class extend Object
{
	IsRetRef := bool
	IsRetComplex := bool
	IsRetVoid := bool
	OutPathName := string
	this := !(Object^ toUse) -> void
	{
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
		if pri == State_GetUse
		{
			Down.SetUp(this&)

			iterF := Up

			lazy := iterF != null
			if lazy lazy = iterF.GetValue() != "!()" and iterF.GetValue() != "x=>x"
			while lazy
			{
				iterF = iterF.Up
				lazy = iterF != null
				if lazy lazy = iterF.GetValue() != "!()" and iterF.GetValue() != "x=>x"
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
		if not IsRetComplex and not IsRetVoid
		{
			//if IsRetRef Down.PrintPointPre(f) else	Down.PrintPre(f)
			//f << "ret "
			//if IsRetRef Down.PrintPointUse(f) else Down.PrintUse(f)
			//f << "\n"

			retTypeName := Down.GetType().GetName()
			if IsRetRef retTypeName = retTypeName + "*"
			if IsRetRef Down.PrintPointPre(f) else	Down.PrintPre(f)
			f << "store " 
			if IsRetRef Down.PrintPointUse(f) else Down.PrintUse(f)
			f << " , " << retTypeName << "* %Result\n"
			
		}else{
			if not IsRetVoid Down.PrintInBlock(f)
			//f << "ret void\n"
		}
		f << "br label %" << OutPathName << "\n"
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
