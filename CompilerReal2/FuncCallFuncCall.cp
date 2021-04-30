FuncCallFuncObjectCall := !(Object^ iterPre, Object^ iter) -> Object^
{
    consts := null->{Object^}
    if iter.GetValue() == "()"
    {
        if iter.Left.GetValue() in !["(d)", "d()"]
        {

            if (iter.Left.IsRef() or (iter.Left is ParamFuncCall and iter.Left->{ParamFuncCall^}.MacroCreated)) and iter.Left.GetType() is TypePoint
            {
                if iter.Left.GetType().Base is TypeFunc
                {
                    iterL := iter.Left
                    iterLT := iterL.GetType()
                    TrimCommas(iter)
                    return new PointFuncCall((iterLT.Base)->{TypeFunc^},iter.Down,iterL)
                }
            }else{
                if iter.Left.GetValue() == "(d)"
                {
                    dynCast := (iter.Left)->{ParamCall^}
                    consts := null->{Object^}
                    if iter.Right?.GetValue() == "." and iter.Right.Right?.GetValue() == "{}"
                    {
                        consts = iter.Right.Right
                    }
                    preRet := OneCall(dynCast.BeforeName, iter,consts,true)
                    if preRet != null return preRet
                }
            }
        }
        if true
        {
            iterL := iterPre.Left

            box := new FuncInputBox()  ; $temp
            FillAttrs(box^,iter)

            box.itPars.Emplace(iterPre.Left.GetType(),iterPre.Left.IsRef())

            for iterD : iterPre.Down
            {
                if iterD.GetValue() != ","
                    box.itPars.Emplace(iterD.GetType(),iterD.IsRef()) 
            }

            //TODO: add consts
            
            plsF := FindFunc("()",iter,box^,false)

            if plsF != null
            {
                if iter.Down != null
                {
                    iterL.Right = iter.Down
                    iter.Down.Left = iterL
                    iter.SetUp(iterL.Up)
                    TrimCommas(iter)
                }else{
                    PopOutNode(iter)
                }
                return MakeSimpleCall(plsF,iterL)
            }
            return null
        }
    }
    return null
}