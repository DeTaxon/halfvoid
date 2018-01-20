BuiltInFunc := class extend BoxFunc
{
		
}

BuiltInFuncBinar := class extend BuiltInFunc
{
	ToEx := string
	this := !(string Name, Type^ l, bool lRef, Type^ r, bool rRef,Type^ retV, string code) -> void
	{
		ToEx = code
	}
}

CreateBuiltIns := !() -> void
{
	for !["int"]
	{
		PType := GetType(it)
		BuiltInFuncs.Push(new BuiltInFuncBinar("=",PType,true,PType,false,PType,""))
	}

}
