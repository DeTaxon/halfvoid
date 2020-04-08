
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
		
		for ToClass.Params
		{
			if it.ItName == asStr
				return 0
			
		}
		for ToClass.FakeParams
		{
			if it.ItName == asStr
				return 0
		}
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

		pos := -1
		retType := Type^
		for it : ToClass.Params, i : 0
		{
			if it.ItName == Name
			{
				pos = i
				retType = it.ResultType
			}
		}
		if pos == -1 
		{
			for it : ToClass.FakeParams, i : 0
			{
				if it.ItName == Name
				{
					pos = i
					retType = it.ResultType
				}
			}
			if pos != -1
			{
				CType := ((ToClass.ClassType)->{Type^})
				CTypeP := CType.GetPoint()

				FP := ToClass.FakeParams[pos]
				if FP.Atter.GetValue() != "~ind" return null //TODO: check in GetPrior and add stuff like 'array[2]'
				FPN := ((FP.Atter)->{ObjIndent^}).MyStr
				pos2 := -1
				midType := FieldParam^

				for ToClass.Params.Size()
				{
					if ToClass.Params[it].ItName == FPN {
						midType = ToClass.Params[it]
						pos2 = it
					}
				}
				if pos2 == -1 return null //TODO: check in GetPrior

				if ToClass.ContainVirtual pos2 += 1
				return  new BuiltInFuncUno(".",CType,true,retType,true,
				"%Pre## = getelementptr "sbt + (CType.GetName()) + " , " + (CTypeP.GetName()) + " #1, i32 0, i32 "+pos2+" #d\n" +
				"#0 = bitcast " + midType.ResultType.GetName() + "* %Pre## to " + FP.ResultType.GetName() + "* #d\n")
			}
			ErrorLog.Push("Cannot find field "+Name+"\n")
			return null
		}

		CType := ((ToClass.ClassType)->{Type^})
		CTypeP := CType.GetPoint()

		usePos := pos
		if ToClass != null
			if ToClass.ContainVirtual {
				usePos = pos + 1
			}
		
	return new BuiltInFuncUno(".",CType,true,retType,true,
		"#0 = getelementptr "sbt + (CType.GetName()) + " , " + (CTypeP.GetName()) + " #1, i32 0, i32 "+usePos+" #d\n")
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
			retType := Type^
			for it : ToClass.Params, i : 0
			{
				if it.ItName == ParName
				{
					pos = i
					//retType = it.ResultType
				}
			}
			if pos == -1
			{
				
				for it : ToClass.FakeParams , i : 0
				{
					if it.ItName == ParName
					{
						pos = i
						//retType = it.ResultType
					}
				}

				CType := ((ToClass.ClassType)->{Type^})
				CTypeP := CType.GetPoint()

				FP := ToClass.FakeParams[pos]
				assert(FP.Atter is ObjIndent) //TODO: check in GetPrior
				FPN := ((FP.Atter)->{ObjIndent^}).MyStr
				pos2 := -1
				midType := FieldParam^

				for ToClass.Params.Size()
				{
					if ToClass.Params[it].ItName == FPN {
						midType = ToClass.Params[it]
						pos2 = it
					}
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

		
		for ToClass.Params.Size()
		{
			if ToClass.Params[it].ItName == asStr
				return 0			
		}
		for ToClass.FakeParams.Size()
		{
			if ToClass.FakeParams[it].ItName == asStr
			{
				FP := ToClass.FakeParams[it]
				if not FP.Atter is ObjIndent return 255 
				FPN := ((FP.Atter)->{ObjIndent^}).MyStr

				for z : ToClass.Params
				{
					if z.ItName == FPN
						return 0
				}

				return 255
			}
		}
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

		pos := -1
		retType := Type^
		for it : ToClass.Params, i : 0
		{
			if it.ItName == Name
			{
				pos = i
				retType = it.ResultType
			}
		}
		if pos == -1 
		{
			for it : ToClass.FakeParams , i : 0
			{
				if it.ItName == Name
				{
					pos = i
					retType = it.ResultType
				}
			}
			assert(pos != -1)
			
			return  new BuiltInAutoField(retType,true,ToClass,Name,null->{char^})
		}
		preRet :=  new BuiltInAutoField(retType,true,ToClass,Name,null->{char^}) //itStr)
		return preRet
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
