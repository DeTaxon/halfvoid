BuiltIn2Func := class extend BoxFunc
{
    PrintFunc := virtual !(BuiltIn2Call^ trg,sfile f) -> void {}
    DoJIT := virtual !(BuiltIn2Call^ trg) -> void^ {}
}

BuiltIn2IntMath := class extend BuiltIn2Func
{
    llvmOper := char^
    this := !(char^ fName, Type^ fType, char^ lOp) -> void
    {
        FuncName = fName
        MyFuncType = fType
        llvmOper = lOp
    }
    PrintFunc := virtual !(BuiltIn2Call^ trg,sfile f) -> void {
        trg.Down.PrintPre(f)
        trg.Down.Right.PrintPre(f)
        trg.PrintRes(f)
        f << " = " << llvmOper << " "
        trg.Down.PrintUse(f)
        f << " , "
        f << trg.Down.Right.GetName()
        f << "\n"
    }
}