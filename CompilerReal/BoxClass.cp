ParseClass := virtual !(Object^ ob)-> BoxClass^
{
	if ob == null return null

	iterT := ob.Down
	if iterT == null return null
	if iterT.GetValue() != "class" return null

	iterT = iterT.Right

	if iterT == null return null
	if iterT.GetValue() != "{}" return null

	return new BoxClass(iterT)
}

BoxClass := class extend Object
{
	ClassId := int
	"this" := !(Object^ item) -> void 
	{
		PopOutNode(item)
		Down = item
		Down.SetUp(this&)
		MakeItBlock(Down)
		Down.SetUp(this&)
		WorkBag.Push(Down,State_Start)

		ClassId = GetNewId()
	}
	GetValue := virtual !() -> string
	{
		return "{..}"
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		//print class
	}
}

