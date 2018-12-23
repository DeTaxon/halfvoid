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
	GetClass := !(FuncInputBox itBox) -> TypeClass^
	{
		for cl : Classes
		{	
			if cl.IsSameConsts(itBox)
			{
			
				return cl.ClassType
			}
		}
		newConsts := Queue.{ObjConstHolder^}()
		if not IsEqConsts(ConstTree,itBox,newConsts)
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
	

		for it : itBox.itConsts
		{
			newClass.ItConsts.Push(it)
		}


		for newConsts
		{
			newClass.ItVals.Push(it)
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
	fConstVal := Object^
	fTyp := Type^

	funcWrapper := BoxFunc^

	CmpItem := !(FuncItem^ Ri) -> bool
	{	
		if (fConstVal != null) !=  (Ri.fConstVal != null) return false
		if fConstVal != null {
			if fName != Ri.fName return false
			return fTyp == Ri.fTyp
		}else{
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
	VirtualCheck := BuiltInCheckVirtualType^

	ExtendObject := Object^
	Parent := BoxClass^

	ContainVirtual  := bool

	ItVals := Queue.{ObjConstHolder^}
	ItConsts := Queue.{Object^}

	ClassName := string

	GetClassOutputName := !() -> string
	{
		return "%Class" + ClassId
	}

	GetWrappedFunc := !(string name ,Queue.{BoxFunc^} funcs, Queue.{BoxTemplate^} templs) -> void
	{
		gotFuncs := Queue.{BoxFunc^}()
		funcsCl := Queue.{BoxClass^}()

		iterF := this&

		//if name == "new" {
		//	return void
		//}

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
				if not toCr.IsStatic {
					inThPre :=  new BuiltInThislessFunc(toCr,this&->{BoxClass^},funcsClIter.Data)
					ThislessFuncs.Push(inThPre)
					inTh = inThPre
				}
			}
			if inTh != null funcs.Push(inTh)
			gotFuncsIter = gotFuncsIter.Next
			funcsClIter = funcsClIter.Next
		}
	}

	IsSameConsts := !(FuncInputBox itBox) -> bool
	{
		if itBox.itConsts.Size() != ItConsts.Size() {
			return false
		}

		for c : itBox.itConsts , i : 0
		{
			if not CmpConstObjs(c,ItConsts[i])
				return false
		}

		return true
	}

	GetItem := virtual !(string name) -> Object^
	{
		for ItVals
			if it.ItName == name
				return it.Down
		return null
	}

	this := !(Object^ item, BoxClass^ par,Object^ extItem) -> void 
	{
		ClassName = "anon"
		if item != null Line = item.Line	
		ExtendObject = extItem

		Down = item
		if Down != null
		{
			Down.SetUp(this&)
			Down.Left = null
			MakeItBlock(Down)
			Down.SetUp(this&)
			WorkBag.Push(Down,State_Start)
		}
		WorkBag.Push(this&,State_PreGetUse)

		ClassId = GetNewId()
		ClassType = new TypeClass(this&)
		ClassType.metaId = ClassId
		UnrollTemplate = new BuiltInTemplateUnroll(this&)
		AutoFieldTemplate = new BuiltInTemplateAutoField(this&)
		VirtualCheck = new BuiltInCheckVirtualType(ClassType)

		this.Parent = par

		if par != null 
		{
			if par.ContainVirtual ContainVirtual = true
		}
		if Down != null
		{
			iterH := Down.Down
			while iterH != null
			{
				if iterH.GetValue() == "virtual" ContainVirtual = true
				iterH = iterH.Right
			}
		}
		WorkBag.Push(this&,State_Start)
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
				for fakes : parentTree.FakeParams FakeParams.PushFront(fakes)
				if parentTree.ContainVirtual 
					ContainVirtual = true

				//parentTree = parentTree.Parent
				parentTree = null
			}
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			itr := Up
			while itr != null and itr.GetValue() != "i:=1" itr = itr.Up
			if itr != null{
				asN := itr->{ObjParam^}
				ClassName = asN.MyStr
			}
		}
		if pri == State_CheckBaseClass
		{
			if ExtendObject != null{
				newType := ParseType(ExtendObject)
				if newType == null{
					EmitError("Can not extend type\n")
				}else{
					if not newType is TypeClass{
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

			if Down == null return void
			iterH := Down.Down
			foundNotThis := false

			while iterH != null
			{
				if iterH is ObjParam//iterH.GetValue() == "i:=1"
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
				funcT := GetFuncType(pars,bools&,GTypeVoid,false,false)
				
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
	GetFunc := virtual !(string name,FuncInputBox itBox,bool iVir) -> BoxFunc^
	{
		Funcs := Queue.{BoxFunc^}()
		Templs := Queue.{BoxTemplate^}()

		if not iVir
		{
			for vTyp : vTypes
			{
				if vTyp.fName == name
				{
					Funcs.Push(vTyp.funcWrapper)
				}
			}
		}

		if Down != null
		{
			iterJ := Down.Down
			while iterJ != null
			{
				if iterJ is ObjParam //iterJ.GetValue() == "i:=1"
				{
					itName := ((iterJ->{ObjParam^}).MyStr)
					if itName == name
					{
						if iterJ.Down.GetValue() == "!()"
						{
							asFunc := (iterJ.Down)->{BoxFunc^}
							if ((not asFunc.IsVirtual) or iVir) and asFunc.IsSameConsts(itBox)
							{
								Funcs.Push(asFunc)
							}
						}
						if iterJ.Down.GetValue() == "!(){}"
							Templs.Push((iterJ.Down)->{BoxTemplate^})
					}
				}
				iterJ = iterJ.Right
			}
		}
		if name == "."{
			Templs.Push(UnrollTemplate->{BoxTemplate^})
		}
		bestFunc := GetBestFunc(itBox,Funcs,Templs)
		if bestFunc != null WorkBag.Push(bestFunc,State_GetUse)

		if bestFunc == null and this.Parent != null and name != "this"
		{
			oldVal := Type^
			if name != "new"
			{
				oldVal = itBox.itPars[0].first
				itBox.itPars[0].first = Parent.ClassType
			}
			res :=  this.Parent.GetFunc(name,itBox,true)
			if name != "new" { itBox.itPars[0].first = oldVal }

			return res
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
				for this.Parent.vTable
				{
					vTable.Push(it)
				}
			}

			for invTypes : vTypes// i : 0
			{
				found := false
				for invTable : vTable , j : 0
				{
					if invTable.CmpItem(invTypes)
					{
						invTable = invTypes
						if invTypes.fConstVal != null
						{
							aB := invTypes.fItem->{BuiltInGetVirtualParam^}
							aB.MakeLine(j)
						}else{
							invTypes.fItem.VirtualId = j
							aS := invTypes.funcWrapper
							aB := aS->{BuiltInVirtualCall^}
							aB.MakeLine(j)
						}
						found = true
					}
				}
				if not found
				{
					aS2 := invTypes.funcWrapper
					if invTypes.fConstVal != null{
						aB := invTypes.fItem->{BuiltInGetVirtualParam^}
						aB.MakeLine(vTable.Size())
						vTable.Push(invTypes)
					}else{
						invTypes.fItem.VirtualId = vTable.Size()
						aB2 := aS2->{BuiltInVirtualCall^}
						aB2.MakeLine(vTable.Size())
						vTable.Push(invTypes)
					}
				}
			}

			for ThislessFuncs
			{
				if it.itFunc.IsVirtual
				{
					it.MakeLine(it.itFunc.VirtualId)
				}else{
					it.MakeLine(0)
				}
			}

		}
	}
	PrintStruct := virtual !(sfile f) -> void
	{
		if not vTable.Empty()
		{
			f << "%ClassTableType" << ClassId << " = type {"
			for it : vTable , i : 0
			{
				if i > 0 f << " , "
				if it.fConstVal == null
					f << it.fType.GetName() << "*"
				else
					f << it.fTyp.GetName()
			}
			f << " }\n"

			f << "@ClassTableItem" << ClassId << " = global %ClassTableType" << ClassId << " {"
			for it : vTable, i : 0
			{
				if i > 0 f << " , "
				if it.fConstVal != null
				{
					f << it.fTyp.GetName() << " " << it.fConstVal.GetName()
				}else{
					f << it.fType.GetName() << "* @" << it.fItem.OutputName
				}
				
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
			for it : Params, i : 0
			{
				if i != 0 or not vTable.Empty() f <<","
				f << it.ResultType.GetName()
			}
			f << "}\n"
			if DebugMode
			{
				f << "!" << ClassId << " = !DICompositeType(tag: DW_TAG_structure_type, name: \"" << ClassName << "\""
				f << ", elements: !{})\n"
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
			if asBase.GetType() is TypeClass
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
				if iterJ.Down.GetValue() == "!(){}"
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
		newItem.fConstVal = null
		newItem.funcWrapper = new BuiltInVirtualCall(fNam,fF,ClassId)

		vTypes.Push(newItem)
	}
	PutVirtualParam := virtual !(string fName,Object^ cnst) -> void
	{
		newItem := new FuncItem
		newItem.fName = fName
		newItem.fTyp = cnst.GetType()
		newItem.fConstVal = cnst
		newItem.fItem = new BuiltInGetVirtualParam(this&,cnst.GetType())

		vTypes.Push(newItem)
	}
	ApplyConstants := !(sfile f,Object^ itm) -> void
	{
		if itm.GetType() is TypePoint
		{
			ApplyConstants(f,itm.GetName())
		}else{
			ApplyConstants(f,itm.GetPointName())
		}
	}
	ApplyConstants := !(sfile f,string itm) -> void
	{
		if ContainVirtual
		{
			f << "call void(%Class" << ClassId << "*)@ClassExtraConstructor" << ClassId <<"(%Class" << ClassId << "* " 
			f << itm
			f << ")\n"
		}
	}
	GetVirtualParamFunc := !(string name) -> BoxFunc^
	{
		for it : vTypes
		{
			if it.fName == name and it.fConstVal != null
				return it.fItem
		}
		if Parent != null return Parent.GetVirtualParamFunc(name)
		return null
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
		
		builder := ""sbt
	
		builder + "%FuncTabel## = getelementptr %Class" + classId + " , %Class" + classId + "* #1, i32 0, i32 0\n" 
		builder + "%PreFunc## = load %ClassTableType" + classId + "* , %ClassTableType" + classId + "** %FuncTabel##\n"
		builder + "%FuncPtr## = getelementptr %ClassTableType" + classId + " , %ClassTableType" + classId + "* %PreFunc##, i32 0, i32 " + id + "\n"
		builder + "%Func## = load " + FuncTypeName + "* , " + FuncTypeName + "** %FuncPtr##\n" 
		if MyFuncType.RetType != GTypeVoid
			builder + "#0 = "
		builder + "call " + FuncTypeName +  "%Func##("
		for i : MyFuncType.ParsCount
		{
			if i > 0 builder + " , "
			if MyFuncType.ParsIsRef[i]
			{
				builder + MyFuncType.Pars[i].GetName() + "* "
			}else{
				builder + MyFuncType.Pars[i].GetName() + " "
			}
			builder << "#"  << (i + 1)
		}

		builder + ")\n"
		ToExe = builder.Str()
	}

}
BuiltInGetVirtualParam := class extend BuiltInFunc
{
	inClass := BoxClass^
	this := !(BoxClass^ frmClass,Type^ itType) -> void
	{
		inClass = frmClass

		itsBools := Queue.{bool}() itsBools.Push(true)
		newTypes := Queue.{Type^}()
		newTypes.Push(frmClass.ClassType)

		MyFuncType = GetFuncType(newTypes,itsBools.ToArray(),itType,true,false)

		IsRetRef = true
	}
	MakeLine := !(int j) -> void
	{
		tp := inClass.ClassType
		classId := inClass.ClassId

		ToExe = "%Pre## = getelementptr " + tp.GetName() + " , " + tp.GetName() + "* #1, i32 0,i32 0\n"
		ToExe = ToExe + "%Tabl## = load %ClassTableType" + classId + "* , %ClassTableType" + classId + "** %Pre##\n"
		ToExe = ToExe + "#0 = getelementptr %ClassTableType" + classId + " , %ClassTableType" + classId + "* %Tabl##, i32 0,i32 " + j + "\n"
	}
	//GetPriority := virtual !(FuncInputBox inBox) -> int
	//{
	//	if inBox.Pars.Size() != 1 return 255

	//	return 0		
	//}
	
}
BuiltInCheckVirtualType := class extend BoxTemplate
{
	classType := Type^
	this := !(Type^ clss) -> void
	{
		classType = clss
		FuncName = "is"
		OutputName = "error"
		typs := Queue.{Type^}()
		typs.Push(clss.GetPoint())
		MyFuncType = GetFuncType(typs,null->{bool^},GetType("bool"),false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int{
		return 0
	}
	GetNewFunc := virtual !(FuncInputBox itPars, TypeFunc^ funct) -> BoxFunc^
	{
		rightCl := itPars.itConsts[0]->{ObjType^}.MyType->{TypeClass^}.ToClass.ClassId
		asCl := classType->{TypeClass^}.ToClass
		classId := asCl.ClassId
		return new BuiltInFuncUno("is",classType.GetPoint(),false,GetType("bool"),false,
			"#0PrePre = getelementptr "sbt + classType.GetName() + " , " + classType.GetName() + "* #1 , i32 0, i32 0\n" +
			"#0Pre = load %ClassTableType" + classId + "* , %ClassTableType" + classId + "** #0PrePre\n" +
			"#0Left = bitcast %ClassTableType" + classId + "* #0Pre to i8*\n" +
			"#0Right = bitcast %ClassTableType" + rightCl + "* @ClassTableItem" + rightCl + " to i8*\n" +
			"#0 = icmp eq i8* #0Left,#0Right\n")
		
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

		IsRetRef = toFunc.IsRetRef

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
		//MakeLine()
	}
	MakeLine := !(int id) -> void
	{
		aseBase := MyFuncType->{Type^}
		FuncTypeName := aseBase.GetName()
		classId := itClass.ClassId

		isRetComp := MyFuncType.RetRef
		if not isRetComp
		{
			if MyFuncType.RetType != null
			{
				isRetComp = MyFuncType.RetType is TypeArr //TODO in ["class","fixarr"]
			}
		}
		outBuff := ""sbt
		if itFunc.IsVirtual
		{
			a1 := itInClass.vTable[id]
			a2 := a1.fType
			a3 := a2->{Type^}
			FuncTypeName2 := a3.GetName()
			outBuff + "%FuncTabel## = getelementptr %Class" + classId + " , %Class" + classId + "* %this, i32 0, i32 0\n" 
			outBuff + "%PreFunc## = load %ClassTableType" + classId + "* , %ClassTableType" + classId + "** %FuncTabel##\n"
			outBuff + "%FuncPtr## = getelementptr %ClassTableType" + classId + " , %ClassTableType" + classId + "* %PreFunc##, i32 0, i32 " + id + "\n"
			outBuff + "%Func## = load " + FuncTypeName2 + "* , " + FuncTypeName2 + "** %FuncPtr##\n" 

			outBuff + "%NewThis## = bitcast " + itClass.GetClassOutputName() + "* %this to " + itInClass.vTable[id].fType.Pars[0].GetName() + "*\n"
			//MyFuncType = itInClass.vTable[id].fType
			//printf("fuk %i %s %s %s\n",id,itInClass.vTable[id].fName,itFunc.FuncName,FuncTypeName2)
		}else{
			outBuff + "%NewThis## = bitcast " + itClass.GetClassOutputName() + "* %this to " + itInClass.GetClassOutputName() + "*\n"
		}

		//ToExe = ToExe + "%NewThis## = bitcast " + itClass.GetClassOutputName() + "* %this to " + itInClass.GetClassOutputName() + "*\n"
		if (MyFuncType.RetType != GTypeVoid and not isRetComp) or IsRetRef
			outBuff + "#0 = "
		fTypp := itFunc.MyFuncType
		fTypp2 := fTypp->{Type^}
		if itFunc.IsVirtual
		{
			asPre1 := itInClass.vTable[id].fType
			asPre2 := asPre1->{Type^}
			outBuff + "call " + asPre2.GetName()  + "%Func##("
			outBuff + itInClass.vTable[id].fType.Pars[0].GetName() + "* %NewThis##"
		}else{
			outBuff + "call " + fTypp2.GetName()  + "@" + OutputName + "("
			outBuff + itInClass.GetClassOutputName() + "* %NewThis##"
		}

		for i : MyFuncType.ParsCount
		{
			outBuff + " , "
			if MyFuncType.ParsIsRef[i]
			{
				outBuff + MyFuncType.Pars[i].GetName() + "* "
			}else{
				outBuff + MyFuncType.Pars[i].GetName() + " "
			}
			outBuff + "#" + (i + 1)
		}

		outBuff + ") #d\n"
		ToExe = outBuff.Str()
	}
}


