

InDataR := !(Object^ obj) -> bool
{
	Val := obj.GetValue()

	if Val == "~d" return true
	if Val == "~ind" return true
	if Val == "~double" return true
	if Val == "~int" return true
	if Val == "~str" return true
	if Val == "~cmd" return true
	return false
}

RuleType := !(void^ begin)^ -> int
RuleUse := !(Object^ Obj,char^ N,RuleType R) -> bool
{
	GotStuff := false
	iter := Obj.Down

	while iter != null
	{
		Try := R(iter)
		if Try > 0
		{
			iter = UNext(iter,new ObjObj(N),Try) 
			GotStuff = true
		}else{
			iter = iter.Right
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
			iter = UNext(iter,new ObjObj(N),Try) 
			GotStuff = true
		}else{
			iter = iter.Right
		}
	}
	return GotStuff
}


StupidWhile := !(Object^ begin,PriorityBag bag ) -> bool
{
	//iter := begin.Down
	GotStuff := false

	iterBag := bag.Lines.Start
	while iterBag != null
	{
		if RuleUseSome(begin,"~d",iterBag.Data) return true
		iterBag = iterBag.Next
	}


	if RuleUse(begin,"i:=0",RuleParam) return true // for func

	return false	
}


SyntaxCompress := !(Object^ begin, PriorityBag bag) -> bool
{
	if begin == null return true

	GotWork := true

	iter := begin.Down

	while iter != null
	{
		if iter.GetValue() == "{}" SyntaxCompress(iter, bag)
		if iter.GetValue() == "()" SyntaxCompress(iter, bag)
		if iter.GetValue() == "[]" SyntaxCompress(iter, bag)
		iter = iter.Right
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
	if not InDataR(It) return 0

	return size + 2
}

RuleOneFunc := !(void^ itr)-> int
{
	It := itr->{Object^}

	if It.GetValue() != "delete" and It.GetValue() != "new" and It.GetValue() != "return" and It.GetValue() != "not" return 0

	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0

	return 2
}

