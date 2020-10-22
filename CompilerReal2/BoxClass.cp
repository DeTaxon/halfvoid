ParseClass := !(Object^ ob)-> BoxClass^
{
	
	putPack := false
	if ob == null return null

	iterT := ob.Down
	if iterT == null return null

	if iterT.GetValue() == "packed_class"
	{
		putPack = true
	}else{
		if iterT.GetValue() != "class" return null
	}


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
		preRes :=  new BoxClass(iterT,null->{BoxClass^},ExtObj)
		preRes.IsPacked = putPack
		return preRes

	}

	if iterT.GetValue() != "{}" return null

	preRes2 := new BoxClass(iterT,null->{BoxClass^},null->{Object^})
	preRes2.IsPacked = putPack
	return preRes2
}

gAppendClass := AVLMap.{char^,Object^}

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
	ItClasses := AVLMap.{int,Stack.{BoxClass^}}
	ClassTree := Object^
	ConstTree := Object^
	ExtendTree := Object^

	TempConsts := Queue.{ObjConstHolder^}^

	relAttrs := QueueSet.{string}

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

		SyntaxCompress(ConstTree,PriorityData)
		MakeGoodConsts(ConstTree)

		for ite : ClassTree.Down.Right.Down
		{
			if ite.GetValue() == "~ind"
			{
				asInd := ite->{ObjIndent^}
				if asInd.MyStr[0] == '$'
					relAttrs.Push(asInd.MyStr)
			}
		}
	}
	GetItem := virtual !(string name) -> Object^
	{
		if TempConsts != null
		{
			if TempConsts^[^].ItName == name 
				return it.Down
		}
		return null
	}
	GetClass := !(FuncInputBox itBox) -> TypeClass^
	{
		itHash := itBox.GetConstsHash()
		if ItClasses[itHash][^].IsSameConsts(itBox)
			return it.ClassType

		newConsts := Queue.{ObjConstHolder^}()
		if not IsEqConsts(ConstTree,itBox,newConsts)
		{
			return null
		}

		newTree := ClassTree.Clone()
		newTree.Up = this&
		
		inher := BoxClass^()

		treeIter := newTree.Down.Right

		if ExtendTree != null
		{
			TempConsts = newConsts&
			inher = ParseType(ExtendTree,itBox.itAttrs&)->{BoxClass^}
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
	

		newClass.ItConsts.Push(itBox.itConsts[^])
		newClass.ItVals.Push(newConsts[^])
		
		templs := Queue.{QueueSet.{string}^}() ; $temp
		templs.Push(this.relAttrs&)

		clIter := inher
		while clIter != null
		{
			if clIter.Up?.GetValue() == "!{}{...}"
			{
				asTmpl := clIter.Up->{BoxClassTemplate^}
				templs.Push(asTmpl.relAttrs&)
			}
			clIter = clIter.Parent
		}
		
		
		for itr,itrK : itBox.itAttrs
		{
			for templs
			{
				if it.Contain(itrK)
				{
					newClass.ItAttrs[StrCopy(itrK)] = itr
				}
			}
		}

		ItClasses[itHash].Push(newClass)
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
	ItTemplates := Queue.{BoxTemplate^}

	ClassType := TypeClass^
	UnrollTemplate := BuiltInTemplateUnroll^
	AutoFieldTemplate := BuiltInTemplateAutoField^
	VirtualCheck := BuiltInCheckVirtualType^

	ExtendObject := Object^
	Parent := BoxClass^

	ContainVirtual  := bool

	IsPacked := bool

	ItVals := Queue.{ObjConstHolder^}
	ItConsts := Queue.{Object^}
	ItAttrs := AVLMap.{string,Object^}

	ClassName := string

	GetClassOutputName := !() -> string
	{
		return StrCopy("%Class"sbt + ClassId)
	}

	
	metaFields := AVLMap.{char^,MetaFieldBox^}
	AddMetaField := !(char^ name, MetaFieldBox^ itMBox) -> void
	{
		metaFields[StrCopy(name)] = itMBox
	}

	ThislessFuncs := Queue.{BuiltInThislessFunc^}
	ThislessTemplates := Queue.{BuiltInThislessTemplate^}
	GetWrappedFunc := !(char^ name ,Queue.{BoxFunc^} funcs, Queue.{BoxTemplate^} templs) -> void
	{
		gotFuncs := Queue.{BoxFunc^}()
		funcsCl := Queue.{BoxClass^}()

		gotTempls := Queue.{Pair.{BoxTemplate^,BoxClass^}}() ; $temp

		iterF := this&
		while iterF != null
		{
			for qIter : iterF.ItMethods 
			{
				if qIter.FuncName == name
				{
					gotFuncs.Push(qIter) ; $temp
					funcsCl.Push(iterF) ; $temp
				}
			}
			for tmps : iterF.ItTemplates
			{
				if tmps.FuncName == name
				{
					gotTempls.Emplace(tmps,iterF)
				}
			}
			iterF = iterF.Parent
		}

		for gotFuncsIter : gotFuncs , funcsClIter : funcsCl
		{
			inTh := BoxFunc^()

			if ThislessFuncs[^].itFunc == gotFuncsIter
			{
				inTh = it
				break
			}


			if inTh == null
			{
				toCr := gotFuncsIter
				if not toCr.IsStatic {
					inThPre :=  new BuiltInThislessFunc(toCr,this&->{BoxClass^},funcsClIter)
					ThislessFuncs.Push(inThPre)
					inTh = inThPre
				}
			}
			if inTh != null funcs.Push(inTh) ; $temp
		}
		for templ : gotTempls
		{
			inTh := BoxTemplate^()

			if ThislessTemplates[^].itTemplate == templ.first
			{
				inTh = it
				break
			}
			if inTh == null
			{
				if not templ.first.IsStatic{
					inThPre := new BuiltInThislessTemplate(templ.first,this&,templ.second)
					ThislessTemplates.Push(inThPre)
					inTh = inThPre
					inTh.FuncName = name
				}
			}
			if inTh != null templs.Push(inTh)
		}

	}

	IsSameConsts := !(FuncInputBox itBox) -> bool
	{
		if itBox.itConsts.Size() != ItConsts.Size() {
			return false
		}

		for c,i : itBox.itConsts
		{
			if not CmpConstObjs(c,ItConsts[i])
				return false
		}
		if Up != null and Up.GetValue() == "!{}{...}"
		{
			asTmp := Up->{BoxClassTemplate^}
			for itr : asTmp.relAttrs
			{
				inItMap := ItAttrs.TryFind(itr)
				inOtMap := itBox.itAttrs.TryFind(itr)
				if inItMap == null and inOtMap != null return false
				if inItMap != null and inOtMap == null return false
			}
		}

		return true
	}

	GetItem := virtual !(string name) -> Object^
	{
		if ItVals[^].ItName == name
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
		}
		WorkBag.Push(this&,State_PreGetUse)

		ClassId = GetNewId()
		ClassType = new TypeClass(this&)
		ClassType.metaId = ClassId
		UnrollTemplate = new BuiltInTemplateUnroll(this&)
		AutoFieldTemplate = new BuiltInTemplateAutoField(this&)
		VirtualCheck = new BuiltInCheckVirtualType(ClassType)

		this.Parent = par

		if par?.ContainVirtual ContainVirtual = true
		if Down != null and not ContainVirtual
		{
			if Down.Down[^].GetValue() == "virtual" 
			{
				ContainVirtual = true
				break
			}
		}
		WorkBag.Push(this&,State_Start)
		WorkBag.Push(this&,State_CheckBaseClass)
		WorkBag.Push(this&,State_PrePrint)
		Classes.Push(this&)

	}
	
	Yodlers := AVLSet.{BoxFuncBody^}
	
	GetFieldParam := !(char^ name) -> MemParam^
	{
		if Params[^].ItName == name
			return it
		if FakeParams[^].ItName == name
			return it
		if Parent != null
			return Parent.GetFieldParam(name)
		return null
	}
	
	Inherited := false
	InheritParams := !() -> void
	{
		if not Inherited
		{
			Inherited = true
			if Parent != null
			{
				Parent.InheritParams()
				//InheritParams2()
				if Parent.ContainVirtual 
					ContainVirtual = true
				WorkBag.Push(this&,State_PrePrint)

			}
		}
	}
	Inherited2 := false	
	InheritParams2 := !() -> void
	{
		if Inherited2 return void
		Inherited2 = true
		if Parent != null
		{

			Parent.InheritParams2()
			Size := Parent.Params.Size()
			NotMineParams = Size
			for i : Size
			{
				Params.PushFront(Parent.Params[Size - i - 1])
			}
			for fakes : Parent.FakeParams FakeParams.PushFront(fakes)
		}
		for Yodlers
		{
			extrField := it.ABox.GetAsType()
			it.yieldInClass = Params.Size()
			it.yieldInClassType = extrField
			if ContainVirtual
				it.yieldInClass += 1
			new FieldParam("yeldData"sbt + GetNewId(), extrField,this&)
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			if ClassName != "anon"
				return void

			itr := Up
			while itr != null and itr.GetValue() != "i:=1" itr = itr.Up
			if itr != null{
				asN := itr->{ObjParam^}
				ClassName = asN.MyStr

				if gAppendClass.Contain(ClassName)
				{
					newTree := gAppendClass[ClassName].Clone()
					if Down == null or Down.Down == null
					{
						Down = newTree.Down
						Down.SetUp(this&)
					}else{
						after := Down.Down
						while after.Right != null
							after = after.Right
						after.Right = newTree.Down
						after.Right.Left = after
						after.Right.SetUp(after.Up)
					}
				}
			}
			if Down != null and itr != null
			{
				MakeItBlock(Down)
				Down.SetUp(this&)
				WorkBag.Push(Down,State_Start)
			}
		}
		if pri == State_CheckBaseClass
		{
			if ExtendObject != null{
				newType := ParseType(ExtendObject,ItAttrs&)
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
			foundNotThis := false
		}
		if pri == State_PrePrint
		{
			InheritParams2()
			ComputeVTable()		
		}
	}
	GetValue := virtual !() -> string
	{
		return "{...}"
	}
	GetFunc := virtual !(string name,FuncInputBox itBox,bool iVir) -> BoxFunc^
	{
		Funcs := Queue.{BoxFunc^}() ; $temp
		Templs := Queue.{BoxTemplate^}() ; $temp

		if not iVir
		{
			if vTypes[^].fName == name
			{
				Funcs.Push(it.funcWrapper) 
			}
		}
		maybeForced := Find(name,'.')
		if maybeForced != -1
		{
			clName := StringSpan(name,maybeForced)
			if clName == ClassName
			{
				funcName := name[(maybeForced+1) .. 0].Str()
				if vTypes[^].fName == funcName
				{
					Funcs.Push(it.fItem)
				}
				for ItMethods
				{
					if it.FuncName == funcName and it.IsSameConsts(itBox) and not it.IsVirtual
					{
						Funcs.Push(it)
					}
				}
				for ItTemplates
				{
					if it.FuncName == funcName
					{
						Templs.Push(it)
					}
				}
			}
		}

		downIter := Down
		if itBox.itMetaPtr != null
		{
			downIter = itBox.itMetaPtr
		}
		if downIter != null
		{
			for iterJ : downIter.Down
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
				for invTable, j : vTable
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

			if ThislessFuncs[^].itFunc.IsVirtual
			{
				it.MakeLine(it.itFunc.VirtualId)
			}else{
				it.MakeLine(0)
			}
			ThislessTemplates[^].MakeLine()

		}
	}
	PrintStruct := virtual !(sfile f) -> void
	{
		if not vTable.Empty()
		{
			f << "%ClassTableType" << ClassId << " = type {"
			for it,i : vTable 
			{
				if i > 0 f << " , "
				if it.fConstVal == null
					f << it.fType.GetName() << "*"
				else
					f << it.fTyp.GetName()
			}
			f << " }\n"

			f << "@ClassTableItem" << ClassId << " = global %ClassTableType" << ClassId << " {"
			for it,i : vTable
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
			if DebugMode
			{
				f << "!" << ClassId << " = !DICompositeType(tag: DW_TAG_structure_type, name: \"" << ClassName << "\""
				f << ", elements: !{})\n"
			}
		}else{
			f << "%Class" << ClassId << " = type "
			if IsPacked f << "<"
			f << "{"
			if not vTable.Empty()
			{
				f << "%ClassTableType" << ClassId << "*"
			}
			for it, i : Params
			{
				if i != 0 or not vTable.Empty() f <<","
				f << it.ResultType.GetName()
			}
			f << "}"
			if IsPacked f << ">"
			f <<"\n"
			if DebugMode
			{
				ClassType.ComputeAlignAndDeb()
				exZ := List.{int}() ; $temp
				for cc : ClassType.debTable
				{
					newII := GetNewId()
					f << "!" << newII << " = !DIDerivedType(tag: DW_TAG_member, name: \"" << cc.0 << "\" , scope: !" << ClassId
					if cc.1.Line != null
					{
						itrr := Up
						while itrr.Up != null itrr = itrr.Up
						f << " , file: !" << itrr->{BoxFile^}.fileId << " , line: " << cc.1.Line.LinePos 
					}
					f << " , baseType: !" << cc.1.ResultType.metaId << ", size: " << cc.3*8 << ", offset: " << cc.2*8 << ")\n"
					//!16 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !14, file: !1, line: 5, baseType: !12, size: 32)
					exZ << newII
				}
				elmIds := GetNewId()
				f << "!" << elmIds << " = !{"
				for ccc,i : exZ
				{
					if i != 0 f << ","
					f << "!" << ccc
				}
				f << "}\n"
				f << "!" << ClassId << " = !DICompositeType(tag: DW_TAG_structure_type, name: \"" << ClassName << "\""
				f << ", elements: !"<< elmIds << ")\n"
			}
		}
	}

	tuplCmp := TupleSpaceship^
	CreateTupleCmp := !() -> void
	{
		if tuplCmp == null
			tuplCmp = new TupleSpaceship(this&)
	}

	PrintCreatedFuncs := !(sfile f) -> void
	{
		if tuplCmp != null
			tuplCmp.PrintAsGlobal(f)
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

		for itParm,i : this.Params
		{
			if not TypeContainVTable(itParm.ResultType)
				continue

			if itParm.ResultType is TypeArr
			{
				lays := Queue.{int}() ; $temp
				typeT := itParm.ResultType->{TypeArr^}
				itBaseT := TypeClass^()
				while true
				{
					lays.Push(typeT.Size)

					if typeT.Base is TypeArr {
						typeT = typeT.Base->{TypeArr^}
					}else{
						assert(typeT.Base is TypeClass)
						itBaseT = typeT.Base->{TypeClass^}
						break
					}
				}
				inCl := i
				if ContainVirtual 
					inCl += 1
				for itL,k : lays
				{
					
					f << "br label %L" << i <<"itr" << k << "PreStart\n"
					f << "L" << i << "itr" << k << "PreStart:\n"
					f << "br label %L" << i <<"itr" << k << "Start\n"
					f << "L" << i << "itr" << k << "Start:\n"
					f << "%L" << i << "itr" << k << "Itr = phi i32 [0," 
						<< "%L" << i << "itr" << k << "PreStart] ,"
						<< "[%L" << i << "itr" << k << "Itr2, "
						<< "%L" << i << "itr" << k << "PreEnd]\n"

				}
				f << "%Pre" << i << "Set = getelementptr %Class" << ClassId << ", %Class" << ClassId << "* %this"
					 << ", i32 0, i32 " << inCl
				for itL, k : lays
				{
					f << ",i32 %L" << i << "itr" << k << "Itr"
				}
				f << "\n"
				itBaseT.ToClass.ApplyConstants(f,"%Pre"sbt + i + "Set")

				k := lays.Size() - 1
				while k >= 0
				{	itL := lays[k]
					f << "br label %L" << i << "itr" << k << "PreEnd\n"
					f << "L" << i << "itr" << k << "PreEnd:\n"
					f << "%L" << i << "itr" << k << "Itr2 = add i32 1, %L" << i << "itr" << k << "Itr\n"
					f << "%L" << i << "itr" << k << "Test = icmp eq i32 %L" << i << "itr" << k << "Itr2 , "<< itL << "\n"
					f << "br i1 " << "%L" << i << "itr" << k << "Test ,"
						<<"label %L" << i << "itr" << k << "End ,"
						<<"label %L" << i << "itr" << k << "Start\n"
					f << "L" << i << "itr" << k << "End:\n"

					k -= 1
				}
			}
			if itParm.ResultType is TypeClass
			{
				clTyp := itParm.ResultType->{TypeClass^}
				classItm := clTyp.ToClass
				if classItm.ContainVirtual
				{
					pos := i
					if this.ContainVirtual pos += 1
					f << "%itrItm" << pos << " = getelementptr %Class" << ClassId << " , %Class" << ClassId
					f << "* %this" <<", i32 0, i32 " << pos <<"\n"
					classItm.ApplyConstants(f,"%itrItm"sbt + pos)
				}
			}

		}
		f << "ret void\n}\n"

		downs := new List.{Object^}() ; $temp
		downs.Push(Down)
		downs.Push(metaFields[^].Down)

		for iterJ : downs^[^].Down
		{
			if iterJ.GetValue() == "i:=1"
			{
				if iterJ.Down.GetValue() == "!()"
				{
					asFunc := iterJ.Down->{BoxFunc^}
					if not asFunc.IsAssembler()
						iterJ.PrintGlobal(f)
				}
				if iterJ.Down.GetValue() == "!(){}"
				{
					iterJ.PrintGlobal(f)
				}
			}
		}
		PrintCreatedFuncs(f)
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
		if TypeContainVTable(ClassType)
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
	PreAskField := virtual !(string name) -> void
	{
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
	
		builder + "%FuncTabel## = getelementptr %Class" + classId + " , %Class" + classId + "* #1, i32 0, i32 0 #d\n" 
		builder + "%PreFunc## = load %ClassTableType" + classId + "* , %ClassTableType" + classId + "** %FuncTabel## #d\n"
		builder + "%FuncPtr## = getelementptr %ClassTableType" + classId + " , %ClassTableType" + classId + "* %PreFunc##, i32 0, i32 " + id + " #d\n"
		builder + "%Func## = load " + FuncTypeName + "* , " + FuncTypeName + "** %FuncPtr## #d\n" 
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

		builder + ") #d\n"
		ToExe = builder.Str()
	}

}
BuiltInGetVirtualParam := class extend BuiltInFunc
{
	inClass := BoxClass^
	this := !(BoxClass^ frmClass,Type^ itType) -> void
	{
		inClass = frmClass

		itsBools := Queue.{bool}() itsBools.Push(true) ; $temp
		newTypes := Queue.{Type^}()
		newTypes.Push(frmClass.ClassType) ; $temp

		MyFuncType = GetFuncType(newTypes,itsBools.ToArray(),itType,true,false)

		IsRetRef = true
	}
	MakeLine := !(int j) -> void
	{
		tp := inClass.ClassType
		classId := inClass.ClassId

		preRes := "%Pre## = getelementptr "sbt << tp.GetName() << " , " << tp.GetName() << "* #1, i32 0,i32 0 #d\n"
		preRes << "%Tabl## = load %ClassTableType" << classId << "* , %ClassTableType" << classId << "** %Pre## #d\n"
		preRes <<  "#0 = getelementptr %ClassTableType" << classId << " , %ClassTableType" << classId << "* %Tabl##, i32 0,i32 " << j << " #d\n"
		ToExe = preRes.Str()
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
		typs := Queue.{Type^}() ; $temp
		typs.Push(clss.GetPoint())
		MyFuncType = GetFuncType(typs,null->{bool^},GTypeBool,false,false)
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
			"#0PrePre = getelementptr "sbt + classType.GetName() + " , " + classType.GetName() + "* #1 , i32 0, i32 0 #d\n" +
			"#0Pre = load %ClassTableType" + classId + "* , %ClassTableType" + classId + "** #0PrePre #d\n" +
			"#0Left = bitcast %ClassTableType" + classId + "* #0Pre to i8* #d\n" +
			"#0Right = bitcast %ClassTableType" + rightCl + "* @ClassTableItem" + rightCl + " to i8* #d\n" +
			"#0 = icmp eq i8* #0Left,#0Right #d\n")
		
	}
}

BuiltInThislessTemplate := class extend BoxTemplate
{
	itTemplate := BoxTemplate^
	itClass := BoxClass^
	itInClass := BoxClass^
	createdFuncs := Queue.{BuiltInThislessFunc^}

	this := !(BoxTemplate^ templ,BoxClass^ toClass, BoxClass^ inClass) -> void
	{
		itTemplate = templ
		itClass = toClass
		itInClass = inClass

		fT := templ.MyFuncType

		newPars := Queue.{Type^}() ; $temp
		newBools := bool[]
		newBools = null
		if fT.ParsCount >= 2
			newBools = new bool[fT.ParsCount - 1]

		for i : fT.ParsCount
		{
			if i == 0 continue

			newPars.Push(fT.Pars[i])
			newBools[i-1] = fT.ParsIsRef[i]
		}
		MyFuncType = GetFuncType(newPars,newBools,fT.RetType,fT.RetRef,fT.IsVArgs)

	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		newBox := new FuncInputBox ; $temp

		newBox.itPars.Emplace(itInClass.ClassType,true)
		for it,i : itBox.itPars
		{	
			newBox.itPars.Emplace(it.first,it.second)
		}
		newBox.itConsts.Push(itBox.itConsts[^])
		newBox.itAttrs[ind] = itBox.itAttrs[^ind]
		return itTemplate.GetPriority(newBox^)
	}
	GetNewFunc := virtual !(FuncInputBox itBox,TypeFunc^ funType) -> BoxFunc^
	{
		newBox := new FuncInputBox ; $temp

		newBox.itPars.Emplace(itClass.ClassType,true)
		for it,i : itBox.itPars
		{	
			newBox.itPars.Emplace(it.first,it.second)
		}
		newBox.itConsts.Push(itBox.itConsts[^])
		newBox.itAttrs[ind] =itBox.itAttrs[^ind]
		newFunc := itTemplate.GetFunc(newBox^)

		preRes := new BuiltInThislessFunc(newFunc,itClass,itInClass)
		createdFuncs.Push(preRes)
		return preRes
	}
	MakeLine := !() -> void
	{
		createdFuncs[^].MakeLine(0)
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

		newTypes := Queue.{Type^}() ; $temp
		itsBools := Queue.{bool}() ; $temp

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
			outBuff + "%FuncTabel## = getelementptr %Class" + classId + " , %Class" + classId + "* %this, i32 0, i32 0 #d\n" 
			outBuff + "%PreFunc## = load %ClassTableType" + classId + "* , %ClassTableType" + classId + "** %FuncTabel## #d\n"
			outBuff + "%FuncPtr## = getelementptr %ClassTableType" + classId + " , %ClassTableType" + classId + "* %PreFunc##, i32 0, i32 " + id + " #d\n"
			outBuff + "%Func## = load " + FuncTypeName2 + "* , " + FuncTypeName2 + "** %FuncPtr## #d\n" 

			outBuff + "%NewThis## = bitcast " + itClass.GetClassOutputName() + "* %this to " + itInClass.vTable[id].fType.Pars[0].GetName() + "* #d\n"
			//MyFuncType = itInClass.vTable[id].fType
			//printf("fuk %i %s %s %s\n",id,itInClass.vTable[id].fName,itFunc.FuncName,FuncTypeName2)
		}else{
			outBuff + "%NewThis## = bitcast " + itClass.GetClassOutputName() + "* %this to " + itInClass.GetClassOutputName() + "* #d\n"
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


