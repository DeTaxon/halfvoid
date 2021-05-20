CheckMetaBlock := !(Object^ metBl) -> void
{
	itrUp := GetUpClass(metBl.Up)

	if itrUp == null
		return void

	if metBl.Up? is ObjParam
	{
		paramName := metBl.Up->{ObjParam^}.MyStr
	
		blkItem := metBl.Down.Right //TODO:MEMORY
		metWrp := ReplaceNode(metBl,new MetaFieldBox(paramName))->{MetaFieldBox^}
		metWrp.Down = blkItem
		blkItem.Up = metWrp
		blkItem.Left = null

		MakeItBlock(metWrp.Down)
		WorkBag.Push(metWrp.Down,State_Start)

		metWrp.Down->{BoxBlock^}.InClass = true //itrUp->{BoxClass^}
		assert(metWrp is MetaFieldBox)
		itrUp->{BoxClass^}.AddMetaField(paramName,metWrp)
	}
}

BoxClassFuncsHolder := class
{
	methods := AVLMap.{char^,List.{BoxFunc^}}
	templates := AVLMap.{char^,List.{BoxTemplate^}}
}

GetBoxClassFuncsHolder := !(Object^ start) -> BoxClassFuncsHolder^
{
	itr := start
	while itr != null and not (itr is MetaFieldBox)
		itr = itr.Up
	if itr != null
	{
		return itr->{MetaFieldBox^}.ptrToHolder
	}
	cls := GetUpClass(start)
	if cls != null
	{
		return cls.defaultFuncsHolder&
	}
	return null
}

MetaFieldBox := class extend Object
{
	className := char^
	ptrToHolder := BoxClassFuncsHolder^
	this := !(char^ clName) -> void
	{
		className = clName
		WorkBag.Push(this&,State_Start)
	}
	GetValue := virtual !() -> char^
	{
		return "~fake"
	}
	PrintGlobal := virtual !(TIOStream f) -> void
	{
		if Down != null for iterJ : Down.Down
		{
			if iterJ.GetValue() == "i:=1"
			{
				if iterJ.Down.GetValue() == "!()"
				{
					asFunc := iterJ.Down->{BoxFunc^}
					if not asFunc.IsAssembler()
						iterJ.PrintGlobal(f)
				}
				if iterJ.Down.GetValue() == "!(){}"
				{
					iterJ.PrintGlobal(f)
				}
			}
			if iterJ is MetaFieldBox
			{
				iterJ.PrintGlobal(f)
			}
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			names := List.{char^}() ; $temp
			itr := this&->{Object^}
			while itr != null
			{
				if itr is MetaFieldBox
				{
					asMt := itr->{MetaFieldBox^}
					names.PushFront(asMt.className)
				}else if IsClassObj(itr)
				{

					nms := ""sbt
					for it,i : names
					{
						if i != 0 nms << "."
						nms << it
					}
					nm := nms.Str()
					cls := GetClassObj(itr)
					assert(cls != null)
					ptrToHolder = cls.funcsHolders[nm]&	
					break
				}
				itr = itr.Up
				assert(itr != null)
			}
		}
	}
}
MetaItemWrapper := class extend Object
{
	ptrToBlock := BoxClassFuncsHolder^

	PrintInBlock := virtual !(TIOStream f) -> void { Down.PrintInBlock(f) }
	PrintPointPre := virtual !(TIOStream f) -> void { Down.PrintPointPre(f) }
	PrintPointUse := virtual !(TIOStream f) -> void { Down.PrintPointUse(f) }
	PrintPre := virtual !(TIOStream f) -> void { Down.PrintPre(f) }
	PrintUse := virtual !(TIOStream f) -> void { Down.PrintUse(f) }
	GetName := virtual !() -> char^ { return Down.GetName() }
	GetPointName := virtual !() -> string { return Down.GetPointName() }
	GetItem := virtual !(char^ name) -> Object^ {	return Down.GetItem(name)}
	GetType := virtual !() -> Type^	{ return Down.GetType()	}
	Clone := virtual !() -> Object^
	{
		toRet := new MetaItemWrapper
		toRet.Down = Down.Clone()
		toRet.ptrToBlock = ptrToBlock
		return toRet
	}
	GetValue := virtual !() -> char^
	{
		return "(fake)"
	}
}
