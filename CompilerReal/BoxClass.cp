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
	UnrollTemplate := BuiltInTemplateUnroll^
	AutoFieldTemplate := BuiltInTemplateAutoField^
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
		UnrollTemplate = new BuiltInTemplateUnroll(this&)
		AutoFieldTemplate = new BuiltInTemplateAutoField(this&)
	}
	GetValue := virtual !() -> string
	{
		return "{...}"
	}
	GetFunc := !(string name,Queue.{Type^} pars) -> BoxFunc^
	{
		temp := Queue.{Object^}()
		return this.GetFunc(name,pars,temp)
	}
	GetFunc := !(string name,Queue.{Type^} pars, Queue.{Object^} consts) -> BoxFunc^
	{
		Funcs := Queue.{BoxFunc^}()
		Templs := Queue.{BoxTemplate^}()

		iterJ := Down.Down
		while iterJ != null
		{
			if iterJ.GetValue() == "i:=1"
			{
				itName := ((iterJ->{ObjParam^}).MyStr)
				if itName == name
				{
					if iterJ.Down.GetValue() == "!()" and consts.Size() == 0
						Funcs.Push((iterJ.Down)->{BoxFunc^})
					if iterJ.Down.GetValue() == "!(){}"
						Templs.Push((iterJ.Down)->{BoxTemplate^})
				}
			}
			iterJ = iterJ.Right
		}
		bestFunc := GetBestFunc(pars,Funcs,Templs)
		printf("using %p\n",bestFunc)
		if bestFunc != null WorkBag.Push(bestFunc,State_GetUse)
		return bestFunc
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
		iterJ := Down.Down
		while iterJ != null
		{
			if iterJ.GetValue() == "i:=1"
			{
				if iterJ.Down.GetValue() == "!()"
				{
					iterJ.PrintGlobal(f)
				}
			}
			iterJ = iterJ.Right
		}

		//print class
	}
}

