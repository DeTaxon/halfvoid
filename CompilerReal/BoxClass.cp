ParseClass := virtual !(Object^ ob)-> BoxClass^
{

	if ob == null return null

	iterT := ob.Down
	if iterT == null return null
	if iterT.GetValue() != "class" return null

	iterT = iterT.Right

	if iterT == null return null
	
	if iterT.GetValue() == "extend"
	{
		iterT = iterT.Right
		if iterT == null return null

		extType := ParseType(iterT)

		if extType == null return null
		if extType.GetType() != "class" return null

		asClass := extType->{TypeClass^}

		iterT = iterT.Right
		if iterT == null return null
		if iterT.GetValue() != "{}" return null

		return new BoxClass(iterT,asClass.ToClass)

	}

	if iterT.GetValue() != "{}" return null

	return new BoxClass(iterT,null->{BoxClass^})
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
	Parent := BoxClass^

	"this" := !(Object^ item, BoxClass^ par) -> void 
	{
		PopOutNode(item)
		Down = item
		Down.SetUp(this&)
		MakeItBlock(Down)
		Down.SetUp(this&)
		WorkBag.Push(Down,State_Start)
		WorkBag.Push(this&,State_GetUse)

		ClassId = GetNewId()
		ClassType = new TypeClass(this&)
		UnrollTemplate = new BuiltInTemplateUnroll(this&)
		AutoFieldTemplate = new BuiltInTemplateAutoField(this&)

		Parent = par
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_GetUse and Parent != null
		{
			Size := Parent.Params.Size()
			for i : Size
			{
				Params.PushFront(Parent.Params[Size - i - 1])
			}
		}
	}
	GetValue := virtual !() -> string
	{
		return "{...}"
	}
	//GetMethod
	GetFunc := !(string name,Queue.{Type^} pars) -> BoxFunc^
	{
		temp := Queue.{Object^}()
		return this.GetFunc(name,pars,temp)
	}
	//GetMethod
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
		if bestFunc != null WorkBag.Push(bestFunc,State_GetUse)

		if bestFunc == null and Parent != null
		{
			return Parent.GetFunc(name,pars,consts)
		}

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

