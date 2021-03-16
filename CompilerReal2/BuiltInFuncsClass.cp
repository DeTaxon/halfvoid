BuiltIn2UnrollClass := class extend BuiltIn2Func
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
	GetFieldPos := !() -> int
	{
		retType := Type^
		if ToClass.Params[^i].ItName == ParName
		{
			return i
		}
		return -1
	}
	PrintFunc := virtual  !(BuiltIn2Call^ trg,TIOStream f) -> void
	{
		pos := GetFieldPos()
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
			
			cn := CType.GetName()

			trg.Down.PrintPointPre(f)
			itId := trg.GenId() 
			f << "%Pre"<<itId<<" = getelementptr " << cn << " , " << cn << "* "
			f << trg.Down.GetPointName()
			f << ", i32 0, i32 "<<pos2<<"\n"
			f << "%T"<<itId<<" = bitcast " << midType.ResultType.GetName() << "* %Pre" << itId
			f << " to " << FP.ResultType.GetName() << "*\n"
			return void
		}

		usePos := pos
		if ToClass.ContainVirtual
			usePos = pos + 1
		
		clName := ToClass.ClassType.GetName()

		trg.Down.PrintPointPre(f)
		itId := trg.GenId() 
		f << "%T"<< itId <<" = getelementptr " << clName << " , " << clName << "* " 
		f << trg.Down.GetPointName()
		f <<", i32 0, i32 " << usePos <<"\n"
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

		return new BuiltIn2UnrollClass(Name,ToClass,true,retType,true)
	}
	PrePrintEvent := virtual !() -> void
	{
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

BuiltIn2AutoField := class extend BuiltIn2Func
{
	ToClass := BoxClass^
	ParName := char^

	this := !(Type^ l,bool lRef,BoxClass^ cl,char^ pName,char^ code) -> void
	{
		FuncName = "."
		OutputName = "."
		IsRetRef = lRef

		PP := Queue.{Type^}()
		MyFuncType = GetFuncType(PP,null->{bool^},l,lRef,false)

		ToClass = cl
		ParName = pName
	}
	PrintFunc := virtual  !(BuiltIn2Call^ trg,TIOStream f) -> void
	{
		if true 
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

				cn := CType.GetName()

				itId := trg.GenId() 
				f << "%Pre"<<itId<<" = getelementptr " << cn << " , " << cn << "* "
				f << " %this"
				f << ", i32 0, i32 "<<pos2<<"\n"
				f << "%T"<<itId<<" = bitcast " << midType.ResultType.GetName() << "* %Pre" << itId
				f << " to " << FP.ResultType.GetName() << "*\n"

				return void
			}

			CType := ((ToClass.ClassType)->{Type^})
			clName := CType.GetName()
			fatPos := pos
			if ToClass.ContainVirtual fatPos += 1

			itId := trg.GenId() 
			f << "%T"<< itId <<" = getelementptr " << clName << " , " << clName << "* " 
			f << " %this"
			f <<", i32 0, i32 " << fatPos <<"\n"
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
		preRet :=  new BuiltIn2AutoField(retType,true,ToClass,Name,null->{char^}) //itStr)
		return preRet
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
