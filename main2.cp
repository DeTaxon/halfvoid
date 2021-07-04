
baseGCContainer := class extend TGCObject
{
	dataBase := RBMap.{StringSpan,TCachedGCObject^}
	Contain := !(StringSpan name) -> bool
	{
		return dataBase.Contain(name)
	}
	Remove := !(StringSpan name) -> void
	{
		spn := dataBase.Key(name)
		dataBase.Remove(spn)
		delete spn.Get()
	}
}
TGCContainer := class .{@V} extend baseGCContainer
{
	Create := !(StringSpan name) -> V^
	{
		result := new V
		result.Name = name.Str()
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
		if basePointer != null
			basePointer.Remove(Name)
	}
}

Screamer := class extend TCachedGCObject
{
	Destroy := virtual !() -> void
	{
		printf("AAAAAAA %p\n",this&)
	}
}


miniClass := class
{
	x := int
}

fnc := !() -> void
{
	x := u8[12]
	x[^] = 16
}
fnc2 := !() -> void
{
	//tst := CommonTreeNode.{BadPair.{char^,Screamer^}}^
	tstParam := new Screamer
	printf("wow %p\n",tstParam)
	throw new Exception("wow")
}

cls1 := class
{
	CreateObject := !(!(int)&->int ww) -> int
	{
		return ww(3)
	}
	Func2 := !() -> void
	{
		UniqName := CreateObject(x ==> x*23)
	}
}
main := !(int argc, char^^ argv) -> int
{
	c := cls1
	c.Func2()
	return 0
	try
	{
		fnc()
		TSleep(1)
		fnc2()
	}catch(IException^ e)
	{
		printf("exception\n")
	}
	//cnt := new TGCContainer.{Screamer}
	//val := cnt.Create(StringSpan("wow"))
	return 0
}
