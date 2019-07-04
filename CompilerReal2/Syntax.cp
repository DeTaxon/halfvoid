#import "Tree.cp"
#import "ObjObj.cp"
#import "SLambda.cp"
#import "ObjData.cp"

InDataR := !(Object^ obj) -> bool
{
	Val := obj.GetValue()

	if Val == "~null" return true
	if Val == "~d" return true
	if Val == "~ind" return true
	if Val == "~double" return true
	if Val == "~bool" return true
	if Val == "~int" return true
	if Val == "~str" return true
	if Val == "~cmd" return true
	if Val == "~{}type" return true
	if Val == "~type" return true
	//if Val == "()" return true
	return false
}

InBlockData := !(Object^ obj) -> bool
{
	if InDataR(obj) return true
	if obj.GetValue() == "return()" return true
	if obj.GetValue() == "if()" return true
	if obj.GetValue() == "while()" return true
	if obj.GetValue() == "for()" return true
	if obj.GetValue() == "switch()" return true
	if obj.GetValue() == "~continue" return true
	if obj.GetValue() == "~break" return true

	return false
}

//RuleType := type !(void^ begin)^ -> int
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
	
	if RuleUse(begin,"imprt",RuleImport) return true
	if RuleUse(begin,"~d",RuleMinus) return true

	GotStuff := false

	iterBag := bag.Lines.Start
	while iterBag != null
	{
		if RuleUseSome(begin,"~d",iterBag.Data) return true
		iterBag = iterBag.Next
	}


	if RuleUse(begin,"~extra1",RuleSLambda) return true
	if RuleUse(begin,"Defer()",RuleDefer) return true
	if RuleUse(begin,"return()",RuleOneFunc) return true
	if RuleUse(begin,"i:=0",RuleParam) return true // for func 
	if RuleUseReverse(begin,"if()",RuleIf) return true
	if RuleUse(begin,"while()",RuleWhile) return true
	if RuleUse(begin,"for()",RuleFor) return true
	if RuleUse(begin,"switch()",RuleSwitch) return true
	if RuleUse(begin,"case()",RuleCase) return true

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
		if InDataR(iterU) iterVal = "~d"
		if iterNode.WhatNext.Exist(iterVal)
		{
			iterNode = iterNode.WhatNext[iterVal]
			NowRet += 1
			iterU = iterU.Right
			if iterNode.IsTerm 
			{
				ToRet = NowRet
			}
		}else
		{
			return ToRet
		}
	}
	return ToRet
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
	if It.GetValue() == "thread_local"
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

	if not InDataR(It) return 0

	It = It.Right
	if It == null return 0

	if It.GetValue() != "=>" return 0

	It = It.Right
	if It == null return 0

	if It.GetValue() == "{}" return 3
	if InDataR(It) return 3
	return 0
}
RuleMinus := !(void^ itr) -> int
{
	It := itr->{Object^}

	if It.Left != null
	{
		if It.Left.GetValue() == "[]" return 0
		if It.Left.GetValue() == "()" return 0
		if InDataR(It.Left)	return 0
	}
	if It.GetValue() != "-" return 0

	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0
	
	It = It.Right
	if It == null return 2

	if It.GetValue() == "." return 0
	if It.GetValue() == "()" return 0
	if It.GetValue() == "[]" return 0
	if It.GetValue() == "^" return 0
	if It.GetValue() == "&" return 0

	return 2
}
RuleDefer := !(void^ itr) -> int
{
	It := itr->{Object^}

	if It.GetValue() != "defer" return 0

	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0
	return 2
}

RuleOneFunc := !(void^ itr)-> int
{
	It := itr->{Object^}

	if  It.GetValue() != "return" return 0

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

