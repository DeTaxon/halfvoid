BoxTemplateTypedef := class extend Object
{
	this := !(Object^ toDw) -> void
	{
		SyntaxCompress(toDw.Down.Right.Right,PriorityData)
	
	}
	ApplyDown := !(Object^ toDw) -> void
	{
		Down = toDw
		Down.SetUp(this&)

		itName := Down.Down.Down->{ObjIndent^}.MyStr

		if Up? is BoxFile{
			asBF := Up->{BoxFile^}
			if asBF.cs != null
			{
				asBF.cs.codeTemplateTypedefs[itName].Push(this&)
			}
		}
	}
	TryGetType := !(FuncInputBox itBox) -> Type^
	{
		return null
	}
}
