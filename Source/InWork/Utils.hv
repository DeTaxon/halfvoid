CollectConsts := !(Object^ obj , FuncInputBox^ itBox) -> void
{
	for H : obj.Down
	{
		if H.GetValue() != ","
		{
			if H.IsConst
			{
				itBox.itConsts.Push(H) ; $temp
			}else{
				tp := ParseType(H)
				if tp == null
				{
					ErrorLog.Push("can not parse type in .{}\n")
				}else{
					constType := new ObjType(tp)
					itBox.itConsts.Push(constType) ; $temp
				}
			}
		}
	}
}
