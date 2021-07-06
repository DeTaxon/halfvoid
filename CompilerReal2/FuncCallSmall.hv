FuncCallSuffix := !(Object^ iter) -> Object^
{
    if iter is ObjSuffix //iter.GetValue() == "~suffix"
    {
        if iter.Left.GetType() == null return null
        sBox := new FuncInputBox()  ; $temp

        AsSuf := iter->{ObjSuffix^}
        sBox.itPars.Emplace(iter.Left.GetType(),false)

        Func := FindSuffix(AsSuf.MyStr,iter,sBox^)
        if Func == null return null
        
        iter = iter.Left
        PopOutNode(iter.Right)
        return MakeSimpleCall(Func,iter) 
    }
    return null
}