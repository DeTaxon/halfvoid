
baseGCContainer := class extend TGCObject
{
	dataBase := RBMap.{StringSpan,TCachedGCObject^}
	Contain := !(StringSpan name) -> bool
	{
		return dataBase.Contain(name)
	}
	Remove := !(StringSpan name) -> void
	{
		dataBase[name] = null
		charPtr := dataBase.Key(name).Get()
		dataBase.Remove(name)
		delete charPtr
	}
}
TGCContainer := class .{@V} extend baseGCContainer
{
	Create := !(StringSpan name) -> V^
	{
		result := new V
		result.Name = StrCopy(name)
		result.basePointer = this&
		dataBase[result.Name] = result
		return result
	}
}

TCachedGCObject := class extend TGCObject
{
	Name := StringSpan
	basePointer := baseGCContainer^
	GCLastRef := virtual !() -> void
	{
		basePointer.Remove(Name)
	}
}

Screamer := class extend TCachedGCObject
{
	Destroy := virtual !() -> void
	{
		printf("AAAAAAA\n")
	}
}


fnc := !() -> void
{
	x := u8[12]
	x[^] = 16
}
fnc2 := !() -> void
{
	//tst := CommonTreeNode.{BadPair.{char^,Screamer^}}^
	tstParam := Screamer^
	printf("wow %p\n",tstParam)
}
main := !(int argc, char^^ argv) -> int
{
	cnt := new TGCContainer.{Screamer}
	val := cnt.Create(StringSpan("wow"))
	return 0
}
