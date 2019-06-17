#import "BoxClass.cp"

Tupls := Queue.{TupleClass^}

PrintTuples := !(sfile f) -> void
{
	Tupls[^].PrintGlobal(f)
}
PrintTuplesFuncs := !(sfile f) -> void
{
	Tupls[^].PrintGlobalExtra(f)
}

GetTuple := !(FuncInputBox^ typs) -> TupleClass^
{
	for itT,i : Tupls
	{
		if typs.itPars.Size() == itT.Params.Size()
		{
			isGood := true
			for ttps,j : typs.itPars
			{
				if ttps.first != itT.Params[j].ResultType
				{
					isGood = false
					break
				}
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

	askedCreate := char
	createFunc := TypeFunc^

	askedSet := char


	this := !(FuncInputBox^ typs) -> void
	{
		ClassId = GetNewId()
		ClassType = new TypeClass(this&->{BoxClass^})
		for tt,i : typs.itPars
		{
			new FieldParam("",tt.first,this&->{BoxClass^})
		}
		GetNmFunc = new TupleFuncGetItem(this&)
		cnPre := ClassType->{Type^}
		cn := cnPre.GetName()
		FncSetVal = new BuiltInFuncBinar("=",cnPre,true,cnPre,true,GetType("void"),false,
			"call void(" + cn + "*," + cn + "*)@TupleSet" + ClassId + "(" + cn + "* #1," + cn + "* #2)\n")

	}
	GetFunc := virtual !(string name,FuncInputBox itBox, bool isV) -> BoxFunc^
	{
		//if name == "=" {
		//	WorkBag.Push(this&,State_GetUse)
		//	return FncSetVal
		//}
		//if name == "." return GetNmFunc
		//if name == "!{}" return 
		if name == ".x"
		{
			return GetNmFunc.GetFunc(itBox)
		}
		return null->{BoxFunc^}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		f << "%Class" << ClassId << " = type {"
		for i : Params.Size()
		{
			if i > 0 f << ","
			f <<  Params[i].ResultType.GetName()
		}
		f << "}\n"
	}
	PrintGlobalExtra := !(sfile f) -> void
	{
		if askedCreate != 0
		{
			//toEx := "call " + ftB.GetName() + "@TupleCreate" + itTpl.ClassId + 
			//	"(" 
			//toEx = toEx + ctB.GetName() + "* #0"i
			f << "define void @TupleCreate" << ClassId << "(" << ClassType.GetName() << "* %ToSet" 
			for par,i : Params
			{
				itTyp := par.ResultType
				isRef := false
				if itTyp is TypeClass isRef = true
				if itTyp is TypeArr isRef = true
				
				f << ", "<< itTyp.GetName()
				if isRef f << "*"
				f << " %From" << i

			}
			f << ")\n"
			f << "{\n"
			FncSets[^].PrintPre(f)
			f << "ret void\n"
			f << "}\n"
			
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_PreGetUse
		{	
			if askedCreate == 1
			{
				for itPar,i : Params
				{
					box := new FuncInputBox() ; $temp
					box.itPars.Emplace(itPar.ResultType,true)

					sbRef := false
					if itPar is TypeArr sbRef = true
					if itPar is TypeClass sbRef = true
					box.itPars.Emplace(itPar.ResultType,sbRef)

					fnc := FindFunc("=",this&,box^,false)

					box2 := new FuncInputBox() ; $temp
					box2.itPars.Emplace(ClassType->{Type^},true)
					box2.itConsts.Push(new ObjInt(i))
					
					frmPre := GetNmFunc->{BoxTemplate^}
					frm := frmPre.GetFunc(box2^)
					if fnc != null
					{
						partA := MakeSimpleCall(frm,new ParamNaturalCall("",new FuncParam("ToSet",ClassType->{Type^},true)))
						partB := new ParamNaturalCall("",new FuncParam("From"sbt + i,ClassType->{Type^},false))
						partA.Right = partB
						partB.Left = partA
						FncSets.Push(MakeSimpleCall(fnc,partA))
					}else{
						EmitError("tuple, cant get operator=")
					}
				}
				askedCreate = 2
			}
			//for itPar,i : Params
			//{	
			//	box := new FuncInputBox() ; $temp
			//	//Items.Push(itPar.ResultType)
			//	//Items.Push(itPar.ResultType)

			//	//fnc := FindFunc("=",Up,Items,cc,false)
			//	//if fnc != null
			//	//{
			//	//	if frm != null
			//	//	{
			//	//	}else{
			//	//		EmitError("compiler error 436234")
			//	//	}
			//	//}else{
			//	//	EmitError("can not create tuple")//TODO: fund it calls
			//	//}
			//}
		}
	}
}



CreateTupleTemplate := class extend BoxTemplate
{
	this := !() -> void
	{
	}
	GetPriority := virtual !(FuncInputBox it) -> int
	{
		return 0
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
	}
	CreateFuncPointer := virtual !(FuncInputBox itBox) -> TypeFunc^
	{
		itTpl := GetTuple(itBox&)
		ct := itTpl.ClassType
		cc := ct->{Type^}
		tps := Queue.{Type^}() ; $temp
		tps.Push(itBox.itPars[^].first)
		return GetFuncType(tps,null->{bool^},cc,false,false)
	}
	GetNewFunc := virtual !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars

		ftB := funct->{Type^}
		ctB := funct.RetType->{TypeClass^}
		itTpl := ctB.ToClass->{TupleClass^}
		itTpl.createFunc = funct
		if itTpl.askedCreate == 0
			itTpl.askedCreate = 1

		WorkBag.Push(itTpl,State_PreGetUse)

		toEx := "call " + ftB.GetName() + "@TupleCreate" + itTpl.ClassId + 
			"(" 
		toEx = toEx + ctB.GetName() + "* #0"

		for par,i : pars
		{
			if funct.ParsIsRef[i]
			{
				toEx = toEx + ", " + pars[i].first.GetName() + "* #" + (i + 1)
			}else{
				toEx = toEx + ", " + pars[i].first.GetName() + " #" + (i + 1)
			}
		}
		toEx = toEx + ")\n"

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
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		if pars.Size() != 1 return 255
		if consts.Size() != 1 return 255
		if consts[0].GetValue() != "~int"  return 255
		asN := consts[0]->{ObjInt^}
		val := asN.MyInt
		if val < 0 return 255
		if val >= frmIn.Params.Size() return 255
		return 0
	}
	CreateFuncPointer := virtual !(FuncInputBox itBox) -> TypeFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		asN := consts[0]->{ObjInt^}
		ind := asN.MyInt
		
		itParam := frmIn.Params[ind]

		typs := Queue.{Type^}() ; $temp
		return GetFuncType(typs,null->{bool^},itParam.ResultType,false,false)
	}

	GetNewFunc := virtual !(FuncInputBox itBox, TypeFunc^ funcs) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		asN := consts[0]->{ObjInt^}
		ind := asN.MyInt
		
		itParam := frmIn.Params[ind]
		itType := itParam.ResultType
		return new BuiltInFuncUno(".",pars[0].first,true,itType,true,"#0 = getelementptr " + pars[0].first.GetName() + " , " + pars[0].first.GetName() + "* #1, i32 0, i32 " + ind + "\n")
	}
}

