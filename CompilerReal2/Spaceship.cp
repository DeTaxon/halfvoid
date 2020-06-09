
BuiltInFuncSpaceTransformer := class extend BuiltInFunc
{
	this := !(char^ oName) -> void
	{
		chng := ""sbt
		chng << "#0 = icmp "
		switch oName
		{
			case "<"  chng << " slt i32 #1, 0"
			case ">"  chng << " sgt i32 #1, 0"
			case "<=" chng << " sle i32 #1, 0"
			case ">=" chng << " sge i32 #1, 0"
			case "==" chng << " eq i32 #1, 0"
			case "!=" chng << " ne i32 #1, 0"
		}
		chng << " #d\n"
		ToExe = chng.Str()

		IsSuffix = false
		FuncName = "<=>"
		OutputName = "<=> transformer"

		PP := Queue.{Type^}() ; $temp
		PP.Push(GTypeInt)

		IsRefs := bool[1]
		IsRefs[0] = false
		MyFuncType = GetFuncType(PP,IsRefs,GTypeBool,false,false)
	}
}

spaceT := AVLMap.{char^,BoxFunc^}
GetSpaceTransformer := !(char^ operName) -> BoxFunc^
{
	if operName in spaceT
	{
		return spaceT[operName]
	}
	strCpy := StrCopy(operName)
	spaceT[strCpy] = new BuiltInFuncSpaceTransformer(strCpy)
	return spaceT[strCpy]
}

