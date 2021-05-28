TypeCmpForWhileIf := !(Type^ from,Type^ out) -> int
{
	if from == null or from.GetType() == null
		return 255
	extrCmp := 255
	if from.GetType() ==  "point" extrCmp = 1
	if from.GetType() == "fatarr" extrCmp = 1

	realCmp := TypeCmp(from,out)
	if extrCmp < realCmp
		return extrCmp
	return realCmp
}

BoxIf := class extend Object
{
	MyId := int
	ForceGo := int 
	onBadLabel := BoxLabel^
	this := !(Object^ DaRet) -> void
	{
		Down = DaRet.Down
		MyId = GetNewId()
		onBadLabel = new BoxLabelAnon()
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if visitedWork[pri] return void
		else visitedWork << pri

		if pri == State_Start
		{
			Down.SetUp(this&)
			compRes := TryCompute(Down.Right)
			if compRes? is ObjBool
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
				//WorkBag.Push(Down[^],State_Start)
				WorkBag.Push(Down,State_Start)
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
			if Down is ObjBool
			{
				if Down->{ObjBool^}.MyBool
				{
					ForceGo = 1
				     	WorkBag.Push(Down.Right,State_Start)
				}else{
					ForceGo = 2
					if Down.Right.Right != null
						WorkBag.Push(Down.Right.Right,State_Start)
				}
			}else{
				WorkBag.Push(Down.Right[^],State_Start)
				if Down.GetType() != GTypeBool
				{
					if TypeCmpForWhileIf(Down.GetType(),GTypeBool) != 255
					{
						if BoxExc(Down,GTypeBool,false) == null
							EmitError("compiler error: can not convert\n")
					}else{
						EmitError("can not use type as if statement\n")
					}
				}
			}
		}
	}
	PrintInBlock := virtual !(TIOStream f) -> void
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
			f << ", label %OnTrue" << MyId << " , label %" << onBadLabel.GetLabel() << "\n"
			f << "OnTrue" << MyId << ":\n"
			Down.Right.PrintInBlock(f)
			f << "br label %" << onBadLabel.GetLabel() << "\n"
			onBadLabel.PrintLabel(f)

		}else{
			Down.PrintPre(f)
			f << "br "
			Down.PrintUse(f)
			f << ", label %OnTrue" << MyId << " , label %" << onBadLabel.GetLabel() << "\n"
			f << "OnTrue" << MyId << ":\n"
			Down.Right.PrintInBlock(f)
			f << "br label %End" << MyId << "\n"
			onBadLabel.PrintLabel(f)
			Down.Right.Right.PrintInBlock(f)
			f << "br label %End" << MyId << "\n"
			f << "End" << MyId << ":\n"
		}
	}
	GetOutPath := virtual !(Object^ frm,int typ , int siz) -> BoxLabel^
	{
		if Up != null
		{	
			return Up.GetOutPath(this&,typ,siz)
		}
		return null
	}
	GetBadLabel := virtual !(Object^ prevNode) -> BoxLabel^
	{
		return onBadLabel
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
		labelEnd = new BoxLabelAnon()
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
			WorkBag.Push(Down[^],State_Start)
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			if Down.GetType() != GTypeBool
			{
				if TypeCmpForWhileIf(Down.GetType(),GTypeBool) != 255
				{
					if BoxExc(Down,GTypeBool,false) == null
						EmitError("compiler error: can not convert\n")
				}else{
					EmitError("can not use type as while statement\n")
				}
			}
		}
	}
	
	labelEnd := BoxLabel^
	labelContinue := BoxLabel^
	labelBreak := BoxLabel^
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		if Down.Right.Right == null
		{
			f << "br label %Check" << MyId << "\n"
			f << "Check" << MyId << ":\n"
			Down.PrintPre(f)
			f << "br "
			Down.PrintUse(f)
			f << ", label %OnTrue" << MyId << " , label %" << labelEnd.GetLabel() << "\n"
			f << "OnTrue" << MyId << ":\n"
			Down.Right.PrintInBlock(f)
			f << "\nbr label %Check" << MyId << "\n"
			
			if labelContinue != null
			{
				labelContinue.PrintLabel(f)
				f << "br label %Check" << MyId << "\n"
			}

			if labelBreak != null
			{
				labelBreak.PrintLabel(f)
				f << "br label %" << labelEnd.GetLabel() << "\n"
			}

			labelEnd.PrintLabel(f)
		}

	}

	GetOutPath := virtual !(Object^ itm, int typ, int size) -> BoxLabel^
	{
		if typ == PATH_RETURN
		{
			return Up.GetOutPath(this&,typ,size) //poke
		}
		if typ == PATH_CONTINUE
		{
			if size == 0{
				if labelContinue == null
					labelContinue = new BoxLabelAnon()
				return labelContinue
			}
			return Up.GetOutPath(itm,typ,size - 1)
		}
		if typ == PATH_BREAK
		{
			if size == 0{
				if labelBreak == null
					labelBreak = new BoxLabelAnon()
				return labelBreak
			}
			return Up.GetOutPath(itm,typ,size - 1)
		}
		return null
	}

	GetBadLabel := virtual !(Object^ prebNode) -> BoxLabel^
	{
		return labelEnd
	}
	GetValue := virtual !() -> string
	{
		return "~while()"
	}
}
