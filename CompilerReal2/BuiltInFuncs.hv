BuiltInFunc := class extend BoxFunc
{
	ToExe := string
	IsSelfPre := bool

	IsAssembler := virtual !() -> bool
	{
		return true
	}
	CheckIsSelf := !() -> void
	{	
		AsmLine := ToExe

		i := 0
		while AsmLine[i] != 0
		{
			if AsmLine[i] == '#'
			{
				i += 1
				if AsmLine[i] == '^' IsSelfPre = true
			}else i += 1
		}

	}
	PrePrintEvent := virtual !() -> void
	{
	}
}

BuiltInSuffix := class extend BuiltInFunc
{
	this := !(string Name, Type^ l, bool lRef,Type^ retV, SBTType code) -> void
	{
		this."this"(Name,l,lRef,retV,code.Str())
	}
	this := !(string Name, Type^ l, bool lRef,Type^ retV, string code) -> void
	{
		IsSuffix = true
		FuncName = Name
		OutputName = Name
		ToExe = StrCopy(code)

		PP := Queue.{Type^}() ; $temp
		PP.Push(l)

		IsRefs := bool[1]
		IsRefs[0] = lRef
		MyFuncType = GetFuncType(PP,IsRefs,retV,false,false)

		CheckIsSelf()
	}
}

BuiltInFuncZero := class extend BuiltInFunc
{
	this := !(string Name,Type^ retV, bool RRetRef,string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = StrCopy(code)
		IsRetRef = RRetRef

		PP := Queue.{Type^}()
		MyFuncType = GetFuncType(PP,null->{bool^},retV,RRetRef,false)

		CheckIsSelf()
	}
}

BuiltInFuncTypeTimes := class extend BuiltInFunc
{
	this := !(string Name,Type^ inType, int Count,Type^ retV,SBTType code) -> void
	{
		this."this"(Name,inType,Count,retV,code.Str())
	}
	this := !(string Name,Type^ inType, int Count,Type^ retV,string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = StrCopy(code)

		PP := Queue.{Type^}() ; $temp
		for Count PP.Push(inType)

		if Name == ". this" 
		{
			ItConsts.Push(new ObjType(retV)) 
		}

		MyFuncType = GetFuncType(PP,null->{bool^},retV,false,false)

		CheckIsSelf()
	}
}

BuiltInFuncUno := class extend BuiltInFunc
{
	this := !(string Name, Type^ l, bool lRef,Type^ retV,bool RRetRef, SBTType code) -> void
	{
		Init(Name,l,lRef,retV,RRetRef,code.Str())
	}
	this := !(string Name, Type^ l, bool lRef,Type^ retV,bool RRetRef, string code) -> void
	{
		Init(Name,l,lRef,retV,RRetRef,code)
	}
	this := !(string Name, Type^ l, bool lRef,Type^ retV, SBTType code) -> void
	{
		Init(Name,l,lRef,retV,false,code.Str())
	}
	this := !(string Name, Type^ l, bool lRef,Type^ retV, string code) -> void
	{
		Init(Name,l,lRef,retV,false,code)
	}
	Init := !(string Name, Type^ l, bool lRef,Type^ retV, bool RRetRef, string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = StrCopy(code)
		IsRetRef = RRetRef

		PP := Queue.{Type^}() ; $temp
		PP.Push(l)

		IsRefs := bool[1]
		IsRefs[0] = lRef
		MyFuncType = GetFuncType(PP,IsRefs,retV,RRetRef,false)

		CheckIsSelf()
	}
}
BuiltInFuncBinar := class extend BuiltInFunc
{
	this := !(string Name, Type^ l, bool lRef, Type^ r, bool rRef,Type^ retV,bool retRef, SBTType code) -> void
	{
		this."this"(Name,l,lRef,r,rRef,retV,retRef,code.Str())
	}
	this := !(string Name, Type^ l, bool lRef, Type^ r, bool rRef,Type^ retV,bool retRef, string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = StrCopy(code)
		IsRetRef = retRef

		PP := Queue.{Type^}() ; $temp
		PP.Push(l)
		PP.Push(r)

		IsRefs := bool[2]
		IsRefs[0] = lRef
		IsRefs[1] = rRef
		MyFuncType = GetFuncType(PP,IsRefs,retV,retRef,false)
		CheckIsSelf()
	}
	this := !(string Name, Type^ l, bool lRef, Type^ r, bool rRef,Type^ retV, SBTType code) -> void
	{
		this."this"(Name,l,lRef,r,rRef,retV,code.Str())
	}
	this := !(string Name, Type^ l, bool lRef, Type^ r, bool rRef,Type^ retV, string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = StrCopy(code)

		PP := Queue.{Type^}() ; $temp
		PP.Push(l)
		PP.Push(r)

		IsRefs := bool[2]
		IsRefs[0] = lRef
		IsRefs[1] = rRef
		MyFuncType = GetFuncType(PP,IsRefs,retV,false,false)
		CheckIsSelf()
	}
}
BuiltInFuncMega := class extend BuiltInFunc
{
	this := !(string Name, TypeFunc^ fType, SBTType code) -> void
	{
		this."this"(Name,fType,code.Str())
	}
	this := !(string Name, TypeFunc^ fType, string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = StrCopy(code)

		MyFuncType = fType
		CheckIsSelf()
	}
}

BuiltInTemplateGetRef := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "&"
		OutputName = "error"
		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		if itBox.itPars.Size() != 1 return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncUno("&",pars[0].first,true,pars[0].first.GetPoint(), 
		"#0 = getelementptr "sbt + pars[0].first.GetName() + " , " + pars[0].first.GetName() + "* #1, i32 0 #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplatePoint := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "^"
		OutputName = "error"
		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 1 return 255
		if not pars[0].first is TypePoint return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncUno("^",pars[0].first,false,pars[0].first.Base,true, 
		"#0 = getelementptr "sbt + pars[0].first.Base.GetName() + " , " + pars[0].first.GetName() + " #1, i32 0 #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplatePointArr := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "[]"
		OutputName = "error"
		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 2 return 255
		if not (pars[0].first is TypePoint or pars[0].first is TypeArr or  pars[0].first is TypeFatArr) return 255
		
		if not IsInt(pars[1].first) return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		if pars[0].first is TypeArr
		{
			return new BuiltInFuncBinar("[]",pars[0].first,true,pars[1].first,false,pars[0].first.Base,true,
		"#0 = getelementptr "sbt + pars[0].first.GetName() + " , " + pars[0].first.GetName() + "* #1, i32 0, " + pars[1].first.GetName() + " #2 #d\n")
		}
		return new BuiltInFuncBinar("[]",pars[0].first,false,pars[1].first,false,pars[0].first.Base,true,
		"#0 = getelementptr "sbt + pars[0].first.Base.GetName() + " , " + pars[0].first.GetName() + " #1, " + pars[1].first.GetName() + " #2 #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

BuiltInPointAdd := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "+"
		OutputName = "error"
		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		emptType.Push(GetType("s64"))
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 2 return 255
		//if pars[0].first.GetType() != "point" and pars[0].first.GetType() != "arr" and pars[0].first.GetType() != "fatarr" return 255
		if not (pars[0].first is TypePoint or pars[0].first is TypeArr or  pars[0].first is TypeFatArr) return 255
		if pars[0].first == GTypeString return 255
		return TypeCmp(pars[1].first,GetType("s64"))
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		if pars[0].first.Base == GTypeVoid
		{
			return new BuiltInFuncBinar("-",pars[0].first,false,GetType("s64"),false,pars[0].first.Base.GetPoint(),false,
			"#0 = getelementptr i8 , i8* #1 , i64 #2 #d\n")
		}

		return new BuiltInFuncBinar("-",pars[0].first,false,GetType("s64"),false,pars[0].first.Base.GetPoint(),false,
		"#0 = getelementptr "sbt + pars[0].first.Base.GetName() + " , " + pars[0].first.Base.GetName() + "* #1 ,i64 #2 #d\n")
	}
}
BuiltInPointAddInc := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "+="
		OutputName = "error"
		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		emptType.Push(GetType("size_t"))
		bols := bool[2]
		bols[0] = true
		bols[1] = false
		MyFuncType = GetFuncType(emptType,bols[0]&,null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 2 return 255
		//if pars[0].first.GetType() != "point" and pars[0].first.GetType() != "arr" and pars[0].first.GetType() != "fatarr" return 255
		if not pars[0].first is TypePoint return 255
		if not pars[0].second return 255
		return TypeCmp(pars[1].first,GetType("size_t"))
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		if pars[0].first.Base == GTypeVoid
		{
			return new BuiltInFuncBinar("+=",pars[0].first,true,GetType("size_t"),false,pars[0].first.Base.GetPoint(),false,
			"#0Pre = load i8* , i8** #1 #d\n"sbt
			<< "#0 = getelementptr i8 , i8* #0Pre , i64 #2 #d\n"
			<< "store i8* #0,i8** #1 #d\n")
		}

		ptrName := pars[0].first.Base.GetName()
		return new BuiltInFuncBinar("-",pars[0].first,true,GetType("size_t"),false,pars[0].first.Base.GetPoint(),false,
			"#0Pre = load "sbt << ptrName << "* , " << ptrName <<"** #1 #d\n"
			<< "#0 = getelementptr "<< ptrName <<" , "<< ptrName <<"* #0Pre , i64 #2 #d\n"
			<< "store "<< ptrName <<"* #0," << ptrName <<"** #1 #d\n");
	}
}
BuiltInPointSub := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "-"
		OutputName = "error"
		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		emptType.Push(GetType("s64"))
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 2 return 255
		if pars[0].first.GetType() != "point" and pars[0].first.GetType() != "arr" and pars[0].first.GetType() != "fatarr" return 255
		return TypeCmp(pars[1].first,GetType("s64"))
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		if pars[0].first.Base == GTypeVoid
		{
			return new BuiltInFuncBinar("-",pars[0].first,false,GetType("s64"),false,pars[0].first.Base.GetPoint(),false,
			"%Pre## = sub i64 0,#2 #d\n"sbt +
			"#0 = getelementptr i8 , i8* #1 , i64 %Pre## #d\n")
		}

		return new BuiltInFuncBinar("-",pars[0].first,false,GetType("s64"),false,pars[0].first.Base.GetPoint(),false,
		"%Pre## = sub i64 0,#2 #d\n"sbt +
		"#0 = getelementptr " + pars[0].first.Base.GetName() + " , " + pars[0].first.Base.GetName() + "* #1 ,i64 %Pre## #d\n")
	}
}
BuiltInPointSubDec := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "-="
		OutputName = "error"
		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		emptType.Push(GetType("size_t"))
		bols := bool[2]
		bols[0] = true
		bols[1] = false
		MyFuncType = GetFuncType(emptType,bols[0]&,null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 2 return 255
		//if pars[0].first.GetType() != "point" and pars[0].first.GetType() != "arr" and pars[0].first.GetType() != "fatarr" return 255
		if not pars[0].first is TypePoint return 255
		if not pars[0].second return 255
		return TypeCmp(pars[1].first,GetType("size_t"))
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		if pars[0].first.Base == GTypeVoid
		{
			return new BuiltInFuncBinar("-=",pars[0].first,true,GetType("size_t"),false,pars[0].first.Base.GetPoint(),false,
			"#0Pre = load i8* , i8** #1 #d\n"sbt
			<<"#0PreVal = sub i64 0,#2 #d\n"
			<< "#0 = getelementptr i8 , i8* #0Pre , i64 #0PreVal #d\n"
			<< "store i8* #0,i8** #1 #d\n")
		}

		ptrName := pars[0].first.Base.GetName()
		return new BuiltInFuncBinar("-",pars[0].first,true,GetType("size_t"),false,pars[0].first.Base.GetPoint(),false,
			"#0Pre = load "sbt << ptrName << "* , " << ptrName <<"** #1 #d\n"
			<<"#0PreVal = sub i64 0,#2 #d\n"
			<< "#0 = getelementptr "<< ptrName <<" , "<< ptrName <<"* #0Pre , i64 #0PreVal #d\n"
			<< "store "<< ptrName <<"* #0," << ptrName <<"** #1 #d\n");
	}
}

BuiltInTemplateExcArr := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "->{}"
		OutputName = "error"
		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 1 return 255
		if not pars[0].first is TypeArr  return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncUno("->{}",pars[0].first,true,pars[0].first.Base.GetPoint(),false,
		"#0 = getelementptr "sbt + pars[0].first.GetName() + " , " + pars[0].first.GetName() + "* #1, i32 0,i32 0 #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

BuiltInFuncClassInfo := class extend BuiltInFunc
{
	toGet := TypeClass^
	toCheck := int
	this := !(TypeClass^ tp,int toC) -> void
	{
		toGet = tp
		toCheck = toC

		cc := Queue.{Type^}()

		MyFuncType = GetFuncType(cc,null->{bool^},GTypeInt,false,false)

		PostFuncs.Push(this&)
	}
	PostCreate := !() -> void
	{
		asBase := toGet->{Type^}
		if toCheck == 0 //TypeSize
		{
			preSet :=  "%Pre## = getelementptr "sbt + asBase.GetName() + ", " + asBase.GetName() + "* null ,i32 1 #d\n"
				   + "#0 = ptrtoint " + asBase.GetName() + "* %Pre## to i32 #d\n"
			ToExe = preSet.Str()
		}
		if toCheck == 1 //Align
		{
			preSet := "#0 = add i32 "sbt + asBase.GetAlign() + ",0 #d\n"
			ToExe = preSet.Str()
		}
		if toCheck == 2 //FatTypeSize
		{
			preSet := "%Pre## = getelementptr "sbt + asBase.GetName() + ", " + asBase.GetName() + "* null ,i32 1 #d\n"
				+ "#0 = ptrtoint " + asBase.GetName() + "* %Pre## to i32 #d\n"
			ToExe = preSet.Str()
		}
	}
}

BuiltInTemplateTypeInfo := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "->"
		OutputName = "error"
		emptType := Queue.{Type^}()
		MyFuncType = GetFuncType(emptType,null->{bool^},GTypeInt,false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		if pars.Size() != 0 return 255
		if consts.Size() != 2 return 255
		if not consts[0] is ObjType return 255
		if not consts[1] is ObjStr return 255

		asN := consts[1]->{ObjStr^}
		St := asN.GetString()

		if St == "TypeSize" or St == "FatTypeSize" 
		{
			return 0
		}
		if St == "Align" return 0
		return 255
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		asN := consts[1]->{ObjStr^}
		St := asN.GetString()

		intT := GTypeInt

		itT := (((consts[0])->{ObjType^}).MyType)

		if itT is TypeStandart
		{
			itT2 := itT->{TypeStandart^}

			if St == "TypeSize" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 "sbt + itT2.ItSize + ", 0  #d\n")
			if St == "Align" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 "sbt + itT2.ItAlign + ", 0  #d\n")
			if St == "FatTypeSize" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 "sbt + itT2.ItSize + ", 0  #d\n")
		}else
		if itT is TypePoint or itT is TypeFatArr
		{
			//WARN64:
			if St == "TypeSize" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 8, 0  #d\n")
			if St == "Align" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 8, 0  #d\n")
			if St == "FatTypeSize" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 8, 0  #d\n")
		}else
		if itT is TypeClass or itT is TypeArr
		{
			itT2 := itT->{TypeClass^}
			if St == "TypeSize" return new BuiltInFuncClassInfo(itT2,0)
			if St == "Align" return new BuiltInFuncClassInfo(itT2,1)
			if St == "FatTypeSize" return new BuiltInFuncClassInfo(itT2,2)
		}
		assert(false)

		return null
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

BuiltInTemplateExcFatArr := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "->{}"
		OutputName = "error"
		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		return 255

		//if pars.Size() != 1 return 255
		//if pars[0].GetType() != "fatarr" return 255
		//if consts.Size() != 1 return 255
		//if consts[0].GetValue() != "~type" return 255 
		return 0
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ funct) -> BoxFunc^
	{
		//toToT := ((consts[0]) ->{ObjType^}).MyType
		toToT := pars[0].Base.GetPoint()
		return new BuiltInFuncUno("->{}",pars[0],false,pars[0].Base.GetPoint(),false,
		"#0 = bitcast "sbt + pars[0].GetName() +  "#1 to " + toToT.GetName() + " #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateCheckPoint := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "->{}"
		OutputName = "error"
		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		if pars.Size() != 1 return 255
		if not (pars[0].first is TypeFatArr or pars[0].first is TypePoint) return 255
		if consts.Size() != 1 return 255
		if not consts[0] is ObjType return 255

		asNeed := consts[0]->{ObjType^}
		if asNeed.MyType != GTypeBool return 255
		
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncUno("->{}",pars[0].first,false,GTypeBool,false,
		"#0 = icmp ne "sbt + pars[0].first.GetName() +  "#1,null #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}



BuiltInTemplateFuncWrapper := class extend BoxTemplate
{
	ToClass := BoxClass^
	ToFunc := BoxFunc^

	this := !(BoxFunc^ toAdd,BoxClass^ toClass) -> void
	{
		FuncName = toAdd.FuncName
		OutputName = toAdd.OutputName

		ToClass = toClass
		ToFunc = toAdd

		emptType := Queue.{Type^}() ; $temp
		parsC := toAdd.MyFuncType.ParsCount
		
		refs := null->{bool^}

		if parsC > 1 refs = new bool[parsC-1] ; $temp

		PP := Queue.{Type^}() ; $temp

		for parsC
		{
			if it != 0
			{
				refs[it] = toAdd.MyFuncType.ParsIsRef[it-1]
				PP.Push(toAdd.MyFuncType.Pars[it-1])
			}
		}

		MyFuncType = GetFuncType(PP,refs,toAdd.MyFuncType.RetType,toAdd.MyFuncType.RetRef,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		return ComputePriorFunc(MyFuncType,itBox)
	}

	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ fun) -> BoxFunc^
	{
		Name := string
		AsStrObj := (consts[0]->{ObjStr^})
		Name = (AsStrObj.GetString())

		CType := ((ToClass.ClassType)->{Type^})
		CTypeP := CType.GetPoint()

		preRetCode := ""sbt

		if ToFunc.MyFuncType.RetType != GTypeVoid
			preRetCode << "#0 = "

		asBasePre := ToFunc.MyFuncType
		asBase := asBasePre->{Type^}
		preRetCode << asBase.GetName() << "@" << ToFunc.OutputName

		preRet :=  new BuiltInFuncMega(".",ToFunc.MyFuncType,preRetCode)
		return	preRet
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInLenArr := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "![]"
		OutputName = "error"

	}
	GetBestType := !(FuncInputBox itBox) -> Type^
	{
		pars := ref itBox.itPars
		expectedType := pars[0].first
		resPriority := 0

		for pars
		{
			expectedType = TypeFight(it.first,expectedType)
			if expectedType == null
				return null
		}

		return expectedType
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if consts.Size() != 0 return 255
		if pars.Size() == 0 return 255

		for pars
		{
			if it.first == null
				return 255
			if it.first is TypeStandart
				continue
			if it.first is TypePoint
				continue
			return 255
		}
		if GetBestType(itBox) == null
			return 255
			
		return 0
	}
	CreateFuncPointer := virtual !(FuncInputBox itBox) -> TypeFunc^
	{
		pars := ref itBox.itPars
		newQueue := Queue.{Type^}()
		retTyp := GetBestType(itBox)
		for pars.Size() newQueue.Push(retTyp) ; $temp
		return GetFuncType(newQueue,null->{bool^},retTyp.GetArray(pars.Size()),false,false)
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		toCreate := ""sbt

		baseType :=  fun.RetType.Base
		retType := baseType.GetArray(pars.Size())

		baseTypeN := baseType.GetName()

		asBase := retType->{Type^}
		retTypeN := asBase.GetName()

		for i : pars.Size()
		{
			toCreate  + "%TPre##n" + i + " = getelementptr " + retTypeN + " , " + retTypeN + "* #0,i32 0,i32 " + i + " #d\n"
			toCreate  + "store " + baseTypeN + " #" + (i + 1) + " , " + baseTypeN + "* %TPre##n" + i + " #d\n"
		}

		//newQueue := Queue.{Type^}()
		//for pars.Size() newQueue.Push(pars[0].first)

		itFunc := new BuiltInFunc
		itFunc.MyFuncType = fun //GetFuncType(newQueue,null->{bool^},retType,false,false)
		itFunc.ToExe = toCreate.Str()
		itFunc.IsRetComplex = true

		return itFunc
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateNew := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "new"
		OutputName = "error"

		emptType := Queue.{Type^}() ; $temp
		emptType.Push(GTypeInt)
		emptType.Push(GTypeInt)
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if pars.Size() != 2 return 255
		if not (pars[0].first is TypeStandart) return 255
		if not (pars[1].first is TypeStandart) return 255
		if consts.Size() != 1 return 255
		if not (consts[0] is ObjType) return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		ResType := Type^
		if consts.Size() != 1 return null
		if consts[0].GetValue() != "~type" 
			return null
		AsTypeObj := (consts[0]->{ObjType^})
		ResType = AsTypeObj.MyType
		ResP := ResType.GetFatArray()
		ExtraSize := ResType.GetAlign()
		if ExtraSize < 4 ExtraSize = 4

		return new BuiltInFuncBinar("new",pars[0].first,false,pars[1].first,false,ResP,
			"%PreTotalSize## = mul i32 #1,#2 #d"sbt + "\n" +
			"%TotalSize## = add i32 %PreTotalSize##, " + ExtraSize + " #d\n" +
			"%PrePtr## = call i8*(i32)@malloc(i32 %TotalSize##) #d\n"+
			"%PtrInt## = ptrtoint i8* %PrePtr## to i64 #d\n" + 
			"%RealPtrInt## = add i64 %PtrInt## , " + ExtraSize + " #d\n" +
			"%SizePtrInt## = sub i64 %RealPtrInt## , 4 #d\n" +
			"%SizePtr## = inttoptr i64 %SizePtrInt## to i32* #d\n" + 
			"store i32 #2, i32* %SizePtr## #d\n" +
			"#0 = inttoptr i64 %RealPtrInt## to " + ResP.GetName() + " #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateNext := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "->"
		OutputName = "error"

		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},GTypeInt,false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if consts.Size() != 1 return 255
		if not (consts[0] is ObjStr) return 255
		if pars.Size() != 1 return 255

		asObjStr := consts[0]->{ObjStr^}
		asStr := asObjStr.GetString()
		
		if asStr == "len"
		{
			if not (pars[0].first is TypeFatArr or pars[0].first is TypeArr) return 255
			return 0
		}
		if asStr == "begin"
		{
			if pars[0].first == TypeTable[13] or pars[0].first == TypeTable[14] return 0
			return 255
		}
		if asStr == "end"
		{
			if pars[0].first == TypeTable[13] or pars[0].first == TypeTable[14] return 0
			return 255
		}

		return 255
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		asPreStr := consts[0]->{ObjStr^}
		asStr := asPreStr.GetString()

		if asStr == "begin"
		{
			return new BuiltInFuncUno("->",pars[0].first,false,pars[0].first.Base,false,
					"#0 = extractvalue "sbt + pars[0].first.GetName() + " #1,0 #d\n")
		}else
		if asStr == "end"
		{
			return new BuiltInFuncUno("->",pars[0].first,false,pars[0].first.Base,false,
					"#0 = extractvalue "sbt + pars[0].first.GetName() + " #1,1 #d\n")
		}else
		if asStr == "len"
		{
			if pars[0].first is TypeArr
			{	
				asType := (pars[0].first)->{TypeArr^}
				pre := new BuiltInFuncUno("->",pars[0].first,true,GTypeInt,false,
					"#0 = add i32 0,"sbt + asType.Size + " #d\n")
				pre.IsSelfPre = true
				return pre
			}else{
				return new BuiltInFuncUno("->",pars[0].first,false,GTypeInt,false,
				"%PreP## = bitcast "sbt + pars[0].first.GetName() + " #1 to i32* #d\n" + 
				"%PreI## = getelementptr i32, i32* %PreP##,i32 -1 #d\n" +
				"#0 = load i32 , i32 * %PreI## #d\n")
			}
		}
		return null
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateCmpPoints := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "=="
		OutputName = "error"

		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if consts.Size() != 0 return 255
		if pars.Size() != 2 return 255
		if not (pars[0].first is TypePoint or pars[0].first is TypeFatArr or pars[0].first is TypeFuncLambda) return 255
		if not (pars[1].first is TypePoint or pars[1].first is TypeFatArr or pars[0].first is TypeFuncLambda) return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncBinar("->",pars[0].first,false,pars[1].first,false,GTypeBool,false,
			"%PreOne## = bitcast "sbt + pars[0].first.GetName() + " #1 to i8* #d\n" +
			"%PreTwo## = bitcast "    + pars[1].first.GetName() + " #2 to i8* #d\n" +
			"#0 = icmp eq i8* %PreOne##,%PreTwo## #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateCmpPointsNE := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "!="
		OutputName = "error"

		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if consts.Size() != 0 return 255
		if pars.Size() != 2 return 255
		if not (pars[0].first is TypePoint or pars[0].first is TypeFatArr) return 255
		if not (pars[1].first is TypePoint or pars[1].first is TypeFatArr) return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncBinar("->",pars[0].first,false,pars[1].first,false,GTypeBool,false,
			"%PreOne## = bitcast "sbt + pars[0].first.GetName() + " #1 to i8* #d\n" +
			"%PreTwo## = bitcast "    + pars[1].first.GetName() + " #2 to i8* #d\n" +
			"#0 = icmp ne i8* %PreOne##,%PreTwo## #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateRefEx := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "."
		OutputName = "error"

		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
		IsMethod = true
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		if itBox.itPars.Size() != 1 return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		asType := consts[0]->{ObjType^}
		return new BuiltInFuncUno("->",pars[0].first,true,asType.MyType,true,
			"#0 = bitcast "sbt + pars[0].first.GetPoint().GetName() + " #1 to " + asType.MyType.GetPoint().GetName() + " #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

BuiltInTemplateStandartTypeConstructor := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = ". this"
		OutputName = "error"

		emptType := Queue.{Type^}() ; $temp
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
		IsMethod = true
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		if itBox.itPars.Size() != 0 return 255
		if itBox.itConsts.Size() != 1 return 255
		if not itBox.itConsts[0] is ObjType return 255

		asTyp := itBox.itConsts[0]->{ObjType^}
		if asTyp.MyType is TypeClass return 255
		if asTyp.MyType is TypeArr return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		asType2 := itBox.itConsts[0]->{ObjType^}
		asType := asType2.MyType

		if asType is TypePoint or asType is TypeFatArr
		{
			if asType == GTypeVoidP{
				return new BuiltInFuncZero(". this",asType,false,
					"#0 = getelementptr i8 , i8* null, i32 0 #d\n")
			}
			return new BuiltInFuncZero(". this",asType,false,
				"#0 = getelementptr "sbt + asType.Base.GetName() + " , " + asType.GetName() + " null, i32 0 #d\n")
		}
		if asType is TypeStandart
		{
			if IsInt(asType)
			{
				return new BuiltInFuncZero(". this",asType,false,
					"#0 = add "sbt + asType.GetName() +   " 0, 0 #d\n")
			}
			if IsFloat(asType)
			{
				if asType == GTypeFloat
				{
					return new BuiltInFuncZero(". this",asType,false,
						"#0 = fptrunc double 0.0 to float #d\n")
				}
				return new BuiltInFuncZero(". this",asType,false,
					"#0 = fadd "sbt + asType.GetName() +   " 0.0, 0.0 #d\n")
			}
			if asType == GTypeBool
			{
				return new BuiltInFuncZero(". this",asType,false,
					"#0 = add i1 0,0 #d\n")
			}
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
AddTemplates := !() -> void
{
	BuiltInTemplates.Push(new BuiltInTemplateStandartTypeConstructor())
	BuiltInTemplates.Push(new BuiltInTemplatePointArr())
	BuiltInTemplates.Push(new BuiltInTemplateSet())
	BuiltInTemplates.Push(new BuiltInTemplateSetGCFatArr())
	BuiltInTemplates.Push(new BuiltInTemplateNext())
	//BuiltInTemplates.Push(new BuiltInTemplateStorePoint())
	BuiltInTemplates.Push(new BuiltInTemplateGetRef())

	BuiltInTemplates.Push(new BuiltInTemplateTypeInfo())

	//GlobalUnroll = new BuiltInTemplateUnroll()
	//GlobalNew = new BuiltInTemplateNew()
	GlobalUnpoint = new BuiltInTemplatePoint()
	GlobalRefExc = new BuiltInTemplateRefEx()
	GlobalExcArr = new BuiltInTemplateExcArr()

	//BuiltInTemplates.Push(new BuiltInTemplateExcFatArr())
	BuiltInTemplates.Push(GlobalUnpoint)
	BuiltInTemplates.Push(new BuiltInLenArr())
	BuiltInTemplates.Push(new BuiltInTemplateCmpPoints())
	BuiltInTemplates.Push(new BuiltInTemplateCmpPointsNE())
	BuiltInTemplates.Push(new BuiltInTemplateCheckPoint())
	BuiltInTemplates.Push(new BuiltInTemplateVec4fGet())

	BuiltInTemplates.Push(new BuiltInPointAdd())
	BuiltInTemplates.Push(new BuiltInPointAddInc())
	BuiltInTemplates.Push(new BuiltInPointSub())
	BuiltInTemplates.Push(new BuiltInPointSubDec())

	BuiltInTemplates.Push(new BuiltInTemplateZeroMem())

	//BuiltInTemplates.Push(GlobalUnroll)
	CreateLambdaBuilts()
}

AddBuiltInFunc := !(BoxFunc^ fnc) -> void
{
	BuiltInFuncs[fnc.FuncName].Push(fnc)
}

CreateBuiltIns := !() -> void
{
	AddTemplates()

	BoolT := GetType("bool")
	VoidT := GetType("void")
	VoidP := VoidT.GetPoint()
	DoubleT := GetType("double")

	preRes := int[4]
	preRes[0] = 8
	preRes[1] = 16
	preRes[2] = 32
	preRes[3] = 64
	preRes2 := string[2]
	preRes2[0] = "s"
	preRes2[1] = "u"

	for iPre : 4 for jPre : 4
	for IsS1Pre : 2
	for IsS2Pre : 2
	{
		i := preRes[iPre]
		j := preRes[jPre]
		IsS1 := preRes2[IsS1Pre]
		IsS2 := preRes2[IsS2Pre]
		from := GetType(""sbt + IsS1 + i)
		to := GetType(""sbt + IsS2 + j)
		if i > j
		{
			BuiltInExcs.Push(new BuiltInFuncUno("->{}",from,false,to,false,"#0 = trunc "sbt + from.GetName() + " #1 to " + to.GetName() + " #d\n"))
		}
		if i < j
		{
			if IsS1 == "s"
			{
				BuiltInExcs.Push(new BuiltInFuncUno("->{}",from,false,to,"#0 = sext "sbt + from.GetName() + " #1 to " + to.GetName() + " #d\n"))
			}else{
				BuiltInExcs.Push(new BuiltInFuncUno("->{}",from,false,to,"#0 = zext "sbt + from.GetName() + " #1 to " + to.GetName() + " #d\n"))
			}
		}
		if i == j 
		{
			BuiltInExcs.Push(new BuiltInFuncUno("->{}",from,false,to,"#0 = add "sbt + from.GetName() + " #1,0 #d\n"))
		}
		//if i == j
		//{
		//}
	}


	for itPre : 4
	{
		it := preRes[itPre]
		for IsSPre : 2
		{
			IsS := preRes2[IsSPre]
			PType := GetType(""sbt + IsS + it) // u8 s32 ...

			AddBuiltInFunc(new BuiltInFuncBinar("InterlockAdd",PType,true,PType,false,PType,"#0 = atomicrmw add  i"sbt + it + "* #1, i" + it + " #2 acquire #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("InterlockSub",PType,true,PType,false,PType,"#0 = atomicrmw sub  i"sbt + it + "* #1, i" + it + " #2 acquire #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("InterlockSet",PType,true,PType,false,PType,"#0 = atomicrmw xchg i"sbt + it + "* #1, i" + it + " #2 acquire #d\n"))

			AddBuiltInFunc(new BuiltInFuncBinar("=",PType,true,PType,false,PType,"store i"sbt + it + " #2, i" + it + "* #1 #d\n"
												+"#0 = add i" + it + " #2,0 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("+",PType,false,PType,false,PType,"#0 = add i"sbt + it + " #1,#2 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("-",PType,false,PType,false,PType,"#0 = sub i"sbt + it + " #1,#2 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("*",PType,false,PType,false,PType,"#0 = mul i"sbt + it + " #1,#2 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("div",PType,false,PType,false,PType,"#0 = "sbt + IsS + "div  i" + it + " #1,#2 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("%",PType,false,PType,false,PType,"#0 = "sbt + IsS + "rem  i" + it + " #1,#2 #d\n"))
			
			AddBuiltInFunc(new BuiltInFuncUno(". -",PType,false,PType,false,"#0 = sub i"sbt + it + " 0,#1 #d\n"))
			
			AddBuiltInFunc(new BuiltInFuncBinar("/",PType,false,PType,false,GTypeFloat,
										"%Pre1## = "sbt + IsS + "itofp i" + it + " #1 to float #d\n" +
										"%Pre2## = " + IsS + "itofp i" + it +" #2 to float #d\n" +
										"#0 = fdiv float %Pre1##,%Pre2## #d\n"))

			AddBuiltInFunc(new BuiltInFuncBinar("==",PType,false,PType,false,BoolT,"#0 = icmp eq i"sbt + it + " #1,#2 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("!=",PType,false,PType,false,BoolT,"#0 = icmp ne i"sbt + it + " #1,#2 #d\n"))

			AddBuiltInFunc(new BuiltInFuncBinar("and_b",PType,false,PType,false,PType,"#0 = and i"sbt + it + " #1,#2 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("or_b",PType,false,PType,false,PType,"#0 = or i"sbt + it + " #1,#2 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("xor_b",PType,false,PType,false,PType,"#0 = xor i"sbt + it + " #1,#2 #d\n"))
			AddBuiltInFunc(new BuiltInFuncUno(". not_b",PType,false,PType,"#0 = xor i"sbt + it + " #1,-1 #d\n"))

			AddBuiltInFunc(new BuiltInFuncBinar(">=",PType,false,PType,false,BoolT,"#0 = icmp "sbt + IsS +"ge i" + it + " #1,#2 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("<=",PType,false,PType,false,BoolT,"#0 = icmp "sbt + IsS +"le i" + it + " #1,#2 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar(">",PType,false,PType,false,BoolT,"#0 = icmp "sbt + IsS +"gt i" + it + " #1,#2 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("<",PType,false,PType,false,BoolT,"#0 = icmp "sbt + IsS +"lt i" + it + " #1,#2 #d\n"))

			AddBuiltInFunc(new BuiltInFuncBinar("<=>",PType,false,PType,false,GTypeInt,
				"%PrePre1## = icmp "sbt + IsS + "le i" + it + " #1,#2\n" +
				"%PrePre2## = icmp " + IsS + "le i" + it + " #2,#1\n" +
				"%Pre1## = zext i1 %PrePre1## to i32\n" + 
				"%Pre2## = zext i1 %PrePre2## to i32\n" + 
				"#0 = sub i32 %Pre2## , %Pre1## #d\n")) 


			BuiltInExcs.Push(new BuiltInFuncUno("->{}",PType,false,BoolT,"#0 = icmp ne i"sbt +it+" #1 ,0 #d\n"))
			BuiltInExcs.Push(new BuiltInFuncUno("->{}",PType,false,DoubleT,"#0 = "sbt +IsS+"itofp i"+it+" #1 to double #d\n"))

			AddBuiltInFunc(new BuiltInFuncBinar("+=",PType,true,PType,false,PType,"#0pre = load i"sbt + it + " , i" + it + "* #1 #d\n"
												+"#0 = add i" + it + " #2,#0pre #d\n"
												+"store i"+it+" #0, i"+it+"* #1 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("-=",PType,true,PType,false,PType,"#0pre = load i"sbt + it + " , i" + it + "* #1 #d\n"
												+"#0 = sub i" + it + " #0pre,#2 #d\n"
												+"store i"+it+" #0, i"+it+"* #1 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("*=",PType,true,PType,false,PType,"#0pre = load i"sbt + it + " , i" + it + "* #1 #d\n"
												+"#0 = mul i" + it + " #2,#0pre #d\n"
												+"store i"+it+" #0, i"+it+"* #1 #d\n"))

			AddBuiltInFunc(new BuiltInFuncUno("++",PType,true,PType,"#0 = load i"sbt + it + " , i" + it + "* #1 #d\n"
												+"#0Pre = add i" + it + " 1,#0 #d\n"
												+"store i"+it+" #0Pre, i"+it+"* #1 #d\n"))
			AddBuiltInFunc(new BuiltInFuncUno("--",PType,true,PType,"#0 = load i"sbt + it + " , i" + it + "* #1 #d\n"
												+"#0Pre = sub i" + it + " #0,1 #d\n"
												+"store i"+it+" #0Pre, i"+it+"* #1 #d\n"))
			AddBuiltInFunc(new BuiltInFuncBinar("<<",PType,false,PType,false,PType, "#0 = shl i"sbt + it + " #1,#2 #d\n"))
		}
		BuiltInExcs.Push( new BuiltInFuncUno("->{}", GetType("s"sbt + it), false,GetType("u"sbt + it), "#0 = add i"sbt + it + " #1,0 #d"))
		AddBuiltInFunc(new BuiltInFuncBinar(">>",GetType("s"sbt + it),false,GetType("s"sbt + it),false,GetType("s"sbt + it), "#0 = ashr i"sbt + it + " #1,#2 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar(">>",GetType("u"sbt + it),false,GetType("s"sbt + it),false,GetType("u"sbt + it), "#0 = lshr i"sbt + it + " #1,#2 #d\n"))
	}
	AddBuiltInFunc(new BuiltInFuncBinar("in",TypeTable[13],false,GTypeInt,false,BoolT,
											"br label %Start##\n"sbt +
											"Start##:" +
											"%First## = extractvalue %RangeTypeInt #1,0 #d\n"+
											"%T1T## = icmp sge i32 #2,%First## #d\n" + 
											"br i1 %T1T## ,label %OnNext##, label %OnEnd## #d\n" +
											"OnNext##:\n"+
											"%Second## = extractvalue %RangeTypeInt #1,1 #d\n" +
											"%T2T## = icmp sle i32 #2, %Second## #d\n" +
											"br label %OnEnd## #d\n" +
											"OnEnd##:\n" +
											"#0 = phi i1 [0, %Start##], [%T2T##, %OnNext##] #d\n"))
	
	preRes3 := string[2]
	preRes3[0] = "float"
	preRes3[1] = "double"
	for itPre : 2
	{
		it := preRes3[itPre]
		PType := GetType(it)
		AddBuiltInFunc(new BuiltInFuncBinar("=",PType,true,PType,false,PType,"store "sbt + it + " #2, " + it + "* #1 #d\n"
											+"#0 = fadd " + it + " #2,0.0 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar("+",PType,false,PType,false,PType,"#0 = fadd "sbt + it + " #1,#2 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar("-",PType,false,PType,false,PType,"#0 = fsub "sbt + it + " #1,#2 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar("*",PType,false,PType,false,PType,"#0 = fmul "sbt + it + " #1,#2 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar("/",PType,false,PType,false,PType,"#0 = fdiv "sbt + it + " #1,#2 #d\n"))
		AddBuiltInFunc(new BuiltInFuncUno(". -",PType,false,PType,false,"#0 = fsub "sbt + it + " 0.0,#1 #d\n"))

		AddBuiltInFunc(new BuiltInFuncBinar("+=",PType,true,PType,false,PType,"#0pre = load "sbt + it +" , "+ it +"* #1 #d\n"
											+"#0 = fadd " + it + " #2,#0pre #d\n"
											+"store "+it+" #0, "+it+"* #1 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar("-=",PType,true,PType,false,PType,"#0pre = load "sbt + it + " , " + it + "* #1 #d\n"
											+"#0 = fsub " + it + " #0pre,#2 #d\n"
											+"store "+it+" #0, "+it+"* #1 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar("*=",PType,true,PType,false,PType,"#0pre = load "sbt + it + " , " + it + "* #1 #d\n"
											+"#0 = fmul " + it + " #2,#0pre #d\n"
											+"store "+it+" #0, "+it+"* #1 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar("/=",PType,true,PType,false,PType,"#0pre = load "sbt + it + " , " + it + "* #1 #d\n"
											+"#0 = fdiv " + it + " #0pre,#2 #d\n"
											+"store "+it+" #0, "+it+"* #1 #d\n"))


		AddBuiltInFunc(new BuiltInFuncBinar(">=",PType,false,PType,false,BoolT,"#0 = fcmp uge "sbt + it + " #1,#2 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar("<=",PType,false,PType,false,BoolT,"#0 = fcmp ule "sbt + it + " #1,#2 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar(">",PType,false,PType,false,BoolT,"#0 = fcmp ugt "sbt + it + " #1,#2 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar("<",PType,false,PType,false,BoolT,"#0 = fcmp ult "sbt + it + " #1,#2 #d\n"))

		BuiltInExcs.Push(new BuiltInFuncUno("->{}",PType,false,GTypeInt,"#0 = fptosi "sbt + it + " #1 to i32 #d\n"))

		if it == "float"
		{
			AddBuiltInFunc(new BuiltInFuncBinar("**",PType,false,PType,false,PType,"#0 = call float @llvm.pow.f32(float #1,float #2) #d\n"))
		}else{
			AddBuiltInFunc(new BuiltInFuncBinar("**",PType,false,PType,false,PType,"#0 = call double @llvm.pow.f64(double #1,double #2) #d\n"))
		}
	}
	AddBuiltInFunc(new BuiltInFuncBinar("=",GTypeHalf,true,GTypeHalf,false,GTypeVoid,"store half #2, half* #1 #d\n"))

	AddBuiltInFunc(new BuiltInFuncBinar("=",BoolT,true,BoolT,false,BoolT,"store i1 #2, i1* #1 #d\n"sbt
										+"#0 = add i1 #2,0 #d\n"))
	AddBuiltInFunc(new BuiltInFuncBinar("==",BoolT,false,BoolT,false,BoolT,"#0 = icmp eq i1 #1,#2 #d\n"))
	AddBuiltInFunc(new BuiltInFuncBinar("!=",BoolT,false,BoolT,false,BoolT,"#0 = icmp ne i1 #1,#2 #d\n"))
	AddBuiltInFunc(new BuiltInFuncBinar("xor",BoolT,false,BoolT,false,BoolT,"#0 = xor i1 #1,#2 #d\n"))

	
	BuiltInExcs.Push(new BuiltInFuncUno("->{}",GTypeDouble,false,GTypeFloat,"#0 = fptrunc double #1 to float #d\n"))
	BuiltInExcs.Push(new BuiltInFuncUno("->{}",GTypeFloat,false,GTypeDouble,"#0 = fpext float #1 to double #d\n"))

	BuiltInExcs.Push(new BuiltInFuncUno("->{}",GTypeFloat,false,GTypeHalf,"#0 = fptrunc float #1 to half #d\n"))
	BuiltInExcs.Push(new BuiltInFuncUno("->{}",GTypeHalf,false,GTypeFloat,"#0 = fpext half #1 to float #d\n"))

	BuiltInExcs.Push(new BuiltInFuncUno("->{}",GTypeDouble,false,GTypeHalf,"#0Pre = fptrunc double #1 to float #d\n"sbt +
										"#0 = fptrunc float #0Pre to half #d\n"))
	BuiltInExcs.Push(new BuiltInFuncUno("->{}",GTypeHalf,false,GTypeDouble,"#0Pre = fpext half #1 to float #d\n"sbt +
										"#0 = fpext float #0Pre to double #d\n"))

	BuiltInExcs.Push(new BuiltInFuncUno("->{}",GTypeInt,false,GTypeHalf,"#0Pre = sitofp i32 #1 to float #d\n"sbt +
										"#0 = fptrunc float #0Pre to half #d\n"))
	BuiltInExcs.Push(new BuiltInFuncUno("->{}",GTypeHalf,false,GTypeInt,"#0Pre = fpext half #1 to float #d\n"sbt +
										"#0 = fptosi float #0Pre to double #d\n"))

	BuiltInExcs.Push(new BuiltInFuncUno("->{}",GTypeInt,false,GTypeFloat,"#0 = sitofp i32 #1 to float #d\n"))

	BuiltInExcs.Push(new BuiltInFuncUno("->{}",GTypeVoidP,false,GetType("size_t"),"#0 = ptrtoint i8* #1 to i64 #d\n"))
	BuiltInExcs.Push(new BuiltInFuncUno("->{}",GetType("size_t"),false,GTypeVoidP,"#0 = inttoptr i64 #1 to i8* #d\n"))


	AddBuiltInFunc(new BuiltInFuncUno(". not",BoolT,false,BoolT,"#0 = xor i1 #1,1 #d\n"))
	AddBuiltInFunc( new BuiltInSuffix("f",GTypeDouble,false,GTypeFloat,"#0 = fptrunc double #1 to float #d\n"))
	AddBuiltInFunc( new BuiltInSuffix("L",GTypeInt,false,GetType("s64"),"#0 = sext i32 #1 to i64 #d\n"))
	AddBuiltInFunc( new BuiltInSuffix("U",GTypeInt,false,GetType("u64"),"#0 = zext i32 #1 to i64 #d\n"))
	AddBuiltInFunc( new BuiltInSuffix("pi",GTypeFloat,false,GTypeFloat,"%Pre## = fptrunc double 3.14159265389 to float #d\n"sbt +
					"#0 = fmul float #1,%Pre## #d\n"))
	AddBuiltInFunc( new BuiltInSuffix("deg",GetType("float"),false,GetType("float"),"%Pre## = fptrunc double 0.017453292521161111 to float #d\n"sbt +
					"#0 = fmul float #1,%Pre## #d\n"))
	AddBuiltInFunc( new BuiltInSuffix("deg",GetType("int"),false,GetType("float"),"%Pre## = fptrunc double 0.017453292521161111 to float #d\n"sbt +
					"%PrePre## = sitofp i32 #1 to float #d\n" +
					"#0 = fmul float %PrePre##,%Pre## #d\n"))
	AddBuiltInFunc( new BuiltInSuffix("pi",GTypeInt,false,GTypeFloat,
					"%PrePre## = sitofp i32 #1 to float #d\n"sbt +
					"%Pre## = fptrunc double 3.14159265389 to float #d\n" +
					"#0 = fmul float %PrePre##,%Pre## #d\n"))

	AddBuiltInFunc( new BuiltInSuffix("ms",GTypeDouble,false,GTypeDouble,"#0 = fmul double #1,0.001 #d\n"))
	AddBuiltInFunc( new BuiltInSuffix("s",GTypeDouble,false,GTypeDouble,"#0 = fmul double #1,1.0 #d\n"))
	AddBuiltInFunc( new BuiltInSuffix("min",GTypeDouble,false,GTypeDouble,"#0 = fmul double #1,60.0 #d\n"))
	AddBuiltInFunc( new BuiltInSuffix("hour",GTypeDouble,false,GTypeDouble,"#0 = fmul double #1,3600.0 #d\n"))

	AddBuiltInFunc( new BuiltInFuncBinar("<",GTypeVoidP,false,GTypeVoidP,false,GTypeBool,false,"#0 = icmp ult i8* #1,#2 #d\n"))
	AddBuiltInFunc( new BuiltInFuncBinar("<=>",GTypeVoidP,false,GTypeVoidP,false,GTypeInt,false,
				"%PrePre1## = icmp ule i8* #1,#2 #d\n"sbt +
				"%PrePre2## = icmp ule i8* #2,#1 #d\n" +
				"%Pre1## = zext i1 %PrePre1## to i32 #d\n" + 
				"%Pre2## = zext i1 %PrePre2## to i32 #d\n" + 
				"#0 = sub i32 %Pre2## , %Pre1## #d\n")) 
	AddBuiltInFunc( new BuiltInFuncBinar("<=>",GTypeBool,false,GTypeBool,false,GTypeInt,false,
				"%Pre1## = zext i1 #1 to i32 #d\n"sbt + 
				"%Pre2## = zext i1 #2 to i32 #d\n" + 
				"#0 = sub i32 %Pre2## , %Pre1## #d\n")) 
	AddBuiltInFunc(new BuiltInFuncBinar("-",GTypeVoidP,false,GTypeVoidP,false,GetType("size_t"),
						"#0Pre1 = ptrtoint i8* #1 to i64 #d\n"sbt +
						"#0Pre2 = ptrtoint i8* #2 to i64 #d\n" +
						"#0 = sub i64 #0Pre1,#0Pre2 #d\n"))

	AddBuiltInFunc(new BuiltInFuncUno("internalSetJmp",GTypeVoidP,false,GTypeInt,"#0 = call i32 @llvm.eh.sjlj.setjmp(i8* #1) #d\n"))
	AddBuiltInFunc(new BuiltInFuncUno("internalLongJmp",GTypeVoidP,false,GTypeVoid,"call void @llvm.eh.sjlj.longjmp(i8* #1) #d\nunreachable\n"))
	AddBuiltInFunc(new BuiltInFuncUno("setjmp",GTypeVoidP,false,GTypeInt,"#0 = call i32 @setjmp(i8* #1) #d\n"))
	AddBuiltInFunc(new BuiltInFuncBinar("longjmp",GTypeVoidP,false,GTypeInt,false,GTypeVoid,"call void @longjmp(i8* #1,i32 #2) #d\nunreachable\n"))
	AddBuiltInFunc(new BuiltInFuncZero("internalLSDA",GTypeVoidP,false,"#0 = call i8* @llvm.eh.sjlj.lsda() #d\n"))
	AddBuiltInFunc(new BuiltInFuncZero("internalDebugTrap",GTypeVoid,false,"call void @llvm.debugtrap() #d\n"))
	AddBuiltInFunc(new BuiltInFuncZero("internalGetCallFrame",GTypeVoidP,false,"#0 = call i8* @llvm.frameaddress(i32 0) #d\n"))
	AddBuiltInFunc(new BuiltInFuncZero("internalStackSave",GTypeVoidP,false,"#0 = call i8* @llvm.stacksave() #d\n"))
	AddBuiltInFunc(new BuiltInFuncUno("internalStackRestore",GTypeVoidP,false,GTypeVoid,"call i8* @llvm.stackrestore(i8* #1) #d\n"))

	AddBuiltInFunc(new BuiltInFuncUno("va_start",GTypeVoidP,false,GTypeVoid,"call void @llvm.va_start(i8* #1) #d\n"))
	AddBuiltInFunc(new BuiltInFuncUno("va_end",GTypeVoidP,false,GTypeVoid,"call void @llvm.va_end(i8* #1) #d\n"))
	AddBuiltInFunc(new BuiltInFuncBinar("va_copy",GTypeVoidP,false,GTypeVoidP,false,GTypeVoid,"call void @llvm.va_copy(i8* #1,i8* #2) #d\n"))

	AddBuiltInFunc(new BuiltInFuncUno("builtin_setjump",GTypeVoidP,false,GTypeVoid,
			"%AsArray## = bitcast i8* #1 to [20 x i8*]*\n"sbt 
  			+ "%Frame##  = call i8* @llvm.frameaddress.p0i8(i32 0)\n"
			+ "%FrameStore## = getelementptr [20 x i8*], [20 x i8*]* %AsArray## , i64 0, i64 0\n"
  			+ "%Stack## = call i8* @llvm.stacksave()\n"
			+ "%StackStore## = getelementptr [20 x i8*], [20 x i8*]* %AsArray##, i64 0, i64 2\n"
  			+ "store i8* %Stack##, i8** %StackStore##, align 16\n"
  			+ "#0 = call i32 @llvm.eh.sjlj.setjmp(i8* #1)\n"))
	AddBuiltInFunc(new BuiltInFuncUno("builtin_longjump",GTypeVoidP,false,GTypeVoid,"call void @llvm.eh.sjlj.longjmp(i8* #1) #d\n"))

	RangeFuncs()
	Vec4fFuncs()

	if DebugMode
	{
		AddBuiltInFunc(new BuiltInFuncUno("assert",GTypeBool,false,GTypeVoid,
				"br i1 #1, label %Good##, label %Bad## #d\n"sbt +
				"Bad##:\n"+
				"call void @llvm.debugtrap() #d\n" +
				"br label %Good## #d\n" +
				"Good##:\n"))
	}else{
		preAdd := new BuiltInFuncUno("assert",GTypeBool,false,GTypeVoid, "")
		preAdd.IsSelfPre = true
		AddBuiltInFunc(preAdd)
	}
}

RangeFuncs := !() -> void
{
	intType := GetType("int")
	rangeType := TypeTable[13]
	AddBuiltInFunc( new BuiltInFuncBinar("..",intType,false,intType,false,rangeType,"%PreR## = insertvalue %RangeTypeInt undef,i32 #1, 0 #d\n"sbt 
												+"#0 = insertvalue %RangeTypeInt %PreR##,i32 #2,1 #d\n"))

	AddBuiltInFunc( new BuiltInFuncBinar("=",rangeType,true,rangeType,false,rangeType,"store %RangeTypeInt #2, %RangeTypeInt* #1 #d\n"))

}


