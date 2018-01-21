BuiltInFunc := class extend BoxFunc
{
	ToExe := string
	IsAssembler := virtual !() -> bool
	{
		return true
	}
}

BuiltInFuncBinar := class extend BuiltInFunc
{
	this := !(string Name, Type^ l, bool lRef, Type^ r, bool rRef,Type^ retV, string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = code

		PP := Queue.{Type^}()
		PP.Push(l)
		PP.Push(r)

		IsRefs := bool[2]
		IsRefs[0] = lRef
		IsRefs[1] = rRef
		MyFuncType = GetFuncType(PP,IsRefs,retV,false,false)
	}
}

CreateBuiltIns := !() -> void
{
	for ![8,16,32,64]
	{
		for IsS : !["s","u"]
		{
			PType := GetType(IsS + it) // u8 s32 ...
			BuiltInFuncs.Push(new BuiltInFuncBinar("=",PType,true,PType,false,PType,"store i" + it + " #2, i" + it + "* #1\n"
												+"#0 = add i" + it + " #2,0\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("+",PType,false,PType,false,PType,"#0 = add i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("*",PType,false,PType,false,PType,"#0 = imul i" + it + " #1,#2\n"))
		}
	}
	for !["float","double"] // half?
	{
		PType := GetType(it)
		BuiltInFuncs.Push(new BuiltInFuncBinar("=",PType,true,PType,false,PType,"store " + it + " #2, " + it + "* #1\n"
											+"#0 = fadd " + it + " #2,0.0\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("+",PType,false,PType,false,PType,"#0 = fadd " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("-",PType,false,PType,false,PType,"#0 = fsub " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("*",PType,false,PType,false,PType,"#0 = fmul " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("/",PType,false,PType,false,PType,"#0 = fdiv " + it + " #1,#2\n"))
	}

}
