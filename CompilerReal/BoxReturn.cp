BoxReturn := class extend Object
{
	IsRetRef := bool
	IsRetComplex := bool
	IsRetVoid := bool
	this := !(Object^ toUse) -> void
	{
		PopOutNode(toUse.Down)
		Down = toUse.Down
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
			Down.SetUp(this&)
		}
		if pri == State_Syntax
		{
			if Down.GetValue() == "~ind"
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
			if lazy lazy = iterF.GetValue() != "!()"
			while lazy
			{
				iterF = iterF.Up
				lazy = iterF != null
				if lazy lazy = iterF.GetValue() != "!()"
			}
			if iterF != null
			{
				asNeed := iterF->{BoxFunc^}
				IsRetComplex = asNeed.IsRetComplex
				IsRetRef = asNeed.MyFuncType.RetRef
				if Down.GetType() != null
				{
					PreType := asNeed.MyFuncType.RetType
					if PreType == null
					{
						asNeed.SetReturnType(Down.GetType())
					}else
					{
						if PreType != Down.GetType()
						{
							if TypeCmp(Down.GetType(),PreType) != 255
							{
								Down = BoxExc(Down,PreType,asNeed.MyFuncType.RetRef)
								Down.Up = this&
							}else{
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
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		if not IsRetComplex and not IsRetVoid
		{
			if IsRetRef Down.PrintPointPre(f) else	Down.PrintPre(f)
			f << "ret "
			if IsRetRef Down.PrintPointUse(f) else Down.PrintUse(f)
			f << "\n"
		}else{
			if not IsRetVoid Down.PrintInBlock(f)
			f << "ret void\n"
		}
	}
	GetValue := virtual !() -> string
	{
		return "~Return()"
	}
}
