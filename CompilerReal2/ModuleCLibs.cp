
OneCLib := class
{
	jsonStart := jsonNode^

	initFunc := StringSpan

	consts := AVLMap.{StringSpan,jsonNode^}
	funcs := AVLMap.{StringSpan,jsonNode^}
	vars := AVLMap.{StringSpan,jsonNode^}

	isStatic := bool

	initFuncObj := Object^
}

CLibModule := class extend CompilerModule
{
	libs := List.{OneCLib^}
	objs := AVLMap.{StringSpan,Object^}
	AddCLib := !(vRepoFile^ libFile,bool isStatic) -> void
	{
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
		if spn in objs
			return objs[spn]

		for it : libs
		{
			if spn in it.consts
			{
				inCnsts := ref it.consts[spn]
				itInt := new ObjInt(inCnsts.GetInt())
				objs[inCnsts.Key()] = itInt
				return itInt
			}
			if spn in it.funcs
			{
				inFuncs := ref it.funcs[spn]
				tList := DivideStr(inFuncs.GetStr()," ")
				retType := CheckTypeString(tList[0])
				nextTypes := Queue.{Type^}()
				for it,i : tList
				{
					if i == 0 continue
					nextTypes.Push(CheckTypeString(it))
				}
				fType := GetFuncType(nextTypes,null,retType,false,false)
				resPar := new GlobalParam(fType.GetPoint(),null)
				objs[inFuncs.Key()] = resPar

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
		for it : objs
		{
			if it is GlobalParam
				it.PrintGlobal(f)
		}
		for lib : libs
		{
			if lib.isStatic
				continue
			f << "define void @" << lib.initFunc << "()\n"
			f << "{\n"
			f << "ret void\n"
			f << "}\n"
		}
	}
}
