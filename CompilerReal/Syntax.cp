

InDataR := !(Object^ obj) -> bool
{
	Val := obj.GetValue()

	if Val == "~dm" return true
	if Val == "~ind" return true
	if Val == "double" return true
	if Val == "~int" return true
	if Val == "~str" return true
	if Val == "!()" return true
	if Val == "~cmd" return true
	return false
}

RuleType := !(void^ begin)^ -> int
RuleUse := !(Object^ Obj,char^ N,RuleType R) -> bool
{
	GotStuff := false
	iter := Obj.Down
	Pass := R

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


StupidWhile := !(Object^ begin) -> bool
{
	iter := begin.Down
	GotStuff := false

	if RuleUse(begin,"~dm",RuleRight) return true

	if RuleUse(begin,"~dm",RuleTwoFuncMul) return true
	if RuleUse(begin,"~dm",RuleTwoFuncAdd) return true

	if RuleUse(begin,"~dm",RuleTwoFuncLog) return true
	if RuleUse(begin,"~dm",RuleTwoFuncShift) return true

	if RuleUse(begin,"!()",RuleFunc) return true
	if RuleUse(begin,"cmd()",RuleCmd) return true
	if RuleUse(begin,"i:=0",RuleParam) return true // for func
	if RuleUse(begin,"~func r",RuleOneFunc) return true

	return false	
}


SyntaxCompress := !(Object^ begin) -> bool
{
	if begin == null return true

	GotWork := true

	iter := begin.Down

	while iter != null
	{
		if iter.GetValue() == "{}" SyntaxCompress(iter)
		if iter.GetValue() == "()" SyntaxCompress(iter)
		if iter.GetValue() == "[]" SyntaxCompress(iter)
		iter = iter.Right
	}

	while GotWork GotWork = StupidWhile(begin)

	return true
}

RuleFunc := !(void^ itr) -> int
{
	It := itr->{Object^}

	if It == null return -1

	if It.GetValue() != "!" return 0

	It = It.Right
	if It == null return 0

	if It.GetValue() != "()" return 0

	It = It.Right
	if It == null return 0

	Size := 2

	if It.GetValue() == "->"{
		Size += 2
		It = It.Right
		if not InDataR(It) return 0
		It = It.Right
	}
	if It.GetValue() != "declare" and It.GetValue() != "{}" return 0
	return Size + 1
}
RuleCmd := !(void^ itr) -> int
{
	It := itr->{Object^}
	if It.GetValue() != "~cmd" return 0

	It = It.Right
	if It == null return 0
	if not InDataR(It) and It.GetValue() != "{}" return 0
	
	return 2
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

RuleRight := !(void^ itr) -> int
{
	It := itr->{Object^}

	if not InDataR(It) return 0

	It = It.Right
	if It == null return 0

	if It.GetValue() == "->"
	{
		It = It.Right
		if It == null return 0

		if It.GetValue() != "{}" return 0
	}else{
		if It.GetValue() != "^" and It.GetValue() != "&" and It.GetValue() != "[]" return 0

		return 2
	}
	return 0
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

RuleTwoFuncAdd := !(void^ itr)-> int
{
	It := itr->{Object^}

	if not InDataR(It) return 0

	It = It.Right
	if It == null return 0
	
	if It.GetValue() != "-" and It.GetValue() != "+" return 0

	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0

	return 3
}

RuleTwoFuncMul := !(void^ itr)-> int
{
	It := itr->{Object^}

	if not InDataR(It) return 0

	It = It.Right
	if It == null return 0
	
	if It.GetValue() != "*" and It.GetValue() != "/" and It.GetValue() != "%" return 0

	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0

	return 3
}
RuleTwoFuncLog := !(void^ itr)-> int
{
	It := itr->{Object^}

	if not InDataR(It) return 0

	It = It.Right
	if It == null return 0
	
	if It.GetValue() != "or" and It.GetValue() != "and" and It.GetValue() != "xor" return 0

	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0

	return 3
}
RuleTwoFuncShift := !(void^ itr)-> int
{
	It := itr->{Object^}

	if not InDataR(It) return 0

	It = It.Right
	if It == null return 0
	
	if It.GetValue() != "<<" and It.GetValue() != ">>" return 0

	It = It.Right
	if It == null return 0

	if not InDataR(It) return 0

	return 3
}
