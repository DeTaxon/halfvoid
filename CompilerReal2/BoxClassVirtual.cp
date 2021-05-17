
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
				if i == 0 continue
				if fType.Pars[i] != Ri.fType.Pars[i] return false
			}
		}

		return true
	}
}

AppendClass BoxClass
{
	vTypes := Queue.{FuncItem^}
	vTable := Queue.{FuncItem^}

	computedVTable := bool
	ComputeVTable := !() -> void
	{	
		if not computedVTable
		{
			computedVTable = true

			tableOffset := 0

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
							aB.MakeLine(j + tableOffset)
						}else{
							invTypes.fItem.VirtualId = j
							aS := invTypes.funcWrapper
							aB := aS->{BuiltInVirtualCall^}
							aB.MakeLine(j + tableOffset)
						}
						found = true
					}
				}
				if not found
				{
					aS2 := invTypes.funcWrapper
					if invTypes.fConstVal != null{
						aB := invTypes.fItem->{BuiltInGetVirtualParam^}
						aB.MakeLine(vTable.Size() + tableOffset)
						vTable.Push(invTypes)
					}else{
						invTypes.fItem.VirtualId = vTable.Size() + tableOffset
						aB2 := aS2->{BuiltInVirtualCall^}
						aB2.MakeLine(vTable.Size() + tableOffset)
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
	

	GetVTableTypeName := !() -> char^ { return "%ClassTableType"sbt + ClassId }
	GetVTableName := !() -> char^ { return "@ClassTableItem"sbt + ClassId }
	PrintVTableTypeName := !(TIOStream f) -> void
	{
		f << "%ClassTableType" << ClassId
	}
	PrintVTableObject := !(TIOStream f) -> void
	{
		f << "@ClassTableItem" << ClassId
	}
	PrintVTable := !(TIOStream f) -> void
	{
		if vTable.Empty()
			return void

		PrintVTableTypeName(f)
		f << " = type {"
		for it,i : vTable 
		{
			if i > 0 f << " , "
			if it.fConstVal == null
				f << it.fType.GetName() << "*"
			else
				f << it.fTyp.GetName()
		}
		f << " }\n"

		PrintVTableObject(f)
		f <<  " = global "
		PrintVTableTypeName(f)
		f << " {"
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
