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
internalDeferExcpAdd := !(!(void^)^->void defFunc,void^ stackPoint) -> void
{
	defBlock := ref internalDeferBlock
	defBlock.funcsExe[defBlock.indPoint].0 = defFunc
	defBlock.funcsExe[defBlock.indPoint].1 = stackPoint
	defBlock.funcsIsExcp[defBlock.indPoint] = true
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
	this := !(char^ sMsg) -> void
	{
		itId = 0
		itMsg = sMsg
	}
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
	regsData := vec4f[16] // 16*4*4=256 data aligned to 16
	deferDepth := int
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
		printf("uncathed exception %s\n",inp->{IException^}.Msg())
		internalDebugTrap()
		return void
	}

	nowFrame := ref nowDat.frames[nowDat.depth - 1]
	internalDeferApplyExp(nowFrame.deferDepth)
	nowDat.expData = inp
	//builtin_longjump(nowFrame.regsData[0]&)
	longjmp(nowFrame.regsData[0]&,1)
}

