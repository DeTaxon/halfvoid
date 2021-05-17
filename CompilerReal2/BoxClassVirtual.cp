
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

//AppendClass BoxClass
//{
//}
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
