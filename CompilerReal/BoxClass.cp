ParseClass := virtual !(Object^ ob)-> BoxClass^
{

	IsTemplate := false
	
	if ob == null return null

	iterT := ob.Down
	if iterT == null return null
	if iterT.GetValue() != "class" return null

	iterT = iterT.Right

	if iterT == null return null

	if iterT.GetValue() == "!"
	{
		iterT = iterT.Right.Right
		IsTemplate = true
	}
	
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

		if IsTemplate return new BoxClassTemplate(ob)
		return new BoxClass(iterT,asClass.ToClass)

	}

	if iterT.GetValue() != "{}" return null

	if IsTemplate return new BoxClassTemplate(ob)
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

BoxClassTemplate := class extend Object
{
	Classes := Stack.{BoxClass^}
	ClassesObjs := Stack.{Object^^}()
	Names := Queue.{string}
	ClassTree := Object^

	this := !(Object^ tree) -> void
	{
		ClassTree = tree.Clone()

		iterY := ClassTree.Down.Right.Right.Down

		while iterY != null
		{
			if iterY.GetValue() == "~ind"
			{
				Names.Push(iterY->{ObjIndent^}.MyStr)
			}
			iterY = iterY.Right
		}

		PopOutNode(ClassTree.Down.Right)		
		PopOutNode(ClassTree.Down.Right)		
	}

	GetClass := !(Queue.{Object^} stuf) -> TypeClass^
	{
		if stuf.Size() != Names.Size() return null

		for i : Classes.Size()
		{	
			coud := true
			for j : stuf.Size()
			{
				if not CmpConstObjs(stuf[j],ClassesObjs[i][j])
					coud = false
			}
			if coud return Classes[i].ClassType
		}

		cont := new BoxBlock()

		cont.Right = Down
		cont.Up = this&
		if Down != null Down.Left = cont
		Down = cont

		for i : stuf.Size()
		{
			if stuf[i].GetValue() == "~type"
			{
				def := new TypeDef(Names[i],((stuf[i])->{ObjType^}).MyType)
				def.Right = cont.Down
				def.Up = cont
				if def.Right != null cont.Down.Left = def
				cont.Down = def
			}
			if stuf[i].IsConst()
			{
				toAd := new ConstItem(Names[i],stuf[i])
				toAd.Right = cont.Down
				toAd.Up = cont
				if toAd.Right != null cont.Down.Left = toAd
				cont.Down = toAd
			}
		}

		iterR := cont.Down
		while iterR.Right != null
		{
			iterR = iterR.Right
		}
		
		newTree := ClassTree.Clone()
		
		inher := BoxClass^
		inher = null

		iterR.Right = newTree
		iterR.Right.Left = iterR
		iterR.Right.Up = iterR.Up

		treeIter := newTree.Down.Right

		if treeIter.GetValue() == "extend"
		{
			inher = ParseType(treeIter.Right)
			if inher == null return null
			inher = ((inher->{TypeClass^}).ToClass)

			treeIter = treeIter.Right.Right
		}

		newClass := new BoxClass(treeIter,inher)
		iterR.Right = newClass
		iterR.Right.Left = iterR
		iterR.Right.Up = iterR.Up

		Classes.Push(newClass)
		ClassesObjs.Push(stuf.ToArray())
		return newClass.ClassType
	}
	GetValue := virtual !() -> string
	{
		return "!{}{...}"
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
	}
}

FuncItem := class
{
	fName := string
	fType := TypeFunc^
	fItem := BoxFunc^

	CmpItem := !(FuncItem^ Ri) -> bool
	{	
		if fName != Ri return false
		if fType != Ri.fType return false
		return true
	}
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

	vTypes := Queue.{FuncItem^}
	vTable := Queue.{FuncItem^}

	computedVTable := bool
	ComputeVTable := !() -> void
	{	
		if not computedVTable
		{
			computedVTable = true

			if Parent != null
			{
				Parent.ComputeVTable()
				for i : Parent.vTable.Size()
				{
					vTable.Push(Parent.vTable[i])
				}
			}

			for i : vTypes.Size()
			{
				found := false
				for j : vTable.Size()
				{
					if vTable[j].CmpItem(vTypes[i])
					{
						vTable[j] = vTypes[i]
						found = true
					}
				}
				if not found vTable.Push(vTypes[i])
			}
		}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		ComputeVTable()		
		if not vTable.Empty()
		{
			f << "%ClassTableType" << ClassId << " = type {"
			for vTable.Size()
			{
				if it > 1 f << " , "
				f << vTable[it].fType.GetName()
			}
			f << " }\n"
		}
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
	PutVirtualFunc := virtual !(string fNam,TypeFunc^ fTyo,BoxFunc^ fF) -> void
	{
		newItem := new FuncItem
		newItem.fName = fNam
		newItem.fType = fTyo
		newItem.fItem = fF

		vTypes.Push(newItem)
	}

}

