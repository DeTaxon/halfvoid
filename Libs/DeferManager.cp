internalDeferBlock := thread_local DeferBlock.{512}

DeferBlock := class .{@DeferStackSize} {
	funcsExe := Tuple.{!(void^)^->void,void^}[DeferStackSize]
	funcsIsExcp := bool[DeferStackSize]
	indPoint := int
}

internalDeferAdd := !(!(void^)^->void defFunc,void^ stackPoint) -> void
{
	defBlock := ref internalDeferBlock
	defBlock.funcsExe[defBlock.indPoint].0 = defFunc
	defBlock.funcsExe[defBlock.indPoint].1 = stackPoint
	defBlock.funcsIsExcp[defBlock.indPoint] = false
	defBlock.indPoint++
}

internalDeferGetDepth := !() -> int
{
	return internalDeferBlock.indPoint
}
internalDeferApply := !(int depth) -> void
{
	defBlock := ref internalDeferBlock
	i := defBlock.indPoint
	while i != depth
	{
		i--
		if not defBlock.funcsIsExcp[i]
		{
			itFunc := defBlock.funcsExe[i].0
			itFunc(defBlock.funcsExe[i].1)
		}
	}
	defBlock.indPoint = depth
}
internalDeferApplyExp := !(int depth) -> void
{
	defBlock := ref internalDeferBlock
	i := defBlock.indPoint
	while i != depth
	{
		i--
		itFunc := defBlock.funcsExe[i].0
		itFunc(defBlock.funcsExe[i].1)
	}
	defBlock.indPoint = depth
}

//internalSetJmp
//internalLongJmp
//internalDebugTrap

//setjmp := !(void^ jmpEnv) -> int declare
//longjmp := !(void^ jmpEnv, int val) -> void declare

IException := class
{
	GetId := virtual !() -> int
	{
		return 0
	}
	Msg := virtual !() -> char^
	{
		return ""
	}
}
Exception := class extend IException
{
	itId := int
	itMsg := char^
	this := !(int sId, char^ sMsg) -> void
	{
		itId = sId
		itMsg = sMsg
	}
	GetId := virtual !() -> int
	{
		return itId
	}
	Msg := virtual !() -> char^
	{
		return itMsg
	}
}

intExcepFrame := class
{
	deferDepth := int
	regsData := char[200]
}

intExcepData := class
{
	depth := int
	expData := void^
	frames := intExcepFrame[64]
}
intExc := thread_local intExcepData

internalEnterTry := !() -> void^
{
	nowDat := ref intExc
	nowFrame := ref nowDat.frames[nowDat.depth]
	nowFrame.deferDepth = internalDeferGetDepth()
	nowDat.depth++
	return nowFrame.regsData[0]&
}
internalLeaveTry := !() -> void
{
	intExc.depth--
}
internalGetExceptPoint := !() -> void^
{
	return intExc.expData
}

". throw" := !(void^ inp) -> void
{
	nowDat := ref intExc
	if nowDat.depth == 0
	{
		internalDebugTrap()
		return void
	}

	nowFrame := ref nowDat.frames[nowDat.depth - 1]
	internalDeferApplyExp(nowFrame.deferDepth)
	nowDat.expData = inp
	longjmp(nowFrame.regsData[0]&,1)
}

