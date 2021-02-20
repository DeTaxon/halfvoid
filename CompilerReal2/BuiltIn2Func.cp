InitBuiltIn2Funcs := !() -> void
{
    funcPars := Queue.{Type^}() ; $temp
    funcPars.Push(null)
    funcPars.Push(null)
    funcBools := bool[2]
    funcBools[0] = false
    funcBools[1] = false
    for i : 8
    {
        funcPars[0] = TypeTable[i]
        funcPars[1] = TypeTable[i]
        fType := GetFuncType(funcPars,funcBools,TypeTable[i],false,false)

        AddBuiltInFunc(new BuiltIn2IntMath("+",fType,"add"))
        AddBuiltInFunc(new BuiltIn2IntMath("-",fType,"sub"))
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
			case void 
				assert(false)
		}
	}
}