

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
}

