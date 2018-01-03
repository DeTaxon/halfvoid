
GetItem := !(string name, Object^ start) -> Object^
{
	iter := start

	while iter != null
	{
		if iter.GetValue() == "i:=1"
		{
			AsNeed := iter->{ObjParam^}
			if (AsNeed->GetName() == name) { return iter}
		}
		if iter.Left != null iter = iter.Left else iter = iter.Up
	}
	
	for SomeDef : DefsTable
	{
		if SomeDef.ItName == name 
		{
			//printf("found %s\n", SomeDef.GetValue())
			return SomeDef->{Object^}
		}
	}
	
	return null
}

FindFunc := !(string name, Object^ start,Queue.{Type^} pars) -> Object^
{
	iter := start
	while iter != null
	{
		if iter.GetValue() == "i:=1"
		{
			AsNeed := iter->{ObjParam^}
			if(AsNeed->GetName() == name)
			{
				//TODO: check params
				return iter
			}
		}
		if iter.Left != null iter = iter.Left else iter = iter.Up
	}
	return null
}

ReplaceNode := !(Object^ what, Object^ with) -> void
{
	wiEnd := with

	while wiEnd.Right != null 
	{
		wiEnd.Up = what.Up
		wiEnd = wiEnd.Right
	}
	wiEnd.Up = what.Up

	if what.Left == null
	{
		with.Up.Down = with	
	}else{
		with.Left = what.Left
		with.Left.Right = with
	}

	wiEnd.Right = what.Right
	if wiEnd.Right != null wiEnd.Right.Left = wiEnd
}


SomeId := int
GetNewId := !() -> int
{
	SomeId += 1
	return SomeId
}

