BoxLabel := class extend Object
{
	PrintLabel := virtual !(sfile f) -> void
	{
	}
	GetLabel := virtual !() -> char^
	{
		return "problem"
	}
}
BoxLabelStr := class extend BoxLabel
{
	callStr :=char^

	this := !(char^ itStr) -> void
	{
		callStr = StrCopy(itStr)
	}
	PrintLabel := virtual !(sfile f) -> void
	{
		f << callStr << ":\n"
	}
	GetLabel := virtual !() -> char^
	{
		return callStr
	}
}

BoxLabelAnon := class extend BoxLabel
{
	itId := int

	this := !() -> void
	{
		itId = GetNewId()
	}
	PrintLabel := virtual !(sfile f) -> void
	{
		f << "label"<< itId << ":\n"
	}
	GetLabel := virtual !() -> char^
	{
		return "label"sbt + itId
	}
}

