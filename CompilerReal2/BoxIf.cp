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
	GetOutPath := virtual !(Object^ frm,int typ , int siz) -> BoxLabel^
	{
		if Up != null
		{	
			return Up.GetOutPath(this&,typ,siz)
		}
		return null
	}
	GetValue := virtual !() -> string
	{
		return "~if()"
	}
	DoJIT := virtual !() -> void^
	{
		if ForceGo != 0
		{
			if ForceGo == 1
			{
				Down.Right.DoJIT()
			}else{
				if Down.Right.Right != null
					Down.Right.Right.DoJIT()
			}
			return null
		}

		if Down.Right.Right == null
		{
			onFalse := jit_label_undefined
			chk := Down.DoJIT()
			jit_insn_branch_if_not(JITCFunc, chk, onFalse&);
			Down.Right.DoJIT()
			jit_insn_label(JITCFunc,onFalse&)

		}else{
			toEnd := jit_label_undefined
			onFalse := jit_label_undefined
			chk := Down.DoJIT()
			jit_insn_branch_if_not(JITCFunc, chk, onFalse&);
			Down.Right.DoJIT()
			jit_insn_branch(JITCFunc,toEnd&)
			jit_insn_label(JITCFunc,onFalse&)
			Down.Right.Right.DoJIT()
			jit_insn_label(JITCFunc,toEnd&)
		}

		
		return null
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

	labelContinue := BoxLabel^
	labelBreak := BoxLabel^
	PrintInBlock := virtual !(sfile f) -> void
	{
		if Down.Right.Right == null
		{
			if callDeferStuf
				PrintDeferDepth(f,MyId,this&)	
			f << "br label %Check" << MyId << "\n"
			f << "Check" << MyId << ":\n"
			Down.PrintPre(f)
			f << "br "
			Down.PrintUse(f)
			f << ", label %OnTrue" << MyId << " , label %End" << MyId << "\n"
			f << "OnTrue" << MyId << ":\n"
			Down.Right.PrintInBlock(f)
			f << "\nbr label %Check" << MyId << "\n"
			
			if labelContinue != null
			{
				labelContinue.PrintLabel(f)
				if callDeferStuf
					PrintDeferApply(f,MyId,this&)
				f << "br label %Check" << MyId << "\n"
			}

			if labelBreak != null
			{
				labelBreak.PrintLabel(f)
				if callDeferStuf
					PrintDeferApply(f,MyId,this&)
				f << "br label %End" << MyId << "\n"
			}

			f << "End" << MyId << ":\n"

		}

	}
	DoJIT := virtual !() -> void^
	{

		if Down.Right.Right == null
		{
			onBeg := jit_label_undefined
			onEnd := jit_label_undefined
			
			jit_insn_label(JITCFunc,onBeg&)
			chk := Down.DoJIT()
			jit_insn_branch_if_not(JITCFunc,chk,onEnd&)
			Down.Right.DoJIT()
			jit_insn_branch(JITCFunc,onBeg&)
			jit_insn_label(JITCFunc,onEnd&)
		}
		
		return null
	}
	callDeferStuf := bool
	ApplyDeferUse := virtual !(int depth) -> void
	{
		if depth != 1
		{
			Up.ApplyDeferUse(depth - 1)
		}else{
			callDeferStuf = true
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

	GetValue := virtual !() -> string
	{
		return "~while()"
	}
}
