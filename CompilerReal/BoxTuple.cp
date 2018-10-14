

Tupls := Queue.{TupleClass^}

GetTuple := !(Queue.{Type^} typs) -> TupleClass^
{
	for i : Tupls.Size()
	{
		itT := Tupls[i]
		if typs.Size() == itT.Params.Size()
		{
			isGood := true
			for j : typs.Size()
			{
				if typs[j] != itT.Params[j].ResultType
					isGood = false
			}
			if isGood return itT
		}
	}
	
	newCls := new TupleClass(typs)
	Tupls.Push(newCls)
	return newCls
}

TupleClass := class extend BoxClass
{
	EnableSet := bool
	GetNmFunc := TupleFuncGetItem^

	FncSetVal := BuiltInFuncBinar^
	FncSets := Queue.{Object^}

	this := !(Queue.{Type^} typs) -> void
	{
		for i : typs.Size()
		{
			Params.Push(new FieldParam("",typs[i],this&->{BoxClass^}))
		}
		ClassId = GetNewId()
		GetNmFunc = new TupleFuncGetItem(this&)
		cnPre := ClassType->{Type^}
		cn := cnPre.GetName()
		FncSetVal = new BuiltInFuncBinar("=",cnPre,true,cnPre,true,GetType("void"),false,
			"call void(" + cn + "*," + cn + "*)@TupleSet" + ClassId + "(" + cn + "* #1," + cn + "* #2)\n")
	}
	GetFunc := virtual !(string name,Queue.{Type^} pars, Queue.{Object^} consts, bool isV) -> BoxFunc^
	{
		if name == "=" {
			WorkBag.Push(this&,State_GetUse	
			return FncSetVal
		}
		if name == "." return GetNmFunc
		return null->{BoxFunc^}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}


TupleFuncGetItem := class extend BoxTemplate
{
	frmIn := TupleClass^
	this := !(TupleClass^ frmOn) -> void
	{
		frmIn = frmOn
	}
	GetPriority := virtual !(Queue.{Type^} pars,Queue.{Object^} consts) -> int
	{
		if pars.Size() != 1 return 255
		if consts.Size() != 1 return 255
		if consts[0].GetValue() != "~int"  return 255
		asN := consts[0]->{ObjInt^}
		val := asN.MyInt
		if val < 0 return 255
		if val >= frmIn.Params.Size() return 255
		return 0
	}
	GetNewFunc := virtual !(Queue.{Type^} pars, Queue.{Object^} consts, TypeFunc^ funcs) -> BoxFunc^
	{
		asN := consts[0]->{ObjInt^}
		ind := asN.MyInt

		itParam := frmIn.Params[ind]
		itType := itParam.ResultType
		return new BuiltInFuncUno(".",pars[0],true,itType,true,"#0 = getelemntptr " + pars[0].GetName() + " , " + pars[0].GetName() + "* #1, i32 0, i32 " + ind + "\n")
	}
}

