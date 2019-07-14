BoxIf := class extend Object
{
	MyId := int
	ForceGo := int 
	this := !(Object^ DaRet) -> void
	{
		Down = DaRet.Down
		MyId = GetNewId()
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if visitedWork[pri] return void
		else visitedWork << pri

		if pri == State_Start
		{
			Down.SetUp(this&)
			compRes := TryCompute(Down.Right)
			if compRes != null and compRes is ObjBool
			{
				ForceGo = 2
				if compRes->{ObjBool^}.MyBool
					ForceGo = 1
			}

			WorkBag.Push(this&,State_Syntax)
			PopOutNode(Down)
			if Down.Right.Right != null PopOutNode(Down.Right.Right)


			MakeItBlock(Down.Right)

			RR := Down.Right.Right
			if RR != null
				MakeItBlock(RR)
			if ForceGo == 0{
				while TryParseMacro(Down,this&) != null {}
			}
		}
		if pri == State_Syntax
		{
			switch ForceGo
			{
			case 0
				WorkBag.Push(Down[^],State_Start)
				WorkBag.Push(this&,State_GetUse)
			case 1
				WorkBag.Push(Down.Right,State_Start)
			case 2
				if Down.Right.Right != null
					WorkBag.Push(Down.Right.Right,State_Start)
			}
		}
		if pri == State_GetUse
		{
			if Down.GetType() != GTypeBool
			{
				if TypeCmp(Down.GetType(),GTypeBool) != 255
				{
					if BoxExc(Down,GTypeBool,false) == null
						EmitError("compiler error: can not convert\n")
				}else{
					EmitError("can not use type as if statement\n")
				}
			}
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		if ForceGo != 0
		{
			if ForceGo == 1
			{
				Down.Right.PrintInBlock(f)
			}else{
				if Down.Right.Right != null
					Down.Right.Right.PrintInBlock(f)
			}
			return void
		}
		if Down.Right.Right == null
		{
			Down.PrintPre(f)
			f << "br "
			Down.PrintUse(f)
			f << ", label %OnTrue" << MyId << " , label %End" << MyId << "\n"
			f << "OnTrue" << MyId << ":\n"
			Down.Right.PrintInBlock(f)
			f << "br label %End" << MyId << "\n"
			f << "End" << MyId << ":\n"

		}else{
			Down.PrintPre(f)
			f << "br "
			Down.PrintUse(f)
			f << ", label %OnTrue" << MyId << " , label %OnFalse" << MyId << "\n"
			f << "OnTrue" << MyId << ":\n"
			Down.Right.PrintInBlock(f)
			f << "br label %End" << MyId << "\n"
			f << "OnFalse" << MyId << ":\n"
			Down.Right.Right.PrintInBlock(f)
			f << "br label %End" << MyId << "\n"
			f << "End" << MyId << ":\n"
		}

	}
	GetOutPath := virtual !(Object^ frm,int typ , int siz) -> string
	{
		if Up != null
		{	
			return Up.GetOutPath(this&,typ,siz)
		}
		return ""
	}
	GetValue := virtual !() -> string
	{
		return "~if()"
	}
}

BoxWhile := class extend Object
{
	MyId := int
	this := !(Object^ DaRet) -> void
	{
		Down = DaRet.Down
		MyId = GetNewId()
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if visitedWork[pri] return void
		else visitedWork << pri

		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
			Down.SetUp(this&)
			PopOutNode(Down)

			MakeItBlock(Down.Right)
			while TryParseMacro(Down,this&) != null {}
		}
		if pri == State_Syntax
		{
			iter := Down

			while iter != null
			{
				WorkBag.Push(iter,State_Start)
				iter = iter.Right
			}
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			if Down.GetType() != GTypeBool
			{
				if TypeCmp(Down.GetType(),GTypeBool) != 255
				{
					if BoxExc(Down,GTypeBool,false) == null
						EmitError("compiler error: can not convert\n")
				}else{
					EmitError("can not use type as while statement\n")
				}
			}
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		if Down.Right.Right == null
		{
			f << "br label %Check" << MyId << "\n"
			f << "Check" << MyId << ":\n"
			Down.PrintPre(f)
			f << "br "
			Down.PrintUse(f)
			f << ", label %OnTrue" << MyId << " , label %End" << MyId << "\n"
			f << "OnTrue" << MyId << ":\n"
			Down.Right.PrintInBlock(f)
			f << "\nbr label %Check" << MyId << "\n"


			if UseRetPath
			{
				f << "br label %RetPath" << MyId << "\n"
				f << "RetPath" << MyId << ":\n"

				Down.Right[^].PrintDestructor(f)
				f << "br label %" << Up.GetOutPath(this&,PATH_RETURN,0) << "\n"
			}
			for itSize : ContPath
			{
				if itSize != 0{
					f << "br label %ContPath" << MyId << "id" << itSize <<"size\n"
					f << "ContPath" << MyId << "id" << itSize << "size:\n"

					Down.Right[^].PrintDestructor(f)

					f << "br label %" << Up.GetOutPath(this&,PATH_CONTINUE,itSize - 1) << "\n"
				}
			}
			for itSize : BreakPath
			{
				if itSize != 0{
					f << "br label %BreakPath" << MyId << "id" << itSize <<"size\n"
					f << "BreakPath" << MyId << "id" << itSize << "size:\n"

					Down.Right[^].PrintDestructor(f)
					
					f << "br label %" << Up.GetOutPath(this&,PATH_BREAK,itSize - 1) << "\n"
				}
			}

			f << "End" << MyId << ":\n"

		}

	}
	UseRetPath := bool
	ContPath := Set.{int}
	BreakPath := Set.{int}

	GetOutPath := virtual !(Object^ itm, int typ, int size) -> string
	{
		if typ == PATH_RETURN
		{
			UseRetPath = true
			if Up != null Up.GetOutPath(this&,typ,size) //poke
			return "RetPath"sbt + MyId
		}
		if typ == PATH_CONTINUE
		{
			if size == 0{
				return "Check" + MyId
			}
			ContPath.Add(size)
			return "ContPath"sbt + MyId + "id" + size + "size"
		}
		if typ == PATH_BREAK
		{
			if size == 0{
				return "End" + MyId
			}
			BreakPath.Add(size)
			return "BreakPath"sbt + MyId + "id" + size + "size"
		}
		return ""
	}

	GetValue := virtual !() -> string
	{
		return "~while()"
	}
}
