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
	"this" := !(Object^ item) -> void 
	{
		PopOutNode(item)
		MakeItBlock(item)
	}
	GetValue := virtual !() -> string
	{
		return "{..}"
	}
}

