
OneCLib := class
{
	jsonStart := jsonNode^

	initFunc := StringSpan
	dllNames := List.{StringSpan}

	consts := AVLMap.{StringSpan,jsonNode^}
	funcs := AVLMap.{StringSpan,jsonNode^}
	vars := AVLMap.{StringSpan,jsonNode^}

	objs := AVLMap.{StringSpan,Object^}

	isStatic := bool

	initFuncObj := Object^
	initFuncBody := BoxFuncBodyFromString^
}

CLibModule := class extend CompilerModule
{
	libs := List.{OneCLib^}


	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_CLib
		{
			fType := CheckFuncTypeString(StringSpan("void"))
			for it : libs
			{
				strData := "itLib := Library\n"sbt

				strData << "itLib.Open("
				for toLd,i : it.dllNames
				{
					if i != 0 strData << ","
					strData << "\"" << toLd << "\""
				}
				strData << ")\n"

				for obj,key : it.objs
				{
					if obj is GlobalParam
					{
						asGl := obj->{GlobalParam^}
						if asGl.ResultType is TypePoint and asGl.ResultType.Base is TypeFunc
						{
						strData << key << "&->{void^^}^ = itLib.Get(\"" << key << "\")\n"
						}else{
						}
					}
				}

				it.initFuncBody = new BoxFuncBodyFromString(it.initFunc,fType,strData)
			}
		}
	}

	addedWork := bool
	AddCLib := !(vRepoFile^ libFile,bool isStatic) -> void
	{
		if not addedWork
		{
			addedWork = true
			WorkBag.Push(this&,State_CLib)
		}

		ptr := libFile.Map()

		newLib := new OneCLib
		newjson := new json
		on_exception delete newLib
		on_exception delete newjson
		
		newLib.jsonStart = newjson

		newjson.ParseString(ptr,libFile.Size())

		for part : newjson^
		{
			switch part.Key()
			{
			case "initname"
				newLib.initFunc = part.GetStr()
			case "libs"
				for it : part^
				{
					newLib.dllNames.Push(it.GetStr())
				}
			case "funcs"
				for it : part^
				{
					newLib.funcs[it.Key()] = it
				}
			case "vars"
				for it : part^
				{
					newLib.vars[it.Key()] = it
				}
			case "consts"
				for it : part^
				{
					newLib.consts[it.Key()] = it
				}
			}
		}

		newLib.isStatic = isStatic
		libs.Push(newLib)
	}
	GetItem := virtual !(char^ name) -> Object^
	{
		spn := StringSpan(name)

		for it : libs
		{
			if spn in it.objs
				return it.objs[spn]
		}

		for it : libs
		{
			if spn in it.vars
			{
				inVars := ref it.vars[spn]
				itType := CheckTypeString(inVars.GetStr())
				resPar := new GlobalParam(itType,null)
				resPar.IsRef = true
				it.objs[inVars.Key()] = resPar

				return resPar
			}
			if spn in it.consts
			{
				inCnsts := ref it.consts[spn]
				itInt := new ObjInt(inCnsts.GetInt())
				it.objs[inCnsts.Key()] = itInt
				return itInt
			}
			if spn in it.funcs
			{
				inFuncs := ref it.funcs[spn]
				fType := CheckFuncTypeString(inFuncs.GetStr())
				resPar := new GlobalParam(fType.GetPoint(),null)
				it.objs[inFuncs.Key()] = resPar

				return resPar
			}
		}

		return null
	}
	GetFunc := virtual !(char^ name) -> Object^
	{
		spn := StringSpan(name)
		for it : libs
		{
			if spn == it.initFunc
			{
				if it.initFuncObj != null
					return it.initFuncObj
				it.initFuncObj = new BuiltInFuncZero("",GTypeVoid,false,
					"call void@"sbt + it.initFunc + "()\n")
				return it.initFuncObj
			}
		}
		return null
	}
	CheckFuncTypeString := !(StringSpan toCheck) -> Type^
	{
		tList := DivideStr(toCheck," ")
		retType := CheckTypeString(tList[0])
		nextTypes := Queue.{Type^}()
		for it,i : tList
		{
			if i == 0 continue
			nextTypes.Push(CheckTypeString(it))
		}
		return GetFuncType(nextTypes,null,retType,false,false)
	}
	CheckTypeString := !(StringSpan toCheck) -> Type^
	{
		i := 0
		while toCheck[i] != '^' and i < toCheck.Size() i+= 1
		baseType := GetType(toCheck[0..i])
		if baseType == null throw new Exception("Type not found")
		while i < toCheck.Size() {
			switch toCheck[i]
			{
			case '^'
				baseType = baseType.GetPoint()
			case void
				throw new Exception("Unknown symbol")
			}
			i += 1
		}
		return baseType
	}
	GetModuleType := virtual !(char^ name) -> Type^
	{
		return null
	}
	InitModule := virtual !() -> bool {
		return true
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		for it : libs[^].objs
		{
			if it is GlobalParam
				it.PrintGlobal(f)
		}
		for lib : libs
		{
			if lib.isStatic
				continue
			lib.initFuncBody.PrintGlobal(f)
		}
	}
}
