

createdFuncNames := AVLSet.{char^}

mangledInit := !() -> void
{
    createdFuncNames.Insert("main")
    createdFuncNames.Insert("operator")
}

IsMangleKeepWord := !(string name) -> bool
{
	for c : name
	{
		switch c
		{
			case 'a'..'z':
			case 'A'..'Z':
			case '_':
			case '0'..'9':
			case void 
				return false
		}
	}
	return true
}

mnglIter := 0
GetFuncMangledName := !(char^ funcName, TypeFunc^ fType) -> char^
{
    if not IsMangleKeepWord(funcName)
        funcName = "operator"

    //if createdFuncNames.Contain(funcName)
    //{
        newName := ""sbt + funcName + "_mngl" + mnglIter <-
        mnglIter += 1
		newName = Copy(newName) //TODO: bug, $temp
        createdFuncNames.Insert(newName)
        return newName
    //}
    //cpStr := StrCopy(funcName)
    //createdFuncNames.Insert(cpStr)
    //return cpStr
}
