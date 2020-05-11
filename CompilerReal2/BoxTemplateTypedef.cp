BoxTemplateTypedef := class extend Object
{
	
	this := !(Object^ toDw) -> void
	{
		SyntaxCompress(toDw.Down.Right.Right,PriorityData)
	
	}
	ApplyDown := !(Object^ toDw) -> void
	{
		Down = toDw.Down
		Down.SetUp(this&)
		toDw.Down = null
		PopOutNode(Down.Right)
		PopOutNode(Down.Right)

		itName := Down.Down->{ObjIndent^}.MyStr

		if Up? is BoxFile{
			asBF := Up->{BoxFile^}
			if asBF.cs != null
			{
				asBF.cs.codeTemplateTypedefs[itName].Push(this&)
			}
		}
	}
	createdTemplates := AVLMap.{int,List.{Tuple.{FuncInputBox^,Type^}}}
	TryGetType := !(FuncInputBox itBox) -> Type^
	{
		itHashEntry := ref createdTemplates[itBox.GetConstsHash()]
		if itHashEntry[^].0^ <=> itBox == 0
			return it.1
		newConsts := Queue.{ObjConstHolder^}()
		cmpRes := IsEqConsts(Down.Down.Right.Right,itBox,newConsts)
		if cmpRes 
		{
			oldConsts := itConsts
			itConsts = newConsts&
			itT := ParseType(Down.Right)
			itConsts = oldConsts
			itHashEntry.Emplace(itBox.Clone(),itT)
			return itT
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
			return it
		}
		return null
	}
}
