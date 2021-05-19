//AppendClass BoxClass
//{
//}

BuiltInThislessTemplate := class extend BoxTemplate
{
	itTemplate := BoxTemplate^
	itClass := BoxClass^
	itInClass := BoxClass^
	createdFuncs := Queue.{BuiltIn2ThislessFunc^}

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

		preRes := new BuiltIn2ThislessFunc(newFunc,itClass,itInClass)
		createdFuncs.Push(preRes)
		return preRes
	}
	MakeLine := !() -> void
	{
		createdFuncs[^].MakeLine(0)
	}
}
//BuiltIn2ThislessFunc := class extend BuildIn2Func
//{
//	itFunc := BoxFunc^
//	itClass := BoxClass^
//	itInClass := BoxClass^
//	this := !(BoxFunc^ toFunc,BoxClass^ toClass,BoxClass^ inClass) -> void
//	{
//		toFunc.ParseBlock()
//		itFunc = toFunc
//		itClass = toClass
//		itInClass = inClass
//		FuncName = toFunc.FuncName
//		OutputName = toFunc.OutputName
//
//		IsRetRef = toFunc.IsRetRef
//
//		newTypes := Queue.{Type^}() ; $temp
//		itsBools := Queue.{bool}() ; $temp
//
//		fTyp := toFunc.MyFuncType
//
//		i := 1
//		while i < fTyp.ParsCount
//		{
//			if fTyp.ParsIsRef == null{
//				itsBools.Push(false)
//			}else{
//				itsBools.Push(fTyp.ParsIsRef[i])
//			}
//			newTypes.Push(fTyp.Pars[i])
//			i += 1
//		}
//		MyFuncType = GetFuncType(newTypes,itsBools.ToArray(),fTyp.RetType,fTyp.RetRef,fTyp.IsVArgs)
//		//MakeLine()
//	}
//}
BuiltIn2ThislessFunc := class extend BuiltIn2Func
{
	itFunc := BoxFunc^
	itClass := BoxClass^
	itInClass := BoxClass^
	id := int
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
		//MakeLine()
	}
	MakeLine := !(int inpId) -> void
	{
		id = inpId
	}
	
	PrintFunc := virtual !(BuiltIn2Call^ trg,TIOStream f) -> void 
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

		callId := trg.GenId()

		debStr := ""
		debId := -1
		if DebugMode
		{
			debId = CreateDebugCall(trg)
			if debId != -1
			{
				debStr = (", !dbg !"sbt + debId)->{char^}
			}
		}

		if itFunc.IsVirtual
		{
			a1 := itInClass.vTable[id]
			a2 := a1.fType
			a3 := a2->{Type^}
			FuncTypeName2 := a3.GetName()
			f << "%FuncTabel"<<callId<<" = getelementptr %Class" << classId << " , %Class" << classId << "* %this, i32 0, i32 0 " << debStr << "\n"
			f << "%PreFunc" << callId << " = load %ClassTableType" << classId << "* , %ClassTableType" << classId << "** %FuncTabel" << callId << debStr <<" \n"
			f << "%FuncPtr" << callId << " = getelementptr %ClassTableType" << classId << " , %ClassTableType" << classId << "* %PreFunc" << callId << ", i32 0, i32 " << id <<debStr << " \n"
			f << "%Func" << callId << " = load " << FuncTypeName2 << "* , " << FuncTypeName2 << "** %FuncPtr" << callId << debStr << "\n"

			f << "%NewThis" << callId << " = bitcast " << itClass.GetClassOutputName() << "* %this to " << itInClass.vTable[id].fType.Pars[0].GetName() << "* " << debStr << "\n"
		}else{
			f << "%NewThis" << callId << " = bitcast " << itClass.GetClassOutputName() << "* %this to " << itInClass.GetClassOutputName() << "* " << debStr << "\n"
		}

		
		itr := trg.Down
		for i : MyFuncType.ParsCount
		{
			if MyFuncType.ParsIsRef[i]
			{
				itr.PrintPointPre(f)
			}else{
				itr.PrintPre(f)
			}
			itr = itr.Right
		}

		//ToExe = ToExe + "%NewThis## = bitcast " + itClass.GetClassOutputName() + "* %this to " + itInClass.GetClassOutputName() + "*\n"
		if (MyFuncType.RetType != GTypeVoid and not isRetComp) or IsRetRef
		{
			trg.PrintRes(f)
			f << " = "
		}
		fTypp := itFunc.MyFuncType
		fTypp2 := fTypp->{Type^}
		if itFunc.IsVirtual
		{
			asPre1 := itInClass.vTable[id].fType
			asPre2 := asPre1->{Type^}
			f << "call " << asPre2.GetName()  << "%Func" << callId << "("
			f << itInClass.vTable[id].fType.Pars[0].GetName() << "* %NewThis" << callId << ""
		}else{
			f <<  "call " << fTypp2.GetName()  << "@" << OutputName << "("
			f <<  itInClass.GetClassOutputName() << "* %NewThis" << callId << ""
		}

		itr = trg.Down
		for i : MyFuncType.ParsCount
		{
			f << ","
			if MyFuncType.ParsIsRef[i]
			{
				itr.PrintPointUse(f)
			}else{
				itr.PrintUse(f)
			}
			itr = itr.Right
		}

		f << ")" << debStr  << "\n"
	}
}

