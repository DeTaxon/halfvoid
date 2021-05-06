

createdFuncNames := AVLSet.{char^}

mangledInit := !() -> void
{
    createdFuncNames.Insert("main")
    createdFuncNames.Insert("operator")
}

mnglIter := 0
GetFuncMangledName := !(char^ funcName, TypeFunc^ fType) -> char^
{
    if not IsWord(funcName)
        funcName = "operator"

    //if createdFuncNames.Contain(funcName)
    //{
        newName := ""sbt + funcName + "_mngl" + mnglIter <-
        mnglIter += 1
        createdFuncNames.Insert(newName)
        return newName
    //}
    //cpStr := StrCopy(funcName)
    //createdFuncNames.Insert(cpStr)
    //return cpStr
}