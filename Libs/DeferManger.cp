

internalDeferBlock := thread_local DeferBlock.{512}

DeferBlock := class .{@DeferStackSize} {
	funcsExe := !()&->void[DeferStackSize]
	funcsIsExcp := bool[DeferStackSize]
	indPoint := int
}

internalDeferAdd := !(!()&->void defFunc, bool isExcp) -> void
{
	defBlock := ref internalDeferBlock
	defBlock.funcsExe[defBlock.indPoint] = defFunc
	defBlock.funcsIsExcp[defBlock.indPoint] = isExcp
	defBlock.indPoint++
}

internalDeferGetDepth := !() -> int
{
	return internalDeferBlock.indPoint
}
internalDeferApply := !(int depth, bool isExcp) -> void
{
	defBlock := ref internalDeferBlock
	i := defBlock.indPoint
	while i != depth
	{
		i--
		if isExcp or not defBlock.funcsIsExcp[i]
		{
			itFunc := defBlock.funcsExe[i]
			itFunc()
		}
	}
}

