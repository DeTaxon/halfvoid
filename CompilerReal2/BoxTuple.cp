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

SimpleSetTuple := class
{
	itId := int
	inpDataFuncType := TypeFunc^
	setsFunc := Queue.{Object^}

	this := !(int newId,TypeFunc^ iF) -> void
	{
		itId = newId
		inpDataFuncType = iF
		setsFunc."this"()
	}
	"=" := !(SimpleSetTuple tp) -> void
	{
	}
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

	setSimple := SetTupleValueSimple^
	simpleSetters := Queue.{SimpleSetTuple}
	setSimpleNeedCheck := bool

	cttAsFunc := BoxFunc^


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
		FncSetVal = new BuiltInFuncBinar("=",cnPre,true,cnPre,true,GTypeVoid,false,
			"call void("sbt + cn + "*," + cn + "*)@TupleSet" + ClassId + "(" + cn + "* #1," + cn + "* #2)\n")

	}
	GetFunc := virtual !(string name,FuncInputBox itBox, bool isV) -> BoxFunc^
	{
		if name == "."
		{
			return GetNmFunc.GetFunc(itBox)
		}
		if name == "="
		{
			if setSimple == null
				setSimple = new SetTupleValueSimple(this&)
			if setSimple.GetPriority(itBox) == 255
				return null
			return setSimple.GetFunc(itBox)
		}
		if name == "this"
		{
			if cttAsFunc == null
			{
				box := new FuncInputBox ; $temp
				box.itPars.Emplace(ClassType,false)
				box.itPars.Emplace(Params[^].ResultType,false)
				cttAsFunc =TCT.GetFunc(box^)
				cttAsFunc.IsMethod = true
			}
			return cttAsFunc
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
		if DebugMode
		{
			f << "!" << ClassId << " = !DICompositeType(tag: DW_TAG_structure_type, name: \"" << "Tuple" << "\""
			f << ", elements: !{})\n"
		}
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
		for simpleSetters
		{
			
			f << "define void @TupleSetSimple" << it.itId << "(" 
			f << it.inpDataFuncType.Pars[0].GetName() << "* %ToSet"
			f << ","
			f << it.inpDataFuncType.Pars[1].GetName() << "* %ToGet"
			f << ")\n{\n"
			it.setsFunc[^].PrintPre(f)
			f << "ret void\n"
			f << "}\n"

		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_PreGetUse
		{
			if setSimpleNeedCheck
			{
				setSimpleNeedCheck = false

				for ch : simpleSetters
				{
					if ch.setsFunc.Size() != 0
						continue
					
					asT := ch.inpDataFuncType.Pars[1]->{TypeClass^}.ToClass->{TupleClass^}
					i := 0
					for par : Params, parC : asT.Params 
					{
						boxS := new FuncInputBox() ; $temp

						boxS.itPars.Emplace(par.ResultType,true)

						isRef := false
						if parC is TypeClass isRef = true
						if parC is TypeArr isRef = true
						boxS.itPars.Emplace(parC.ResultType,isRef)

						func := FindFunc("=",this&,boxS^,false)

						if func != null
						{
							oneInt := new ObjInt(i)
							box2 := new FuncInputBox() ; $temp
							box2.itPars.Emplace(ClassType->{Type^},true)
							box2.itConsts.Push(oneInt)
							
							frmPre := GetNmFunc->{BoxTemplate^}
							frm := frmPre.GetFunc(box2^)

							box3 := new FuncInputBox ; $temp
							box3.itPars.Emplace(asT.ClassType,true)
							box3.itConsts.Push(oneInt)
							frmPre2 := asT.GetNmFunc->{BoxTemplate^}
							frm2 := frmPre2.GetFunc(box3^)

							partA := MakeSimpleCall(frm ,new ParamNaturalCall("",new FuncParam("ToSet",ClassType->{Type^},true)))
							partB := MakeSimpleCall(frm2,new ParamNaturalCall("",new FuncParam("ToGet",asT.ClassType,true)))

							partA.Right = partB
							partB.Left = partA
							ch.setsFunc.Push(MakeSimpleCall(func,partA))
						}else{
							EmitError("tuple = tuple, cant get operator= for parameter ."sbt + i)
						}
						i++
					}
				}
			}
			if askedCreate == 1
			{
				for itPar,i : Params
				{
					
					fnc := BoxFunc^()
					if itPar.ResultType is TypeClass
					{
						asCl := itPar.ResultType->{TypeClass^}.ToClass
						boxCl := new FuncInputBox() ; $temp
						boxCl.itPars.Emplace(itPar.ResultType,true)
						boxCl.itPars.Emplace(itPar.ResultType,true)
						fnc = asCl.GetFunc("this",boxCl^,false)
					}
					if fnc == null
					{
						box := new FuncInputBox() ; $temp
						box.itPars.Emplace(itPar.ResultType,true)

						sbRef := false
						if itPar is TypeArr sbRef = true
						if itPar is TypeClass sbRef = true
						box.itPars.Emplace(itPar.ResultType,sbRef)

						fnc = FindFunc("=",this&,box^,false)
					}

					if fnc != null
					{
						box2 := new FuncInputBox() ; $temp
						box2.itPars.Emplace(ClassType->{Type^},true)
						box2.itConsts.Push(new ObjInt(i))
						
						frmPre := GetNmFunc->{BoxTemplate^}
						frm := frmPre.GetFunc(box2^)

						partA := MakeSimpleCall(frm,new ParamNaturalCall("",new FuncParam("ToSet",ClassType->{Type^},true)))

						parBType := itPar.ResultType
						parBIsRef := false
						if parBType is TypeClass parBIsRef = true
						if parBType is TypeArr parBIsRef = true
						partB := new ParamNaturalCall("",new FuncParam("From"sbt + i,itPar.ResultType,parBIsRef))
						partA.Right = partB
						partB.Left = partA
						FncSets.Push(MakeSimpleCall(fnc,partA))
					}else{
						EmitError("tuple, cant get operator=")
					}
				}
				askedCreate = 2
			}
		}
	}
}

SetTupleValueSimple := class extend BoxTemplate
{
	ptrToTuple := TupleClass^
	this := !(TupleClass^ itt) -> void
	{
		ptrToTuple = itt
	}
	GetPriority := virtual !(FuncInputBox it) -> int
	{
		if it.itPars.Size() != 2 return 255
		if it.itPars[0].first is TypeClass and it.itPars[1].first is TypeClass
		{
		}else{
			return 255
		}
		tpA := it.itPars[0].first->{TypeClass^}.ToClass
		tpB := it.itPars[1].first->{TypeClass^}.ToClass
		if tpA is TupleClass and tpB is TupleClass
		{
		}else{
			return 255
		}
		aA := tpA->{TupleClass^}
		aB := tpB->{TupleClass^}
		if aA.Params.Size() != aB.Params.Size()
			return 255
		maxCmp := 0
		for inL : aA.Params, inR : aB.Params
		{
			val := TypeCmp(inR.ResultType,inL.ResultType)
			if val > maxCmp
				maxCmp = val
		}
		return maxCmp

	}
	CreateFuncPointer := virtual !(FuncInputBox itBox) -> TypeFunc^
	{
		tps := Queue.{Type^}() ; $temp
		tps.Push(itBox.itPars[^].first)
		return GetFuncType(tps,null->{bool^},GTypeVoid,false,false)
	}
	GetNewFunc := virtual !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		itID := GetNewId()
		ptrToTuple.setSimpleNeedCheck = true
		ptrToTuple.simpleSetters.Emplace(itID,funct)
		WorkBag.Push(ptrToTuple,State_PreGetUse)

		pars := ref itBox.itPars
	
		toEx := ""sbt
		toEx << "call void @TupleSetSimple" << itID << "(" 

		for par,i : pars
		{
			if i != 0
				toEx << ", "
			if funct.ParsIsRef[i]
			{
				toEx << par.first.GetName() << "* #" << i + 1
			}else{
				toEx << par.first.GetName() << " #" << i + 1
			}
		}
		toEx << ")\n"

		return new BuiltInFuncMega("",funct,toEx)
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

		toEx := "call "sbt << ftB.GetName() << "@TupleCreate" << itTpl.ClassId << 
			"(" 
		toEx << ctB.GetName() << "* #0"

		for par,i : pars
		{
			if funct.ParsIsRef[i]
			{
				toEx + ", " + pars[i].first.GetName() + "* #" + (i + 1)
			}else{
				toEx + ", " + pars[i].first.GetName() + " #" + (i + 1)
			}
		}
		toEx + ")\n"

		return new BuiltInFuncMega("",funct,toEx)
	}
}
TupleConstructorTemplate := class extend BoxTemplate
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
		return GetFuncType(tps,null->{bool^},GTypeVoid,false,false)
	}
	GetNewFunc := virtual !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars

		ftB := funct->{Type^}
		itTpl := funct.Pars[0]->{TypeClass^}.ToClass->{TupleClass^}
		if itTpl.askedCreate == 0
			itTpl.askedCreate = 1

		WorkBag.Push(itTpl,State_PreGetUse)

		toEx := "call "sbt << ftB.GetName() << "@TupleCreate" << itTpl.ClassId << 
			"(" 

		for par,i : pars
		{
			if i != 0
				toEx << ", "
			if funct.ParsIsRef[i]
			{
				toEx + pars[i].first.GetName() + "* #" + (i + 1)
			}else{
				toEx + pars[i].first.GetName() + " #" + (i + 1) 
			}
		}
		toEx + ")\n"

		return new BuiltInFuncMega("",funct,toEx)
	}
}


TupleFuncGetItem := class extend BoxTemplate
{
	frmIn := TupleClass^
	this := !(TupleClass^ frmOn) -> void
	{
		frmIn = frmOn
		IsPassAttrs = true
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
		preRet :=  new BuiltInFuncUno(".",pars[0].first,true,itType,true,
			"#0 = getelementptr "sbt + pars[0].first.GetName() + " , " + pars[0].first.GetName() + "* #1, i32 0, i32 " + ind + "\n")
		preRet.IsSelfReturn = true
		return preRet
	}
}

