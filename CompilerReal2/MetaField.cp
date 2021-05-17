CheckMetaBlock := !(Object^ metBl) -> void
{
	itrUp := GetUpClass(metBl.Up)

	if itrUp == null
		return void

	if metBl.Up? is ObjParam
	{
		paramName := metBl.Up->{ObjParam^}.MyStr
	
		blkItem := metBl.Down.Right //MEMORY
		metWrp := ReplaceNode(metBl,new MetaFieldBox)->{MetaFieldBox^}
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

MetaFieldBox := class extend Object
{
	GetValue := virtual !() -> char^
	{
		return "~fake"
	}
}
MetaItemWrapper := class extend Object
{
	ptrToBlock := Object^

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
