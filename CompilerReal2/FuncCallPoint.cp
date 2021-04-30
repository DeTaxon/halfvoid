FuncCallGetFieldTypes := !(Object^ iterPre, Object^ iter) -> Object^
{
    if not (iter.GetValue() in ![".","->"])
        return null

    return FuncCallGetFieldExchangeType(iterPre,iter)? // x->{newType}
    return FuncCallGetFieldClassElement(iterPre,iter)? // x.0 x.1 

    if iter.Right is ObjIndent or iter.Right is ObjStr
    {
        asName := ""
        if iter.Right is ObjIndent
        {
            asIndent := (iter.Right)->{ObjIndent^} 
            asName = asIndent.MyStr
        }else{
            asStr := (iter.Right)->{ObjStr^}
            asName = asStr.GetString()
        }

        if iter.GetValue() == "->"
        {
            box := new FuncInputBox() ; $temp 
            box.itPars.Emplace(iter.Left.GetType(),iter.Left.IsRef())
            box.itConsts.Push(new ObjStr(asName))

            fun := FindFunc("->",iter,box^,false)
            if fun != null
            {
                iter = iter.Left
                iter.Right.Left = null
                iter.Right = null
                return MakeSimpleCall(fun,iter)
            }
        }

        if iter.Right.Right != null {
            return FuncCallGetFieldClassFieldCall(asName,iterPre,iter)? // x.y()
        }else{
            return FuncCallGetFieldClassGetFieldByName(asName,iterPre,iter)? // x.y()                
        }
    }
    
    return null
}
FuncCallGetFieldExchangeType := !(Object^ iterPre, Object^ iter) -> Object^
{
    if iter.Right.GetValue() != "{}"
        return null
    
    SyntaxCompress(iter.Right,PriorityData)
    typ := ParseType(iter.Right.Down)
    if typ == null return null

    lt := iter.Left.GetType()

    if (lt is TypePoint or lt is TypeFatArr or lt is TypeArr) and (typ is TypePoint or typ is TypePoint)
    {
        BoxExc(iter.Left,typ,false)
        lObj := iter.Left
        itUp := iter.Up
        PopOutNode(lObj)
        //ReplaceNode(itUp,lObj)
        return lObj
    }

    func := GetExchange(iter.Left,iter.Left,typ,iter.Left.IsRef())
    if func == null return null
    iter = iter.Left
    iter.Right.Left = null
    iter.Right = null
    fCall := MakeSimpleCall(func,iter)
    fCall.Line = iter.Line
    return fCall
}
FuncCallGetFieldClassElement := !(Object^ iterPre, Object^ iter) -> Object^
{
    if iter.Right is ObjInt and iter.Right.Right == null
    {
        box := new FuncInputBox() ; $temp
        box.itConsts.Push(iter.Right)

        box.itPars.Emplace(iter.Left.GetType(),iter.Left.IsRef())

        fnc := FindFunc(".",iter,box^,false)

        if fnc != null
        {
            iter = iter.Left
            iter.Right = null
            return MakeSimpleCall(fnc,iter)
        }
    }
    return null
}
FuncCallGetFieldClassFieldCall := !(char^ asName,Object^ iterPre, Object^ iter) -> Object^
{ 
    LL := iter.Left

    //if iter.Left.GetType().Ge == "standart"
    //{
    //	return null
    //}

    GotClass := true
    if  not LL.GetType() is TypeClass
    {
        if LL.GetType() is TypePoint	
        {
            if not LL.GetType().Base is TypeClass
                GotClass = false
        }else	GotClass = false
    }
    asClass := BoxClass^
    if GotClass
    {
        if LL.GetType() is TypePoint
        {
            asClass = ((iter.Left.GetType().Base)->{TypeClass^}).ToClass
        }else
        {
            asClass = ((iter.Left.GetType())->{TypeClass^}).ToClass
        }
    }else{
        asClass = null
    }


    box := new FuncInputBox()  ; $temp
    FillAttrs(box^,iter)

    if LL.inhAttrs != null
    {
        for v,k : LL.inhAttrs^
        {
            if box.itAttrs.TryFind(k) == null
                box.itAttrs[k] = v ; $temp
        }
    }
    
    box.itPars.Emplace(iter.Left.GetType(),iter.Left.IsRef()) 
    for iterK : iter.Right.Right.Down
    {
        if iterK.GetValue() != ","
            box.itPars.Emplace(iterK.GetType(),iterK.IsRef())
    }
    if iter.Left is MetaItemWrapper
    {
        box.itMetaPtr = iter.Left->{MetaItemWrapper^}.ptrToBlock
    }

    func := FindFunc(asName,iter,box^,true)

    if func == null and GotClass and asClass != null
    {
        box.itPars[0].first = asClass.ClassType
        func = asClass.GetFunc(asName,box^,false)
    }

    if func != null
    {
        TrimCommas(iter.Right.Right)
        iter = iter.Left
        iter.Right = iter.Right.Right.Right.Down
        if iter.Right != null iter.Right.Left = iter
        iter.SetUp(iter.Up)

        if iter.GetType() is TypePoint
        {
            iter = new PtrToRef(iter)
        }
        return  MakeSimpleCall(func,iter)
    }


    iter = UNext(iter.Left,new ObjData(),3)
    oldIter := iter
    iter = GetFuncCall(iter)
    if iter == null 
        return null
    ReplaceNode(oldIter,iter)

    return GetFuncCall(iter.Up)
}
FuncCallGetFieldClassGetFieldByName := !(char^ asName,Object^ iterPre, Object^ iter) -> Object^
{
    asClass := BoxClass^
    asClass = null
    LT := iter.Left.GetType()

    if LT is TypeStandart
    {
        box1 := new FuncInputBox()  ; $temp
        box1.itPars.Emplace(LT,iter.Left.IsRef())
        box1.itConsts.Push(new ObjStr(asName)) 
        func := FindFunc(".",iter,box1^,false)
        if func == null return null
        iter = iter.Left
        PopOutNode(iter.Right)
        PopOutNode(iter.Right)
        return MakeSimpleCall(func,iter)
    }

    if LT is TypeClass
    {
        asClass = ((LT->{TypeClass^}).ToClass)
    }else{
        if LT is TypePoint  and LT.Base is TypeClass
        {
            asClass = (((LT.Base)->{TypeClass^}).ToClass)
        }else{
            return null
        }
    }

    asClass.PreAskField(asName)

    box2 := new FuncInputBox()  ; $temp

    box2.itPars.Emplace(asClass.ClassType,true)
    box2.itConsts.Push(new ObjStr(asName))
    if asClass.UnrollTemplate ==null return null
    pru := (asClass.UnrollTemplate^.GetPriority(box2^))
    roll := null->{BoxFunc^}
    if pru == 255 
    {
        roll = asClass.GetVirtualParamFunc(asName)
        if roll == null 
        {
            if asClass.metaFields.Contain(asName)
            {
                iter = iter.Left
                if iter.GetType() is TypePoint
                {
                    iter = new PtrToRef(iter)
                }
                PopOutNode(iter.Right)
                PopOutNode(iter.Right)
                
                metWrapper := new MetaItemWrapper
                metWrapper.ptrToBlock = asClass.metaFields[asName].Down
                iter = UNext(iter,metWrapper,1)
                return iter
            }
        }
        //return null //TODO: check for user functions
    }else{
        roll =  (asClass.UnrollTemplate^.GetFunc(box2^))
    }
    if roll == null return null

    iter = iter.Left
    if iter.GetType() is TypePoint
    {
        iter = new PtrToRef(iter)
    }
    PopOutNode(iter.Right)
    PopOutNode(iter.Right)
    preRes :=  MakeSimpleCall(roll,iter)
    //if preRes != null {
    //	preRes.inhAttrs = iter.inhAttrs
    //}
    return preRes
}