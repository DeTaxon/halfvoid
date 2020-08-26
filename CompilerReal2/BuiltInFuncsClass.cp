BuiltInUnrollClass := class extend BuiltInFunc
{
	ToClass := BoxClass^
	ParName := char^

	this := !(string Name, BoxClass^ l, bool lRef,Type^ retV, bool RRetRef) -> void
	{
		ParName = Name
		ToClass = l

		FuncName = Name
		OutputName = Name
		IsRetRef = RRetRef

		PP := Queue.{Type^}() ; $temp
		PP.Push(l.ClassType)

		IsRefs := bool[1]
		IsRefs[0] = lRef
		MyFuncType = GetFuncType(PP,IsRefs,retV,RRetRef,false)

	}
	PrePrintEvent := virtual  !() -> void
	{
		pos := -1
		retType := Type^
		if ToClass.Params[^i].ItName == ParName
		{
			pos = i
			break
		}

		if pos == -1 
		{
			FP := FakeFieldParam^
			FPN := char^()
			if ToClass.FakeParams[^i].ItName == ParName
			{
				pos = i
				FP = it
				FPN = it.Atter->{ObjIndent^}.MyStr
				break
			}

			CType := ((ToClass.ClassType)->{Type^})

			pos2 := -1
			midType := FieldParam^
			
			if ToClass.Params[^i].ItName == FPN
			{
				midType = it
				pos2 = i
				break
			}

			if ToClass.ContainVirtual pos2 += 1
			
			itSt2 := "%Pre## = getelementptr "sbt + (CType.GetName()) + " , " + (CType.GetName()) + "* #1, i32 0, i32 "+pos2+" #d\n" +
			"#0 = bitcast " + midType.ResultType.GetName() + "* %Pre## to " + FP.ResultType.GetName() + "* #d\n"
			ToExe = itSt2.Str()
			return void
		}

		usePos := pos
		if ToClass.ContainVirtual
			usePos = pos + 1
		
		clName := ToClass.ClassType.GetName()
		itStr :=  "#0 = getelementptr "sbt + clName + " , " + clName + "* #1, i32 0, i32 "+usePos+" #d\n"
		ToExe = itStr.Str()
	}
}


BuiltInTemplateUnroll := class extend BoxTemplate
{
	ToClass := BoxClass^

	this := !(BoxClass^ toAdd) -> void
	{
		FuncName = "."
		IsPassAttrs = true
		OutputName = "error"

		ToClass = toAdd

		emptType := Queue.{Type^}() ; $temp
		emptType.Push(toAdd.ClassType)
		miniArr := true
		MyFuncType = GetFuncType(emptType,miniArr&,null->{Type^},false,false)
		IsMethod = true
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if pars.Size() != 1 return 255
		if not pars[0].first is TypeClass return 255
		if consts.Size() != 1 return 255
		if not consts[0] is ObjStr return 255

		asStrT := (consts[0]->{ObjStr^})
		asStr := asStrT.GetString()

		if ToClass.GetFieldParam(asStr) != null
			return 0
		
		return 255
	}

	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		
		Name := string
		if consts.Size() != 1 return null
		if not consts[0] is ObjStr
			return null
		AsStrObj := (consts[0]->{ObjStr^})
		Name = (AsStrObj.GetString())

		retF := ToClass.GetFieldParam(Name)
		retType := retF.ResultType

		return new BuiltInUnrollClass(Name,ToClass,true,retType,true)
	}
	PrePrintEvent := virtual !() -> void
	{
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

BuiltInAutoField := class extend BuiltInFunc
{
	ToClass := BoxClass^
	ParName := char^

	this := !(Type^ l,bool lRef,BoxClass^ cl,char^ pName,char^ code) -> void
	{
		FuncName = "."
		OutputName = "."
		ToExe = code
		IsRetRef = lRef

		PP := Queue.{Type^}()
		MyFuncType = GetFuncType(PP,null->{bool^},l,lRef,false)

		ToClass = cl
		ParName = pName
	}
	PrePrintEvent := virtual !() -> void
	{
		if ToExe == null
		{

			pos := -1
			if ToClass.Params[^i].ItName == ParName
			{
				pos = i
				break
			}

			if pos == -1
			{
				if ToClass.FakeParams[^i].ItName == ParName
				{
					pos = i
					break
				}

				CType := ((ToClass.ClassType)->{Type^})
				CTypeP := CType.GetPoint()

				FP := ToClass.FakeParams[pos]
				assert(FP.Atter is ObjIndent) //TODO: check in GetPrior
				FPN := ((FP.Atter)->{ObjIndent^}).MyStr
				pos2 := -1
				midType := FieldParam^

				if ToClass.Params[^i].ItName == FPN
				{
					pos2 = i
					midType = it
					break
				}
				assert(pos2 != -1) //TODO: check in GetPrior

				if ToClass.ContainVirtual pos2 += 1
				itSt := "%Pre## = getelementptr "sbt + (CType.GetName()) + " , " + (CTypeP.GetName()) + " %this, i32 0, i32 "+pos2+" #d\n" +
				"#0 = bitcast " + midType.ResultType.GetName() + "* %Pre## to " + FP.ResultType.GetName() + "* #d\n"
				ToExe = itSt.Str()

				return void
			}

			CType := ((ToClass.ClassType)->{Type^})
			CTypeP := CType.GetPoint()
			fatPos := pos
			if ToClass.ContainVirtual fatPos += 1
			itStr := "#0 = getelementptr "sbt+ (CType.GetName()) + " , " + (CTypeP.GetName()) + " %this, i32 0, i32 "+fatPos+" #d\n"
			ToExe = itStr.Str()
		}
	}
}

BuiltInTemplateAutoField := class extend BoxTemplate
{
	ToClass := BoxClass^

	this := !(BoxClass^ toAdd) -> void
	{
		FuncName = "."
		OutputName = "error"

		ToClass = toAdd

		emptType := Queue.{Type^}()
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if pars.Size() != 0 return 255
		if consts.Size() != 1 return 255
		if (not consts[0] is ObjStr) return 255

		asStrT := (consts[0]->{ObjStr^})
		asStr := asStrT.GetString()

		if ToClass.GetFieldParam(asStr) != null
			return 0
		return 255
	}

	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		Name := string
		if consts.Size() != 1 return null
		if not consts[0] is ObjStr
			return null
		AsStrObj := (consts[0]->{ObjStr^})
		Name = (AsStrObj.GetString())

		itPr := ToClass.GetFieldParam(Name)
		retType := itPr.ResultType
		preRet :=  new BuiltInAutoField(retType,true,ToClass,Name,null->{char^}) //itStr)
		return preRet
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
