BuiltInFunc := class extend BoxFunc
{
	ToExe := string
	IsSelfPre := bool

	IsAssembler := virtual !() -> bool
	{
		return true
	}
	CheckIsSelf := !() -> void
	{	
		AsmLine := ToExe

		i := 0
		while AsmLine[i]
		{
			if AsmLine[i] == '#'
			{
				i += 1
				if AsmLine[i] in 'A'..'Z' IsSelfPre = true
			}else i += 1
		}

	}
}

BuiltInFuncUno := class extend BuiltInFunc
{
	this := !(string Name, Type^ l, bool lRef,Type^ retV, string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = code

		PP := Queue.{Type^}()
		PP.Push(l)

		IsRefs := bool[1]
		IsRefs[0] = lRef
		MyFuncType = GetFuncType(PP,IsRefs,retV,false,false)

		CheckIsSelf()
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
		CheckIsSelf()
	}
}

CreateBuiltIns := !() -> void
{
	BoolT := GetType("bool")

	for ![8,16,32,64]
	{
		for IsS : !["s","u"]
		{
			PType := GetType(IsS + it) // u8 s32 ...
			BuiltInFuncs.Push(new BuiltInFuncBinar("=",PType,true,PType,false,PType,"store i" + it + " #2, i" + it + "* #1\n"
												+"#0 = add i" + it + " #2,0\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("+",PType,false,PType,false,PType,"#0 = add i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("-",PType,false,PType,false,PType,"#0 = sub i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("*",PType,false,PType,false,PType,"#0 = imul i" + it + " #1,#2\n"))

			BuiltInFuncs.Push(new BuiltInFuncUno("->{}",PType,false,BoolT,"#0 = icmp eq i" + it + "#1 , 0.0")) 

			BuiltInFuncs.Push(new BuiltInFuncBinar("==",PType,false,PType,false,BoolT,"#0 = icmp eq i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("!=",PType,false,PType,false,BoolT,"#0 = icmp ne i" + it + " #1,#2\n"))

			BuiltInFuncs.Push(new BuiltInFuncBinar(">=",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"ge i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("<=",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"le i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar(">",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"gt i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("<",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"lt i" + it + " #1,#2\n"))

			BuiltInFuncs.Push(new BuiltInFuncUno("->{}",PType,false,BoolT,"#0 = icmp ne i"+it+" #1,0\n"))
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

	BuiltInFuncs.Push(new BuiltInFuncBinar("=",BoolT,true,BoolT,false,BoolT,"store i1 #2, i1* #1\n"
										+"#0 = add i1 #2,0\n"))
	
	BuiltInFuncs.Push(new BuiltInFuncUno("->{}",GetType("double"),false,GetType("float"),"#0 = fptrunc double #1 to float\n"))
	BuiltInFuncs.Push(new BuiltInFuncUno("->{}",GetType("float"),false,GetType("double"),"#0 = fpext float #1 to double\n"))


	BuiltInFuncs.Push(new BuiltInFuncBinar("or",BoolT,false,BoolT,false,BoolT,	"#A" +
											"br i1 #1, label %End##, label %Next##\n" +
											"Next##:\n" +
											"#B" +
											"br label %End##\n" +
											"End##:\n" +
											"#0 = select i1 #1, i1 true, i1 #2\n"))
	BuiltInFuncs.Push(new BuiltInFuncBinar("and",BoolT,false,BoolT,false,BoolT,	"#A" +
											"br i1 #1, label %Next##, label %End##\n" +
											"Next##:\n" +
											"#B" +
											"br label %End##\n" +
											"End##:\n" +
											"#0 = select i1 #1, i1 #2, i1 false\n"))

}
