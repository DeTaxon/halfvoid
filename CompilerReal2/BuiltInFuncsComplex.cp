BuiltInTemplateZeroMem := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "ZeroMem"

		PP := Queue.{Type^}() ; $temp
		PP.Push(null)

		IsRefs := bool[1]
		IsRefs[0] = true
		MyFuncType = GetFuncType(PP,IsRefs,GTypeVoid,false,false)
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		typName := itBox.itPars[0].first.GetName()
		resStr := ""sbt
		resStr << "%ptr## = getelementptr " << typName << " , " << typName << "* null,i32 1\n"
		resStr << "%tSize## = ptrtoint "<< typName << "* %ptr## to i64\n"
		resStr << "%toPtr## = bitcast " << typName << "* #1 to i8*\n"
		resStr << "call void @internalGCMemClear(i8* %toPtr##, i64 %tSize##) #d\n"
		if itBox.itPars[0].first is TypeClass
		{
			clId := itBox.itPars[0].first->{TypeClass^}.ToClass.ClassId
			resStr << "call void @ClassExtraConstructor" << clId << "(" << typName << "* #1)\n"
		}
		return new BuiltInFuncUno("ZeroMem",itBox.itPars[0].first,true,GTypeVoid,false,resStr)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		if itBox.itPars.Size() != 1
			return 255
		if not itBox.itPars[0].second
			return 255
		return 0
	}
}
