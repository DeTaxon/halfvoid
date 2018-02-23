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

GetUpClass := !(Object^ toS) -> BoxClass^
{
	iterF := toS
	while iterF != null
	{
		if iterF.GetValue() == "{...}" return iterF->{BoxClass^}
		iterF = iterF.Up
	}
	return null
}

BoxClass := class extend Object
{
	ClassId := int
	Params := Queue.{FieldParam^}
	ClassType := TypeClass^
	"this" := !(Object^ item) -> void 
	{
		PopOutNode(item)
		Down = item
		Down.SetUp(this&)
		MakeItBlock(Down)
		Down.SetUp(this&)
		WorkBag.Push(Down,State_Start)

		ClassId = GetNewId()
		ClassType = new TypeClass(this&)
	}
	GetValue := virtual !() -> string
	{
		return "{...}"
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		if Params.Size() == 0
		{
			f << "%Class" << ClassId << " = type{i1}\n"
		}else{
			f << "%Class" << ClassId << " = type {"
			for Params.Size()
			{
				if it != 0 f <<","
				f << Params[it].ResultType.GetName()
			}
			f << "}\n"

		}

		//print class
	}
}

