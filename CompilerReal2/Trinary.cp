TrinaryOper := class extend Object
{
	itId  := int
	isItRef := bool
	this := !(Object^ strt,Type^ itType) -> void
	{
		itId = GetNewId()
		oldUp := strt.Up
		Down = strt
		Down.SetUp(this&)
		assert(oldUp.Down == strt)
		oldUp.Down = null
		PopOutNode(Down.Right)
		PopOutNode(Down.Right.Right)

		isItRef = false
		if Down.Right.IsRef() and Down.Right.Right.IsRef()
		{
			if Down.Right.GetType() == Down.Right.Right.GetType()
			{
				isItRef = true
			}
		}

		if Down.Right.GetType() != itType
		{
			BoxExc(Down.Right,itType,isItRef)
		}
		if Down.Right.Right.GetType() != itType
		{
			BoxExc(Down.Right.Right,itType,isItRef)
		}
	}
	GetValue := virtual !() -> char^
	{
		return "?: :"
	}
	IsRef := virtual !() -> bool
	{
		return isItRef
	}
	PrintPointPre := virtual !(TIOStream f) -> void
	{
		Down.PrintPre(f)
		f << "br i1 " << Down.GetName() << ", label %OnTrue" << itId << " , label %OnFalse" << itId << "\n"
		f << "OnTrue" << itId << ":\n"
		Down.Right.PrintPointPre(f)
		f << "br label %OnEnd" << itId << "\n"
		f << "OnFalse" << itId << ":\n"
		Down.Right.Right.PrintPointPre(f)
		f << "br label %OnEnd" << itId << "\n"

		f << "OnEnd" << itId << ":\n"
		f << "%TriRes"<< itId <<" = phi " << Down.Right.GetType().GetName() << "* "
		f << "[" << Down.Right.GetPointName() << ",%OnTrue" << itId <<"],"
		f << "[" << Down.Right.Right.GetPointName() << ",%OnFalse" << itId << "]\n"
	}
	PrintPointUse := virtual !(TIOStream f) -> void
	{
		f << Down.Right.GetType().GetName() 
		f << "* %TriRes" << itId 
	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		Down.PrintPre(f)
		f << "br i1 " << Down.GetName() << ", label %OnTrue" << itId << " , label %OnFalse" << itId << "\n"
		f << "OnTrue" << itId << ":\n"
		Down.Right.PrintPre(f)
		f << "br label %OnEnd" << itId << "\n"
		f << "OnFalse" << itId << ":\n"
		Down.Right.Right.PrintPre(f)
		f << "br label %OnEnd" << itId << "\n"

		f << "OnEnd" << itId << ":\n"
		f << "%TriRes"<< itId <<" = phi " << Down.Right.GetType().GetName() << " "
		f << "[" << Down.Right.GetName() << ",%OnTrue" << itId <<"],"
		f << "[" << Down.Right.Right.GetName() << ",%OnFalse" << itId << "]\n"
	}
	PrintUse := virtual !(TIOStream f) -> void
	{
		f << Down.Right.GetType().GetName() 
		f << " %TriRes" << itId 
	}
	GetName := virtual !() -> char^
	{
		return ""sbt + "%TriRes" + itId
	}

	GetType := virtual !() -> Type^
	{
		return Down.Right.GetType()
	}
	DoTheWork :=  virtual !(int pri) -> void
	{
	}
}
