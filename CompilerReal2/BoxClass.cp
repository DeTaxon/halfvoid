#import "Tree.cp"
#import "BuiltInFuncs.cp"
#import "LocalParam.cp"
#import "CmpConstObjs.cp"

ParseClass := !(Object^ ob)-> BoxClass^
{
	
	if ob == null return null

	iterT := ob.Down
	if iterT == null return null
	if iterT.GetValue() != "class" return null

	iterT = iterT.Right

	if iterT == null return null

	if iterT.GetValue() == "."
	{
		iterT = iterT.Right.Right
		preRes := new BoxClassTemplate(ob,null->{Object^})
		return preRes->{BoxClass^}
	}

	ExtObj := Object^
	ExtObj = null
	
	if iterT.GetValue() == "extend"
	{
		iterT = iterT.Right
		if iterT == null return null

		ExtObj = iterT 

		iterT = iterT.Right
		if iterT == null return null
		if iterT.GetValue() != "{}" return null

		//if IsTemplate return new BoxClassTemplate(ob,ExtObj)
		return new BoxClass(iterT,null->{BoxClass^},ExtObj)

	}

	if iterT.GetValue() != "{}" return null

	return new BoxClass(iterT,null->{BoxClass^},null->{Object^})
}

GetUpClass := !(Object^ toS) -> BoxClass^
{
	iterF := toS
	while iterF != null
	{
		if iterF.GetValue() == "{...}" return iterF->{BoxClass^}
		iterF = iterF.Up
	}
	return null
}

BoxClassTemplate := class extend Object
{
	Classes := Stack.{BoxClass^}
	ClassTree := Object^
	ConstTree := Object^
	ExtendTree := Object^

	TempConsts := Queue.{ObjConstHolder^}^

	this := !(Object^ tree,Object^ txt) -> void
	{
		ClassTree = tree.Clone()
		ClassTree.SetUp(this&)

		ConstTree = ClassTree.Down.Right.Right

		PopOutNode(ClassTree.Down.Right)		
		PopOutNode(ClassTree.Down.Right)
		ConstTree.SetUp(this&)

		if ClassTree.Down.Right.GetValue() == "extend"
		{
			ExtendTree = ClassTree.Down.Right.Right.Clone()
			ExtendTree.Up = this&

			PopOutNode(ClassTree.Down.Right)
			PopOutNode(ClassTree.Down.Right)
		}

		MakeGoodConsts(ConstTree)
	}
	GetItem := virtual !(string name) -> Object^
	{
		if TempConsts != null
		{
			itr := TempConsts.Start
			while itr != null
			{
				if itr.Data.ItName == name return itr.Data.Down
				itr = itr.Next
			}
		}
		return null
	}
	GetClass := !(Queue.{Object^} stuf) -> TypeClass^
	{
		for i : Classes.Size()
		{	
			if Classes[i].IsSameConsts(stuf)
			{
				return Classes[i].ClassType
			}
		}
		newConsts := Queue.{ObjConstHolder^}()
		if not IsEqConsts(ConstTree,stuf,newConsts)
		{
			return null
		}

		newTree := ClassTree.Clone()
		newTree.Up = this&
		
		inher := BoxClass^
		inher = null

		treeIter := newTree.Down.Right

		if ExtendTree != null
		{
			TempConsts = newConsts&
			inher = ParseType(ExtendTree)->{BoxClass^}
			TempConsts = null
			if inher == null return null
			inher = ((inher->{TypeClass^}).ToClass)
		}
		
		newEx := Object^
		newEx = null
		if ExtendTree != null{
			newEx = ExtendTree.Clone()
		}
		newClass := new BoxClass(treeIter,inher,newEx)
		newClass.Up = this&
		if newEx != null{
			newEx.Up = newClass
		}

		newClass.Right = Down
		if Down != null Down.Left = newClass
		Down = newClass


		for i : stuf.Size()
		{
			newClass.ItConsts.Push(stuf[i])
		}

		for j : newConsts.Size()
		{
			newClass.ItVals.Push(newConsts[j])
		}

		Classes.Push(newClass)
		return newClass.ClassType
	}
	GetValue := virtual !() -> string
	{
		return "!{}{...}"
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
	}
}

FuncItem := class
{
	fName := string
	fType := TypeFunc^
	fItem := BoxFunc^
	funcWrapper := BoxFunc^

	CmpItem := !(FuncItem^ Ri) -> bool
	{	
		if fName != Ri.fName return false
		if fType.RetType != Ri.fType.RetType return false
		if fType.ParsCount != Ri.fType.ParsCount return false
		if fType.IsVArgs != Ri.fType.IsVArgs return false

		for i : fType.ParsCount
		{
			if i == 0
			{
				//TODO:no need to check inherence?				
			}else{
				//if fType.Pars[i] != Ri.fType.Pars[i] return false
			}
		}

		return true
	}
}

BoxClass := class extend Object
{
	ClassId := int
	NotMineParams := int

	Params := Queue.{FieldParam^}
	FakeParams := Queue.{FakeFieldParam^}

	ItMethods := Queue.{BoxFunc^}
	ThislessFuncs := Queue.{BuiltInThislessFunc^}

	ClassType := TypeClass^
	UnrollTemplate := BuiltInTemplateUnroll^
	AutoFieldTemplate := BuiltInTemplateAutoField^

	ExtendObject := Object^
	Parent := BoxClass^

	ContainVirtual  := bool

	ItVals := Queue.{ObjConstHolder^}
	ItConsts := Queue.{Object^}

	GetClassOutputName := !() -> string
	{
		return "%Class" + ClassId
	}

	GetWrappedFunc := !(string name ,Queue.{BoxFunc^} funcs, Queue.{BoxTemplate^} templs) -> void
	{
		gotFuncs := Queue.{BoxFunc^}()
		funcsCl := Queue.{BoxClass^}()

		iterF := this&

		while iterF != null
		{
			ptrToQ := iterF.ItMethods&
			itSize := ptrToQ^.Size()
			qIter := ptrToQ.Start
			while qIter != null
			{
				if qIter.Data.FuncName == name
				{
					gotFuncs.Push(qIter.Data)
					funcsCl.Push(iterF)
				}
				qIter = qIter.Next
			}
			iterF = iterF.Parent
		}

		gotFuncsIter := gotFuncs.Start
		funcsClIter := funcsCl.Start
		while gotFuncsIter != null
		{
			inTh := BoxFunc^
			inTh = null
			ThislessIter := ThislessFuncs.Start
			while ThislessIter != null
			{
				if ThislessIter.Data.itFunc == gotFuncsIter.Data
				{
					inTh = ThislessIter.Data
					ThislessIter = null
				}else{
					ThislessIter = ThislessIter.Next
				}

			}
			if inTh != null{
			}else{
				toCr := gotFuncsIter.Data

				inThPre :=  new BuiltInThislessFunc(toCr,this&->{BoxClass^},funcsClIter.Data)
				ThislessFuncs.Push(inThPre)
				inTh = inThPre
			}
			funcs.Push(inTh)
			gotFuncsIter = gotFuncsIter.Next
			funcsClIter = funcsClIter.Next
		}
	}

	IsSameConsts := !(Queue.{Object^} consts) -> bool
	{
		if consts.Size() != ItConsts.Size() {
			return false
		}

		for i : consts.Size()
		{
			if not CmpConstObjs(consts[i],ItConsts[i])
				return false
		}

		return true
	}

	GetItem := virtual !(string name) -> Object^
	{
		for ItVals.Size() 
			if ItVals[it].ItName == name
				return ItVals[it].Down
		return null
	}

	this := !(Object^ item, BoxClass^ par,Object^ extItem) -> void 
	{
		Line = item.Line	
		ExtendObject = extItem

		Down = item
		Down.SetUp(this&)
		Down.Left = null
		MakeItBlock(Down)
		Down.SetUp(this&)
		WorkBag.Push(Down,State_Start)
		WorkBag.Push(this&,State_PreGetUse)

		ClassId = GetNewId()
		ClassType = new TypeClass(this&)
		UnrollTemplate = new BuiltInTemplateUnroll(this&)
		AutoFieldTemplate = new BuiltInTemplateAutoField(this&)

		this.Parent = par

		if par != null 
		{
			if par.ContainVirtual ContainVirtual = true
		}
		iterH := Down.Down
		while iterH != null
		{
			if iterH.GetValue() == "virtual" ContainVirtual = true
			iterH = iterH.Right
		}
		WorkBag.Push(this&,State_CheckBaseClass)
		WorkBag.Push(this&,State_PrePrint)
		Classes.Push(this&)

	}

	Inherited := false
	InheritParams := !() -> void
	{
		if not Inherited
		{
			Inherited = true
			parentTree := this.Parent
			while parentTree != null
			{
				parentTree.InheritParams()
				Size := parentTree.Params.Size()
				NotMineParams = Size
				for i : Size
				{
					Params.PushFront(parentTree.Params[Size - i - 1])
				}
				for i : parentTree.FakeParams.Size() FakeParams.PushFront(parentTree.FakeParams[i])
				if parentTree.ContainVirtual 
					ContainVirtual = true

				//parentTree = parentTree.Parent
				parentTree = null
			}
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_CheckBaseClass
		{
			if ExtendObject != null{
				newType := ParseType(ExtendObject)
				if newType == null{
					EmitError("Can not extend type\n")
				}else{
					if newType.GetType() != "class"{
						EmitError("wip\n")
					}else{
						PreParent := newType->{TypeClass^}
						Parent = PreParent.ToClass
					}
				}
			}
		}
		if pri == State_PreGetUse
		{
			InheritParams()

			iterH := Down.Down
			foundNotThis := false

			while iterH != null
			{
				if iterH.GetValue() == "i:=1"
				{
					asParam := iterH->{ObjParam^}

					if asParam.MyStr == "~this"
						foundNotThis = true
				}
				iterH = iterH.Right
			}

			if not foundNotThis
			{
				newPrm := new ObjParam("~this",false)
				newPrm.Right = Down.Down
				newPrm.Up = Down
				if Down.Down != null
				{
					newPrm.Right.Left = newPrm
					Down.Down = newPrm
				}else{
					Down.Down = newPrm
				}

				pars := Queue.{Type^}()
				pars.Push(ClassType)
				bools := true
				funcT := GetFuncType(pars,bools&,GetType("void"),false,false)
				
				names := new char^[1]
				names[0] = "this"
				newFunc := new BoxFuncBody(names,funcT,"~this",(new ObjSkobs("{}"))->{Object^},false,ClassType->{Type^},ContainVirtual)
				newPrm.Down = newFunc
				newPrm.Down.Up = newPrm

				//newFunc.ParseBlock()
			}

		}
		if pri == State_PrePrint
		{
			ComputeVTable()		
		}
	}
	GetValue := virtual !() -> string
	{
		return "{...}"
	}
	//GetMethod
	GetFunc := virtual !(string name) -> BoxFunc^
	{
		Pars := Queue.{Type^}()
		temp := Queue.{Object^}()
		return GetFunc(name,Pars,temp)
	}
	GetFunc := virtual !(string name,Queue.{Type^} pars) -> BoxFunc^
	{
		temp := Queue.{Object^}()
		return this.GetFunc(name,pars,temp)
	}
	//GetMethod
	GetFunc := virtual !(string name,Queue.{Type^} pars, Queue.{Object^} consts) -> BoxFunc^
	{
		return GetFunc(name,pars,consts,false)
	}
	GetFunc := virtual !(string name,Queue.{Type^} pars, Queue.{Object^} consts,bool iVir) -> BoxFunc^
	{
		Funcs := Queue.{BoxFunc^}()
		Templs := Queue.{BoxTemplate^}()

		if not iVir
		{
			for i : vTypes.Size()
			{
				if vTypes[i].fName == name
				{
					Funcs.Push(vTypes[i].funcWrapper)
				}
			}
		}

		iterJ := Down.Down
		while iterJ != null
		{
			if iterJ.GetValue() == "i:=1"
			{
				itName := ((iterJ->{ObjParam^}).MyStr)
				if itName == name
				{
					if iterJ.Down.GetValue() == "!()"
					{
						asFunc := (iterJ.Down)->{BoxFunc^}
						if (not asFunc.IsVirtual or iVir) and asFunc.IsSameConsts(consts)
							Funcs.Push(asFunc)
					}
					if iterJ.Down.GetValue() == "!(){}"
						Templs.Push((iterJ.Down)->{BoxTemplate^})
				}
			}
			iterJ = iterJ.Right
		}
		if name == "."{
			Templs.Push(UnrollTemplate->{BoxTemplate^})
		}
		bestFunc := GetBestFunc(pars,consts,Funcs,Templs)
		if bestFunc != null WorkBag.Push(bestFunc,State_GetUse)

		if bestFunc == null and this.Parent != null and name != "this"
		{
			pars2 := Queue.{Type^}()
			pars2.Push(this.Parent.ClassType)
			for i : pars.Size()
			{
				if i != 0
					pars2.Push(pars[i])
			}
			return this.Parent.GetFunc(name,pars2,consts)
		}

		return bestFunc
	}

	vTypes := Queue.{FuncItem^}
	vTable := Queue.{FuncItem^}

	computedVTable := bool
	ComputeVTable := !() -> void
	{	
		if not computedVTable
		{
			computedVTable = true

			if this.Parent != null
			{
				this.Parent.ComputeVTable()
				for i : this.Parent.vTable.Size()
				{
					vTable.Push(this.Parent.vTable[i])
				}
			}

			for i : vTypes.Size()
			{
				found := false
				for j : vTable.Size()
				{
					if vTable[j].CmpItem(vTypes[i])
					{
						vTable[j] = vTypes[i]
						aS := vTypes[i].funcWrapper
						aB := aS->{BuiltInVirtualCall^}
						aB.MakeLine(j)
						found = true
					}
				}
				if not found
				{
					aS2 := vTypes[i].funcWrapper
					aB2 := aS2->{BuiltInVirtualCall^}
					aB2.MakeLine(vTable.Size())
					vTable.Push(vTypes[i])
				}
			}
		}
	}
	PrintStruct := virtual !(sfile f) -> void
	{
		if not vTable.Empty()
		{
			f << "%ClassTableType" << ClassId << " = type {"
			for vTable.Size()
			{
				asBasePre := vTable[it].fType
				asBase2 := asBasePre->{Type^}
				if it > 0 f << " , "
				f << asBase2.GetName() << "*"
			}
			f << " }\n"

			f << "@ClassTableItem" << ClassId << " = global %ClassTableType" << ClassId << " {"
			for i : vTable.Size()
			{
				asBasePre := vTable[i].fType
				asBase2 := asBasePre->{Type^}
				if i > 0 f << " , "
				f << asBase2.GetName() << "* @" << vTable[i].fItem.OutputName
			}
			f << "}\n"
		}
		if Params.Size() == 0
		{
			if vTable.Empty()
			{
				f << "%Class" << ClassId << " = type{i1}\n"
			}else{
				f << "%Class" << ClassId << " = type{ %ClassTableType" << ClassId << "*}\n"
			}
		}else{
			f << "%Class" << ClassId << " = type {"
			if not vTable.Empty()
			{
				f << "%ClassTableType" << ClassId << "*"
			}
			for Params.Size()
			{
				if it != 0 or not vTable.Empty() f <<","
				f << Params[it].ResultType.GetName()
			}
			f << "}\n"
			if DebugMode
			{
				f << "!" << ClassId << " = !DICompositeType(tag: DW_TAG_structure_type,  
			}
		}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		f << "define void @ClassExtraConstructor" << ClassId << "(%Class" << ClassId << "* %this)\n"
		f << "{\n"
		if ContainVirtual
		{
			f << "%TmpPt" << ClassId << " = getelementptr %Class" << ClassId << " , %Class" << ClassId << "* "
			f << "%this"
			f << ", i32 0, i32 0\n"
			f << "store %ClassTableType" << ClassId << "* @ClassTableItem" << ClassId <<  ", %ClassTableType" << ClassId << "** %TmpPt" << ClassId << "\n"
		}

		for myStuf : this.Params.Size()
		{
			itParm := this.Params[myStuf]
			asBase := itParm->{Object^}
			if asBase.GetType().GetType() == "class"
			{
				clTyp := asBase.GetType()->{TypeClass^}
				classItm := clTyp.ToClass
				if classItm.ContainVirtual
				{
					pos := myStuf
					if this.ContainVirtual pos += 1
					f << "%itrItm" << pos << " = getelementptr %Class" << ClassId << " , %Class" << ClassId
					f << "* %this" <<", i32 0, i32 " << pos <<"\n"
					classItm.ApplyConstants(f,"%itrItm" + pos)
				}
			}

		}
		f << "ret void\n}\n"

		iterJ := Down.Down
		while iterJ != null
		{
			if iterJ.GetValue() == "i:=1"
			{
				if iterJ.Down.GetValue() == "!()"
				{
					iterJ.PrintGlobal(f)
				}
			}
			iterJ = iterJ.Right
		}
	}
	PutVirtualFunc := virtual !(string fNam,TypeFunc^ fTyo,BoxFunc^ fF) -> void
	{
		newItem := new FuncItem
		newItem.fName = fNam
		newItem.fType = fTyo
		newItem.fItem = fF
		newItem.funcWrapper = new BuiltInVirtualCall(fNam,fF,ClassId)

		vTypes.Push(newItem)
	}
	ApplyConstants := !(sfile f,Object^ itm) -> void
	{
		if itm.GetType().GetType() == "point"
		{
			ApplyConstants(f,itm.GetName())
		}else{
			ApplyConstants(f,itm.GetPointName())
		}
	}
	ApplyConstants := !(sfile f,string itm) -> void
	{
		f << "call void(%Class" << ClassId << "*)@ClassExtraConstructor" << ClassId <<"(%Class" << ClassId << "* " 
		f << itm
		f << ")\n"
	}
}

BuiltInVirtualCall := class extend BuiltInFunc
{
	classId := int
	this := !(string name, BoxFunc^ from, int classID) -> void
	{
		classId = classID
		MyFuncType = from.MyFuncType
		FuncName = name
		ToExe  = ""	
	}
	MakeLine := !(int id) -> void
	{
		aseBase := MyFuncType->{Type^}
		FuncTypeName := aseBase.GetName()
		
		ToExe = "%FuncTabel## = getelementptr %Class" + classId + " , %Class" + classId + "* #1, i32 0, i32 0\n" 
		ToExe = ToExe + "%PreFunc## = load %ClassTableType" + classId + "* , %ClassTableType" + classId + "** %FuncTabel##\n"
		ToExe = ToExe + "%FuncPtr## = getelementptr %ClassTableType" + classId + " , %ClassTableType" + classId + "* %PreFunc##, i32 0, i32 " + id + "\n"
		ToExe = ToExe + "%Func## = load " + FuncTypeName + "* , " + FuncTypeName + "** %FuncPtr##\n" 
		if MyFuncType.RetType != GetType("void")
			ToExe = ToExe + "#0 = "
		ToExe = ToExe + "call " + FuncTypeName +  "%Func##("
		for i : MyFuncType.ParsCount
		{
			if i > 0 ToExe = ToExe + " , "
			if MyFuncType.ParsIsRef[i]
			{
				ToExe = ToExe + MyFuncType.Pars[i].GetName() + "* "
			}else{
				ToExe = ToExe + MyFuncType.Pars[i].GetName() + " "
			}
			ToExe = ToExe + "#" + (i + 1)
		}

		ToExe = ToExe + ")\n"
	}

}
BuiltInThislessFunc := class extend BuiltInFunc
{
	itFunc := BoxFunc^
	itClass := BoxClass^
	itInClass := BoxClass^
	this := !(BoxFunc^ toFunc,BoxClass^ toClass,BoxClass^ inClass) -> void
	{
		toFunc.ParseBlock()
		itFunc = toFunc
		itClass = toClass
		itInClass = inClass
		FuncName = toFunc.FuncName
		OutputName = toFunc.OutputName

		newTypes := Queue.{Type^}()
		itsBools := Queue.{bool}()

		fTyp := toFunc.MyFuncType

		i := 1
		while i < fTyp.ParsCount
		{
			if fTyp.ParsIsRef == null{
				itsBools.Push(false)
			}else{
				itsBools.Push(fTyp.ParsIsRef[i])
			}
			newTypes.Push(fTyp.Pars[i])
			i += 1
		}
		MyFuncType = GetFuncType(newTypes,itsBools.ToArray(),fTyp.RetType,fTyp.RetRef,fTyp.IsVArgs)
		ToExe = ""
		MakeLine()
	}
	MakeLine := !() -> void
	{
		aseBase := MyFuncType->{Type^}
		FuncTypeName := aseBase.GetName()

		isRetComp := MyFuncType.RetRef
		if not isRetComp
		{
			if MyFuncType.RetType != null
			{
				isRetComp = MyFuncType.RetType.GetType() == "class" //TODO in ["class","fixarr"]
			}
		}
		ToExe = ToExe + "%NewThis## = bitcast " + itClass.GetClassOutputName() + "* %this to " + itInClass.GetClassOutputName() + "*\n"
		if MyFuncType.RetType != GetType("void") and not isRetComp
			ToExe = ToExe + "#0 = "
		fTypp := itFunc.MyFuncType
		fTypp2 := fTypp->{Type^}
		ToExe = ToExe + "call " + fTypp2.GetName()  + "@" + OutputName + "("
		ToExe = ToExe + itInClass.GetClassOutputName() + "* %NewThis##"
		for i : MyFuncType.ParsCount
		{
			ToExe = ToExe + " , "
			if MyFuncType.ParsIsRef[i]
			{
				ToExe = ToExe + MyFuncType.Pars[i].GetName() + "* "
			}else{
				ToExe = ToExe + MyFuncType.Pars[i].GetName() + " "
			}
			ToExe = ToExe + "#" + (i + 1)
		}

		ToExe = ToExe + ")\n"
	}
}


