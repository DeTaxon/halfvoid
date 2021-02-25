InDataR := !(Object^ obj) -> bool
{
	if obj.IsDataR return true
	//switch obj.GetValue()
	//{
	//	//case "~null" return true
	//	//case "~d" return true
	//	//case "~ind" return true
	//	//case "~double" return true
	//	//case "~bool" return true
	//	//case "~int" return true
	//	//case "~str" return true
	//	//case "~cmd" return true
	//	//case "~{}type" return true
	//	//case "~type" return true
	//	//case "x=>x" return true
	//}
	return false
}

InBlockData := !(Object^ obj) -> bool
{
	if InDataR(obj) return true
	switch(obj.GetValue())
	{
		case  "return()" return true
		case  "if()" return true
		case  "while()" return true
		case  "for()" return true
		case  "switch()" return true
		case  "~continue" return true
		case  "~break" return true
	}

	return false
}

RuleUse := !(Object^ Obj,char^ N,!(void^)^->int R) -> bool
{
	GotStuff := false
	iter := Obj.Down

	while iter != null
	{
		Try := R(iter)
		if Try > 0
		{
			if N == "~d"
			{
				iter = UNext(iter,new ObjData(),Try) 
			}else
			{
				if N == "~extra1"
				{
					iter = UNext(iter,new SLambda(),Try)
				}else{
					iter = UNext(iter,new ObjObj(N),Try)
				}
			}
			GotStuff = true
		}else{
			iter = iter.Right
		}
	}
	return GotStuff
}

RuleUseReverse := !(Object^ Obj,char^ N,!(void^)^->int R) -> bool
{
	GotStuff := false
	iter := Obj.Down

	if iter == null return false

	while iter.Right != null iter = iter.Right

	while iter != null
	{
		Try := R(iter)
		if Try > 0
		{
			if N == "~d"
			{
				iter = UNext(iter,new ObjData(),Try) 
			}else
			{
				iter = UNext(iter,new ObjObj(N),Try) 
			}
			GotStuff = true
		}else{
			iter = iter.Left
		}
	}
	return GotStuff
}

RuleUseSome := !(Object^ Obj,char^ N, MiniMachineNode^ MiniNode) -> bool
{
	GotStuff := false
	iter := Obj.Down

	while iter != null
	{
		Try := RuleMachine(iter,MiniNode)
		if Try > 0
		{
			if N == "~d"
			{
				iter = UNext(iter,new ObjData(),Try) 
			}else
			{
				if N == "~extra1"
				{
					iter = UNext(iter,new SLambda(),Try)
				}else{
					iter = UNext(iter,new ObjObj(N),Try) 
				}
			}
			GotStuff = true
		}else{
			iter = iter.Right
		}
	}
	return GotStuff
}


StupidWhile := !(Object^ begin,PriorityBag^ bag ) -> bool
{
	//iter := begin.Down
	
	if RuleUse(begin,"~d",RuleMinus) return true

	GotStuff := false

	for iterBag : bag.Lines, iterRaw : bag.LinesRaw
	{
		if iterBag != null
		{
			if RuleUseSome(begin,"~d",iterBag) return true
		}else{
			switch iterRaw
			{
				case "#CallFunction"
					if RuleUse(begin,"~d",RuleCallFunction) return true
			}
		}
	}


	if RuleUse(begin,"~extra1",RuleSLambda) return true
	if RuleUse(begin,"Defer()",RuleDefer) return true
	if RuleUse(begin,"return()",RuleOneFunc) return true
	if RuleUse(begin,"i:=0",RuleParam) return true // for func 
	if RuleUse(begin,"i.{}:=0",RuleTypedefTemplate) return true // for func 
	if RuleUseReverse(begin,"if()",RuleIf) return true
	if RuleUse(begin,"while()",RuleWhile) return true
	if RuleUse(begin,"for()",RuleFor) return true
	if RuleUse(begin,"switch()",RuleSwitch) return true
	if RuleUse(begin,"case()",RuleCase) return true
	if RuleUse(begin,"imprt",RuleImport) return true

	return false	
}


SyntaxCompress := !(Object^ begin, PriorityBag^ bag) -> bool
{
	if begin == null return true

	GotWork := true

	itr := begin.Down
	while itr != null
	{
		if itr.GetValue() == "()" SyntaxCompress(itr,bag)
		itr = itr.Right
	}

	while GotWork GotWork = StupidWhile(begin,bag)

	return true
}

RuleMachine := !(void^ itr,MiniMachineNode^ node) -> int
{
	iterU := itr->{Object^}
	iterNode := node
	ToRet := 0
	NowRet := 0

	while iterU != null
	{
		if iterU == null return ToRet
		iterVal := iterU.GetValue()
		if iterU.IsDataR iterVal = "~d"

		iterId := iterU.GetTokenId()
		if iterU.IsDataR iterId = 1
		//if iterId == 0 printf("wut %s\n",iterU.GetValue())

		inWNext := iterNode.WhatNext.TryFind(iterVal)
		if inWNext != null
		{
			iterNode = inWNext^
			NowRet += 1
			if iterNode.IsTerm 
			{
				ToRet = NowRet
			}
			if iterU.Line != iterU.Right?.Line and iterU.Right.GetValue() == "()"
			{
				return ToRet
			}
			iterU = iterU.Right
		}else
		{
			return ToRet
		}
	}
	return ToRet
}


RuleTypedefTemplate := !(void^ itr) -> int
{
	It := itr->{Object^}
	if It.Down == null return 0
	if It.Down.Right == null return 0
	if It.Down.Right.Right == null return 0
	if not ( It.Down is ObjIndent ) return 0
	if It.Down.Right.GetValue() != "." return 0
	if It.Down.Right.Right.GetValue() != "{}" return 0

	It = It.Right
	if It == null return 0
	if It.GetValue() != ":=" return 0

	It = It.Right
	if It == null return 0
	if It.GetValue() != "type" return 0

	It = It.Right
	if It == null return 0

	if InDataR(It) return 4
	return 0
}

RuleParam := !(void^ itr) -> int
{
	size := 0

	It := itr->{Object^}

	if It.GetValue() != "~ind" and It.GetValue() != "~str" return 0

	It = It.Right
	if It == null return 0
	size += 1

	while It.GetValue() == ","
	{
		It = It.Right
		if It == null return 0
		
		if It.GetValue() != "~ind" and It.GetValue() != "~str" return 0

		It = It.Right
		if It == null return 0

		size += 2
	}

	if It.GetValue() != ":=" return 0

	It = It.Right
	if It == null return 0

	if It.GetValue() == "default"
	{
		return size + 2
	}
	
	if It.GetValue() == "extern"
	{
		It = It.Right
		if It == null return 0
		size += 1
	}
	if It.GetValue() == "type"
	{
		It = It.Right
		if It == null return 0
		size += 1
	}

	if It.GetValue() == "ref"
	{
		It = It.Right
		if It == null return 0
		size += 1
	}
	if It.GetValue() == "virtual"
	{
		It = It.Right
		if It == null return 0
		size += 1
	}
	if It.GetValue() in !["thread_local","task_local"]
	{
		It = It.Right
		if It == null return 0
		size += 1
	}
	if It.GetValue() == "task_local"
	{
		It = It.Right
		if It == null return 0
		size += 1
	}

	if not InDataR(It) return 0

	It = It.Right
	if It == null return size + 2

	if It.GetValue() != "at" return size + 2

	It = It.Right
	if It == null return 0
	if not InDataR(It) return 0

	return size + 4
}
RuleSLambda := !(void^ itr) -> int
{
	It := itr->{Object^}

	siz := 3

	if not InDataR(It) return 0

	It = It.Right
	if It == null return 0

	if It.GetValue() != "=>" and It.GetValue() != "==>" return 0

	It = It.Right
	if It == null return 0

	if It.GetValue() == "[]"
	{
		It = It.Right
		if It == null return 0
		siz += 1 
	}

	if It.GetValue() == "{}" return siz
	if InDataR(It) return siz
	return 0
}
RuleCallFunction := !(void^ itr) -> int
{
	It := itr->{Object^}

	if It.Left != null
	{
		if It.Left.GetValue() == "." return 0
	}

	if not InDataR(It) return 0
	
	It = It.Right
	if It == null return 0

	if It.GetValue() != "()" return 0

	if It.Line != It.Left.Line return 0

	return 2
}
RuleMinus := !(void^ itr) -> int
{
	It := itr->{Object^}

	if It.Left != null
	{
		if It.Left.GetValue() == "[]" return 0
		if It.Left.GetValue() == "()" return 0
		if It.Left is ObjSuffix return 0 
		if It.Left.IsDataR return 0
	}
	if It.GetValue() != "-" return 0

	It = It.Right
	if It == null return 0

	if not It.IsDataR return 0
	
	It = It.Right
	if It == null return 2

	switch It.GetValue()
	{
		case "." return 0
		case "()" return 0
		case "[]" return 0
		case "^" return 0
		case "&" return 0
	}

	return 2
}
RuleDefer := !(void^ itr) -> int
{
	It := itr->{Object^}

	if It.GetValue() != "defer" and It.GetValue() != "on_exception" return 0

	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0
	return 2
}

RuleOneFunc := !(void^ itr)-> int
{
	It := itr->{Object^}

	if  It.GetValue() != "return" and It.GetValue() != "yield" return 0

	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0

	return 2
}

RuleIf := !(void^ itr)-> int
{
	It := itr->{Object^}

	if It.GetValue() != "if" return 0
	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0
	It = It.Right
	if It == null return 0

	if It.GetValue() != "{}" and (not InBlockData(It)) and (It.GetValue() != "i:=1") return 0
	//if It.GetValue() != "{}" and (not InBlockData(It)) return 0
	It = It.Right
	if It == null return 3

	if It.GetValue() != "else" return 3
	It = It.Right
	if It == null return 3

	if It.GetValue() == "{}" or InBlockData(It)  return 5

	return 3
}
RuleSwitch := !(void^ itr) -> int
{
	It := itr->{Object^}

	if It.GetValue() != "switch" return 0

	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0

	It = It.Right
	if It == null return 0

	if It.GetValue() != "{}" return 0

	return 3
}

RuleCase := !(void^ itr) -> int
{
	It := itr->{Object^}

	if It.GetValue() != "case" return 0

	It = It.Right
	if It == null return 0

	if InDataR(It) return 2
	return 0
}
RuleWhile := !(void^ itr)-> int
{
	It := itr->{Object^}

	if It.GetValue() != "while" return 0
	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0
	It = It.Right
	if It == null return 0

	if It.GetValue() != "{}" and not InBlockData(It) return 0
	return 3
}

RuleFor := !(void^ itr)-> int
{
	It := itr->{Object^}
	Size := 3

	if It.GetValue() != "for" return 0
	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0
	It = It.Right
	if It == null return 0

	if It.GetValue() == ","
	{
		It = It.Right

		if It == null return 0

		if not InDataR(It) return 0

		It = It.Right
		if It == null return 0
		Size += 2
	}

	if It.GetValue() == ":"
	{
		It = It.Right
		if It == null return 0

		if not InDataR(It) return 0
		It = It.Right
		if It == null return 0

		Size += 2
	}

	if It == null return 0


	while It.GetValue() == ","
	{
		It = It.Right
		if It == null return 0

		if It.GetValue() != "~ind" return 0

		It = It.Right
		if It == null return 0
		
		if It.GetValue() == ","
		{
			It = It.Right
			if It == null return 0

			if It.GetValue() != "~ind" return 0
			
			It = It.Right
			if It == null return 0

			Size += 2
		}
			
		if It.GetValue() != ":" return 0

		It = It.Right
		if not InDataR(It) return 0
		It = It.Right
		if It == null return 0

		Size += 4
	}

	if It.GetValue() != "{}" and not InBlockData(It) return 0
	return Size
}

RuleImport := !(void^ itr) -> int 
{
	It := itr->{Object^}

	if It.GetValue() != "~cmd" return 0

	asI := It->{ObjCmd^}

	if asI.MyStr != "#import" return 0

	It = It.Right

	if It == null return 0

	if It.GetValue() != "~str" return 0

	return 2
}

