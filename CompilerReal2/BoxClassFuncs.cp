//AppendClass BoxClass
//{
//}

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
		newBox.itConsts.Push(itBox.itConsts[^]) ; $temp
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
		newBox.itConsts.Push(itBox.itConsts[^]) ; $temp
		newBox.itAttrs[ind] =itBox.itAttrs[^ind] ; $temp
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
				isRetComp = IsComplexType(MyFuncType.RetType)
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
		ToExe = StrCopy(ToExe) //TODO BUG: called with $temp
	}
}

