

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
		ClassType = new TypeClass(this&->{BoxClass^})
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
			WorkBag.Push(this&,State_GetUse)
			return FncSetVal
		}
		if name == "." return GetNmFunc
		//if name == "!{}" return 
		return null->{BoxFunc^}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_GetUse
		{	
			cc := Queue.{Object^}()
			for i : Params.Size()
			{	
				itPar := Params[i]
				Items := Queue.{Type^}()
				Items.Push(itPar.ResultType)
				Items.Push(itPar.ResultType)

				fnc := FindFunc("=",Up,Items,cc,false)
				if fnc != null
				{
					mm := Queue.{Type^}()
					mm.Push(ClassType->{Type^})
					cc2 := Queue.{Object^}()
					cc2.Push(new ObjInt(i))
					
					frmPre := GetNmFunc->{BoxTemplate^}
					frm := frmPre.GetFunc(mm,cc2)
					if frm != null
					{
						partA := MakeSimpleCall(frm,new FuncParam("ToSet",ClassType->{Type^},true))
						partB := MakeSimpleCall(frm,new FuncParam("FromIt",ClassType->{Type^},true))
						partA.Right = partB
						partB.Left = partA
						FncSets.Push(MakeSimpleCall(frm,partA))
					}else{
						EmitError("compiler error 436234")
					}
				}else{
					EmitError("can not create tuple")//TODO: fund it calls
				}
			}
		}
	}
}

CTT := CreateTupleTemplate^


CreateTupleTemplate := class extend BoxTemplate
{
	this := !() -> void
	{
	}
	GetPriority := virtual !(Queue.{Type^} pars, Queue.{Object^} consts) -> int
	{
		return 0//?
	}
	CreateFuncPointer := virtual !(Queue.{Type^} pars, Queue.{Object^} consts) -> TypeFunc^
	{
		itTpl := GetTuple(pars)
		ct := itTpl.ClassType
		cc := ct->{Type^}
		return GetFuncType(pars,null->{bool^},cc,false,false)
	}
	GetNewFunc := virtual !(Queue.{Type^} pars, Queue.{Object^} consts, TypeFunc^ funct) -> BoxFunc^
	{
		itTpl := GetTuple(pars)

		ftB := funct->{Type^}
		ctBPre := itTpl.ClassType
		ctB := ctBPre->{Type^}

		toEx := "call " + ftB.GetName() + "@TupleCreate" + itTpl.ClassId + 
			"(" 
		toEx = toEx + ctB.GetName() + "* #0"

		for i : pars.Size()
		{
			if pars[i].GetType() == "class" or pars[i].GetType() == "fixed"
			{
				toEx = toEx + ", " + pars[i].GetName() + "* #" + (i + 1)
			}else{
				toEx = toEx + ", " + pars[i].GetName() + "* #" + (i + 1)
			}
		}
		toEx = toEx + ")"

		return new BuiltInFuncMega("",funct,toEx)
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

