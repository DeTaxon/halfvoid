
VTableObject := class
{
	fName := string
	
	CmpItem := virtual !(VTableObject^ Ri) -> bool { return false}
	PrintGlobal := virtual !(TIOStream f) -> void {}
	PrintType := virtual !(TIOStream f) -> void {}
	PrintValue := virtual !(TIOStream f) -> void {}
}

VTableFunction := class extend VTableObject
{
	fType := TypeFunc^
	fItem := BoxFunc^

	funcWrapper := BuiltInVirtualCall^

	CmpItem := virtual !(VTableObject^ Ri) -> bool
	{	
		if not (Ri is VTableFunction)
			return false
		r := Ri->{VTableFunction^}

		if fName != r.fName return false
		if fType.RetType != r.fType.RetType return false
		if fType.ParsCount != r.fType.ParsCount return false
		if fType.IsVArgs != r.fType.IsVArgs return false

		for i : fType.ParsCount
		{
			if i == 0 continue
			if fType.Pars[i] != r.fType.Pars[i] return false
		}

		return true
	}
	PrintType := virtual !(TIOStream f) -> void 
	{
		f << fType.GetName() << "*"
	}
	PrintValue := virtual !(TIOStream f) -> void 
	{
		f << fType.GetName() << "* @" << fItem.OutputName
	}
}
VTableParameter := class extend VTableObject
{
	fItem := BuiltInGetVirtualParam^
	fConstVal := Object^
	fTyp := Type^

	funcWrapper := BuiltInGetVirtualParam^

	CmpItem := virtual !(VTableObject^ Ri) -> bool
	{	
		if not (Ri is VTableParameter)
			return false
		r := Ri->{VTableParameter^}

		if fName != r.fName return false
		return fTyp == r.fTyp
	}
	PrintType := virtual !(TIOStream f) -> void 
	{
		f << fTyp.GetName()
	}
	PrintValue := virtual !(TIOStream f) -> void 
	{
		f << fTyp.GetName() << " " << fConstVal.GetName()
	}
}

VTableTransform := class extend VTableObject
{
	ptrToClass := BoxClass^
	this := !(BoxClass^ ptr) -> void
	{
		ptrToClass = ptr
	}
	CmpItem := virtual !(VTableObject^ Ri) -> bool
	{
		return Ri is VTableTransform
	}
	PrintType := virtual !(TIOStream f) -> void 
	{
		f << GTypeVoidP.GetName()
	}
	classesCount := int
	PrintGlobal := virtual !(TIOStream f) -> void 
	{
		itr := ptrToClass
		while itr != null
		{
			if itr.vTable.Empty()
				break
			classesCount += 1
			itr = itr.Parent
		}

		arrName := ("["sbt + (classesCount*2 + 1) + " x i64]")->{char^}
		f << "@TableTransform" << ptrToClass.ClassId << " = global " << arrName << " ["
		f << "i64 " << classesCount << "\n"

		itr = ptrToClass
		for i : classesCount
		{
			f << ",i64 ptrtoint ("
			itr.PrintVTableTypeName(f)
			f << "* "
			itr.PrintVTableObject(f)
			f <<" to i64) , i64 0\n"
			itr = itr.Parent
		}
		f << "]\n"
	}
	PrintValue := virtual !(TIOStream f) -> void 
	{
		f << GTypeVoidP.GetName() << " bitcast ( [" << (classesCount*2 + 1) << " x i64]* @TableTransform"<< ptrToClass.ClassId <<" to i8*)"
	}
}

AppendClass BoxClass
{
	vTypes := List.{VTableObject^}
	vTable := List.{VTableObject^}

	computedVTable := bool
	PrintVTable := !(TIOStream f) -> void
	{
		if vTable.Empty()
			return void

		vTable[^].PrintGlobal(f)

		PrintVTableTypeName(f)
		f << " = type {"
		for it,i : vTable 
		{
			if i > 0 f << " , "
			it.PrintType(f)
		}
		f << " }\n"

		PrintVTableObject(f)
		f <<  " = global "
		PrintVTableTypeName(f)
		f << " {"
		for it,i : vTable
		{
			if i > 0 f << " , "
			it.PrintValue(f)
			
		}
		f << "}\n"
	}
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

			if vTable.Size() == 0
			{
				if vTypes.Size() != 0
				{
					vTable.Push(new VTableTransform(this&))
				}
			}else{
				assert(vTable[0] is VTableTransform)
				vTable[0] = new VTableTransform(this&)
			}

			for invTypes : vTypes// i : 0
			{
				found := false
				for invTable, j : vTable
				{
					if invTable.CmpItem(invTypes)
					{
						invTable = invTypes
						if invTypes is VTableParameter
						{
							invTypes->{VTableParameter^}.fItem.MakeLine(j)
						}
						if invTable is VTableFunction
						{
							fnc := invTypes->{VTableFunction^}
							
							fnc.fItem.VirtualId = j
							fnc.funcWrapper.MakeLine(j) 
						}
						found = true
					}
				}
				if not found
				{
					if invTypes is VTableParameter
					{
						invTypes->{VTableParameter^}.fItem.MakeLine(vTable.Size())
						vTable.Push(invTypes)
					}
					if invTypes is VTableFunction
					{
						fnc := invTypes->{VTableFunction^}
						
						fnc.fItem.VirtualId = vTable.Size()
						fnc.funcWrapper.MakeLine(vTable.Size()) 
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
	PutVirtualFunc := virtual !(string fNam,TypeFunc^ fTyo,BoxFunc^ fF) -> void
	{
		newItem := new VTableFunction
		newItem.fName = fNam
		newItem.fType = fTyo
		newItem.fItem = fF
		newItem.funcWrapper = new BuiltInVirtualCall(fNam,fF,ClassId)

		vTypes.Push(newItem)
	}
	PutVirtualParam := virtual !(string fName,Object^ cnst) -> void
	{
		newItem := new VTableParameter
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
			if it.fName == name and it is VTableParameter
				return it->{VTableParameter^}.fItem
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
