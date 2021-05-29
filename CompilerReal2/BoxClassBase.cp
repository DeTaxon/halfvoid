BoxClassBase := class extend Object
{
	ClassId := int

	ClassType := TypeClass^
	Params := Queue.{FieldParam^}

	GetNmFunc := TupleFuncGetItem^
	tuplConstr := BoxFunc^
	tuplConstrSets := List.{Object^}
	CreateTupleConstructor := !() -> void
	{
		if tuplConstr == null
		{
			box := new FuncInputBox ; $temp
			box.itPars.Emplace(ClassType,false) ; $temp
			box.itPars.Emplace(Params[^].ResultType,false) ; $temp
			tuplConstr = TCT.GetFunc(box^)
			tuplConstr.IsMethod = true
		}
	}
	tuplCreateWorked := bool
	TupleCreateWork := !() -> void
	{
		if tuplConstr != null and not tuplCreateWorked
		{
			tuplCreateWorked = true
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
				
					if GetNmFunc == null
					{
						GetNmFunc = new TupleFuncGetItem(this&)	
					}
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
					tuplConstrSets.Push(MakeSimpleCall(fnc,partA))
				}else{
					EmitError("tuple, cant get operator=")
				}
			}
		}
	}	
	tuplCmp := TupleSpaceship^
	CreateTupleCmp := !() -> void
	{
		if tuplCmp == null
			tuplCmp = new TupleSpaceship(this&)
	}
	tuplSet := SetTupleValueSimple^
	CreateTupleSet := !() -> void
	{
		if tuplSet == null
			tuplSet = new SetTupleValueSimple(this&)
	}

	PrintCreatedFuncs := !(TIOStream f) -> void
	{
		if tuplCmp != null
			tuplCmp.PrintAsGlobal(f)
		PrintTupleConstruct(f) 
		if tuplSet != null
		{
			funcs := tuplSet.GetCreatedFuncs()->{SetTupleObj^[]}
			if funcs != null
			{
				for it : funcs
				{
					it.PrintItFunc(f)
				}
			}
		}
	}
	PrintTupleConstruct := !(TIOStream f) -> void
	{
		if tuplConstr == null
			return void
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
		tuplConstrSets[^].PrintPre(f)
		f << "ret void\n"
		f << "}\n"
	}
}
