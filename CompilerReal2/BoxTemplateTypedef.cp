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
	createdTemplates := AVLMap.{int,Type^}
	TryGetType := !(FuncInputBox itBox) -> Type^
	{
		itHash := itBox.GetConstsHash()
		//if createdTemplates[itHash][^].createdBy^ <=> itBox == 0
		//	return it
		newConsts := Queue.{ObjConstHolder^}()
		cmpRes := IsEqConsts(Down.Down.Down,itBox,newConsts)
		if cmpRes 
		{
			oldConsts := itConsts
			itConsts = newConsts&
			itT := ParseType(Down.Down.Right.Right.Right)
			itConsts = oldConsts
		}
		return null
	}
	itConsts := Queue.{ObjConstHolder^}^
	GetItem := virtual !(char^ itName) -> Object^
	{
		if itConsts == null
			return null
		if itConsts^[^].ItName == itName
		{
			printf("tes\n")
			return it.Down
		}
			printf("no\n")
		return null
	}
}
