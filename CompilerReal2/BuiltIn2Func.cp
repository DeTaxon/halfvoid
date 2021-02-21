InitBuiltIn2Funcs := !() -> void
{
	funcPars1 := Queue.{Type^}() ; $temp
	funcPars1.Push(null)
	funcPars2 := Queue.{Type^}() ; $temp
	funcPars2.Push(null)
	funcPars2.Push(null)
	funcBools := bool[2]
	funcBools[0] = false
	funcBools[1] = false

	for i : 8
	{
		pars1 := ![TypeTable[i]]
		pars2 := ![TypeTable[i],TypeTable[i]]
		fType := GetFuncType(pars2,TypeTable[i])

		AddBuiltInFunc(new BuiltIn2IntMath("+",fType,"add"))
		AddBuiltInFunc(new BuiltIn2IntMath("-",fType,"sub"))
		AddBuiltInFunc(new BuiltIn2IntMath("*",fType,"mul"))
		//AddBuiltInFunc(new BuiltIn2IntMath("div",fType,i >= 4 ?: "sdiv" : "udiv"))
		//AddBuiltInFunc(new BuiltIn2IntMath("%",fType,i >= 4 ?:  "srem" : "urem"))
		AddBuiltInFunc(new BuiltIn2IntMath("and_b",fType,"and"))
		AddBuiltInFunc(new BuiltIn2IntMath("or_b",fType,"or"))
		AddBuiltInFunc(new BuiltIn2IntMath("xor_b",fType,"xor"))

		fTypeBoolRes := GetFuncType(pars2,GTypeBool)
		
		AddBuiltInFunc(new BuiltIn2IntMath("==",fTypeBoolRes,"icmp eq"))
		AddBuiltInFunc(new BuiltIn2IntMath("!=",fTypeBoolRes,"icmp ne"))

		fTypeNeg := GetFuncType(pars1,TypeTable[i])
		AddBuiltInFunc(new BuiltIn2Negative(". -",fTypeNeg,"sub"))


		if i < 4
		{
	   		//AddBuiltInFunc(new BuiltIn2IntMath("<=",fTypeBoolRes,"icmp ule"))
	   		//AddBuiltInFunc(new BuiltIn2IntMath("<",fTypeBoolRes,"icmp ult"))
	   		//AddBuiltInFunc(new BuiltIn2IntMath(">=",fTypeBoolRes,"icmp uge"))
	   		//AddBuiltInFunc(new BuiltIn2IntMath(">",fTypeBoolRes,"icmp ugt"))
		}else{
	   		//AddBuiltInFunc(new BuiltIn2IntMath("<=",fTypeBoolRes,"icmp sle"))
	   		//AddBuiltInFunc(new BuiltIn2IntMath("<",fTypeBoolRes,"icmp slt"))
	   		//AddBuiltInFunc(new BuiltIn2IntMath(">=",fTypeBoolRes,"icmp sge"))
	   		//AddBuiltInFunc(new BuiltIn2IntMath(">",fTypeBoolRes,"icmp sgt"))
		}
	}
	for it : ![GTypeFloat,GTypeDouble]
	{
		pars1 := ![it]
		pars2 := ![it,it]
		fType := GetFuncType(pars2,it)

		AddBuiltInFunc(new BuiltIn2IntMath("+",fType,"fadd"))
		AddBuiltInFunc(new BuiltIn2IntMath("-",fType,"fsub"))
		AddBuiltInFunc(new BuiltIn2IntMath("*",fType,"fmul"))
		AddBuiltInFunc(new BuiltIn2IntMath("/",fType,"fdiv"))

		fTypeBoolRes := GetFuncType(pars2,GTypeBool)

	   	AddBuiltInFunc(new BuiltIn2IntMath("<=",fTypeBoolRes,"fcmp ule"))
	   	AddBuiltInFunc(new BuiltIn2IntMath("<",fTypeBoolRes,"fcmp ult"))
	   	AddBuiltInFunc(new BuiltIn2IntMath(">=",fTypeBoolRes,"fcmp uge"))
	   	AddBuiltInFunc(new BuiltIn2IntMath(">",fTypeBoolRes,"fcmp ugt"))
	}
}

BuiltIn2Func := class extend BoxFunc
{
	GenerateCall := virtual !(Object^ pars) -> BaseFuncCall^
	{
		return new BuiltIn2Call(this&,pars)
	}
	PrintFunc := virtual !(BuiltIn2Call^ trg,sfile f) -> void {}
	DoJIT := virtual !(BuiltIn2Call^ trg) -> void^ {}
}

BuiltIn2IntMath := class extend BuiltIn2Func
{
	llvmOper := char^
	this := !(char^ fName, TypeFunc^ fType, char^ lOp) -> void
	{
		FuncName = fName
		MyFuncType = fType
		llvmOper = lOp
	}
	PrintFunc := virtual !(BuiltIn2Call^ trg,sfile f) -> void {
		trg.GenId()
		trg.Down.PrintPre(f)
		trg.Down.Right.PrintPre(f)
		trg.PrintRes(f)
		f << " = " << llvmOper << " "
		trg.Down.PrintUse(f)
		f << " , "
		f << trg.Down.Right.GetName()
		f << "\n"
	}
	DoJIT := virtual !(BuiltIn2Call^ trg) -> void^ {
		par1 := trg.Down.DoJIT()
		par2 := trg.Down.Right.DoJIT()
		switch FuncName
		{
			case "+" return jit_insn_add(JITCFunc,par1,par2)
			case "-" return jit_insn_sub(JITCFunc,par1,par2)
			case "*" return jit_insn_mul(JITCFunc,par1,par2)
			case "div" return jit_insn_div(JITCFunc,par1,par2)
			case "/" return jit_insn_div(JITCFunc,par1,par2)
			case "%" return jit_insn_rem(JITCFunc,par1,par2)
			case "and_b" return jit_insn_and(JITCFunc,par1,par2)
			case "or_b" return jit_insn_or(JITCFunc,par1,par2)
			case "xor_b" return jit_insn_xor(JITCFunc,par1,par2)
			case "==" return jit_insn_eq(JITCFunc,par1,par2)
			case "!=" return jit_insn_ne(JITCFunc,par1,par2)
			case void 
				assert(false)
		}
	}
}
BuiltIn2Negative := class extend BuiltIn2Func
{
	llvmOper := char^
	this := !(char^ fName, TypeFunc^ fType,char^ lOp) -> void
	{
		FuncName = fName
		MyFuncType = fType
		llvmOper = lOp
	}
	PrintFunc := virtual !(BuiltIn2Call^ trg,sfile f) -> void {
		trg.GenId()
		trg.Down.PrintPre(f)
		trg.PrintRes(f)
		f << " = "<< llvmOper <<" "
		trg.Down.GetType().PrintType(f)
		f << " 0, "
		f << trg.Down.GetName()
		f << "\n"
	}
	DoJIT := virtual !(BuiltIn2Call^ trg) -> void^ {
		par1 := trg.Down.DoJIT()
		return jit_insn_neg(JITCFunc,par1)
	}
}
