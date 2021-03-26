
OneCLib := class
{
	jsonStart := jsonNode^

	initFunc := StringSpan
	dllNames := List.{StringSpan}

	consts := AVLMap.{StringSpan,jsonNode^}
	structs := AVLMap.{StringSpan,jsonNode^}
	funcs := AVLMap.{StringSpan,jsonNode^}
	vars := AVLMap.{StringSpan,jsonNode^}

	objs := AVLMap.{StringSpan,Object^}

	isStatic := bool

	initFuncObj := Object^
	initFuncBody := BoxFuncBodyFromString^

	itLib := Library^
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
				if it.isStatic 
					continue
				strData := "itLib := Library()\n"sbt

				strData << "itLib.Open(!["
				for toLd,i : it.dllNames
				{
					if i != 0 strData << ","
					strData << "\"" << toLd << "\""
				}
				strData << "])\n"

				for obj,key : it.objs
				{
					if obj is GlobalParam
					{
						asGl := obj->{GlobalParam^}
						if asGl.ResultType is TypePoint and asGl.ResultType.Base is TypeFunc
						{
							strData << key << "&->{void^^}^ = itLib.Get(\"" << key << "\")\n"
						}else{
							strData << key << "->ptr&->{void^^}^ = itLib.Get(\"" << key << "\")\n"
						}
					}
				}

				it.initFuncBody = new BoxFuncBodyFromString(it.initFunc.Str(),fType,strData)
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
			case "structs"
				for it : part^
				{
					newLib.structs[it.Key()] = it
				}
			case "consts"
				for it : part^
				{
					newLib.consts[it.Key()] = it
				}
			case "link"
				isStatic = part.GetStr() == "static"
			}
		}

		newLib.isStatic = isStatic
		libs.Push(newLib)
	}
	itTypes := AVLMap.{StringSpan,Type^}
	GetModuleType := virtual !(char^ name) -> Type^
	{
		spn := StringSpan(name)

		if spn in itTypes
			return itTypes[spn]

		try
		{
			for it : libs
			{
				if spn in it.structs
				{
					inStructs := ref it.structs[spn]
					nClass := new BoxClass(null,null,null)
					for field : inStructs^
					{
						fldType := CheckTypeString(field.GetStr())
						new FieldParam(field.Key().Str(),fldType,nClass)
					}
					itTypes[inStructs.Key()] = nClass.ClassType
					return nClass.ClassType
				}
			}
		}catch(IException^ e)
		{
		}
		return null
	}
	GetItem := virtual !(char^ name) -> Object^
	{
		spn := StringSpan(name)

		for it : libs
		{
			if spn in it.objs
				return it.objs[spn]
		}

		try
		{
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
					if not it.isStatic
					{
						inFuncs := ref it.funcs[spn]
						fType := CheckFuncTypeString(inFuncs.GetStr())

						resPar := new GlobalParam(fType.GetPoint(),null)
						it.objs[inFuncs.Key()] = resPar

						return resPar
					}
				}
			}
		}catch(IException^ e)
		{
			return null
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
			if spn in it.funcs
			{
				if it.isStatic
				{
					inFuncs := ref it.funcs[spn]
					fType := CheckFuncTypeString(inFuncs.GetStr())

					resPar := new BoxFuncDeclare(fType,spn.Str())
					it.objs[inFuncs.Key()] = resPar

					return resPar
				}
			}
		}
		return null
	}
	CheckFuncTypeString := !(StringSpan toCheck) -> TypeFunc^
	{
		tList := DivideStr(toCheck.StrTmp()," ,") //TODO: divide_str for string span
		retType := CheckTypeString(tList[0])
		nextTypes := Queue.{Type^}()
		isVArg := false
		for it,i : tList
		{
			if i == 0 continue
			if it == "..." 
			{
				isVArg = true
				continue
			}
			nextTypes.Push(CheckTypeString(it)) ; $temp
		}
		return GetFuncType(nextTypes,null,retType,false,isVArg)
	}
	CheckTypeString := !(StringSpan toCheck) -> Type^
	{
		i := 0
		while toCheck[i] != '^' and i < toCheck.Size() i+= 1
		baseType := GetType(toCheck[0..i].StrTmp())
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
	InitModule := virtual !() -> bool {
		return true
	}
	PrintGlobal := virtual !(TIOStream f) -> void
	{
		for it : libs[^].objs
		{
			if it is GlobalParam
				it.PrintGlobal(f)
			if it is BoxFuncDeclare
				it.PrintGlobal(f)
		}
		for lib : libs
		{
			if lib.isStatic
			{
				continue
			}
			lib.initFuncBody.PrintGlobal(f)
		}
	}
	GetPtrFunc := !(char^ name) -> void^
	{
		spn := StringSpan(name)
		for it : libs
		{
			if spn in it.funcs
			{
				if it.itLib == null
				{
					it.itLib = new Library
					loaded := false
					for dll : it.dllNames
					{
						if it.itLib.TryOpen(dll)
						{
							loaded = true
							break
						}
					}
					assert(loaded)
				}
				return it.itLib.Get(name)
			}
		}
		return null
	}
}
