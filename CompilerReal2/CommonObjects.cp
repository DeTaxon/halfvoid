GetItem := !(string name, Object^ start) -> Object^
{
	 stuf := QueueSet.{int}()
	 return GetItem2(name,start,stuf)
}
GetItem2 := !(string name, Object^ start,QueueSet.{int} Searched) -> Object^
{
	iter := start

	while iter != null
	{
		if iter is ObjParam //iter.GetValue() == "i:=1"
		{
			AsNeed := iter->{ObjParam^}
			if (AsNeed.MyStr == name) {
				if (not AsNeed.AskedGetUse) and AsNeed.IsFunc WorkBag.Push(iter,State_GetUse)
				return iter
			}
		}else
		if iter is TypeDef //iter.GetValue() == ":=type"
		{
			asDef := iter->{TypeDef^}
			if asDef.ItName == name return iter
		}else
		if iter.GetValue() == "(const)"
		{
			asC := iter->{ConstItem^}
			if asC.Name == name return iter
		}else
		if iter is ImportCmd //iter.GetValue() == "#import cp"
		{
			asN := iter->{ImportCmd^}
			fl := asN.GetFile()
			res := fl.VisibleParams.TryFind(name)
			if res != null {
				return res^[0]
			}


			Fnd := Searched.Contain(fl.fileId)

			if not Fnd
			{
				Searched.Push(fl.fileId)
				notSure := fl.Down

				if notSure != null
				{
					while notSure.Right != null
						notSure = notSure.Right
				}

				res := GetItem2(name,notSure,Searched)
				if res != null return res
			}
		}else if iter.Up? is BoxClass and name[0] == '$'
		{
			asCl := iter.Up->{BoxClass^}
			resObj := asCl.ItAttrs.TryFind(name)
			if resObj != null
				return resObj^

		}


		if iter.Left != null iter = iter.Left 
		else
		{
			iter = iter.Up


			if iter != null
			{
				ItTp := iter.GetItem(name)
				if ItTp != null return ItTp

				if iter is BoxFuncBody //iter.GetValue() == "!()"
				{
					AsNeed2 := iter->{BoxFuncBody^}
					as2 := iter->{BoxFunc^}

					if AsNeed2.ItParams != null and as2.MyFuncParamNames != null
					for itNames : as2.MyFuncParamNames , itParrs : AsNeed2.ItParams
					{
						if itNames == name 
							return itParrs
					}
					if AsNeed2.ExtraRetParam != null
					{
						if AsNeed2.ExtraRetParam.ItName == name return AsNeed2.ExtraRetParam
					}
					trVal := AsNeed2.ItAttrs.TryFind(name)
					if trVal != null return trVal^
				}
			}
		}
	}
	for SomeDef : DefsTable
	{
		if SomeDef?.ItName == name 
		{
			return SomeDef->{Object^}
		}
	}

	for itCS : CodeSpaces ; $reverse
	{
		for itLib : itCS.codeLibs
		{
			res := itLib.VisibleParams.TryFind(name)
			if res != null return res^[0]
		}
	}
	for Modules {
		inMod := it.GetItem(name)
		if inMod != null return inMod
	}
	if GlobalDataBuiltins.Contain(name)
		return GlobalDataBuiltins[name]
	return null
}



ReplaceNode := !(Object^ what, Object^ with) -> Object^
{
	wiEnd := with

	while wiEnd.Right != null 
	{
		wiEnd.Up = what.Up
		wiEnd = wiEnd.Right
		if wiEnd.Line == null wiEnd.Line = what.Line
	}
	if wiEnd.Line == null wiEnd.Line = what.Line
	wiEnd.Up = what.Up

	if what.Left == null
	{
		if what.Up != null
		{
			what.Up.Down = with
		}
		with.Left = null
	}else{
		with.Left = what.Left
		with.Left.Right = with
	}

	wiEnd.Right = what.Right
	if wiEnd.Right != null wiEnd.Right.Left = wiEnd

	with.SetUp(what.Up)

	what.Up = null
	what.Left = null
	what.Right = null

	return with
}

PopOutNode := !(Object^ what) -> void
{
	if what.Left != null
	{
		what.Left.Right = what.Right
		if what.Right != null	what.Right.Left = what.Left
	}else
	{
		if what.Up != null what.Up.Down = what.Right
		if what.Right != null what.Right.Left = null
	}
	what.Up = null
	what.Left = null
	what.Right = null
}


SomeId := int
GetNewId := !() -> int
{
	SomeId += 1
	return SomeId
}

