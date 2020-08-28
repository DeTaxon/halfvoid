CheckMetaBlock := !(Object^ metBl) -> void
{
	itrUp := metBl.Up
	while itrUp != null
	{
		if itrUp is BoxClass
		{
			if metBl.Up? is ObjParam
			{
				paramName := metBl.Up->{ObjParam^}.MyStr
				
				MakeItBlock(metBl.Down.Right)
				WorkBag.Push(metBl.Down.Right,State_Start)
				metBl.Down.Right->{BoxBlock^}.InClass = itrUp->{BoxClass^}
				itrUp->{BoxClass^}.AddMetaField(paramName,metBl.Down.Right)
				//SyntaxCompress(metBl.Down.Right,PriorityData)
			}
			itrUp = null
		}else{
			itrUp = itrUp.Up
		}
	}
}

MetaFieldBox := class extend Object
{
	
}
MetaItemWrapper := class extend Object
{
	ptrToBlock := Object^

	PrintInBlock := virtual !(sfile f) -> void { Down.PrintInBlock(f) }
	PrintPointPre := virtual !(sfile f) -> void { Down.PrintPointPre(f) }
	PrintPointUse := virtual !(sfile f) -> void { Down.PrintPointUse(f) }
	PrintPre := virtual !(sfile f) -> void { Down.PrintPre(f) }
	PrintUse := virtual !(sfile f) -> void { Down.PrintUse(f) }
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
}
