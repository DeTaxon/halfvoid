
BoxClassTemplate := class extend Object
{
	ItClasses := AVLMap.{int,Stack.{BoxClass^}}
	ClassTree := Object^
	ConstTree := Object^
	ExtendTree := Object^

	TempConsts := Queue.{ObjConstHolder^}^

	relAttrs := QueueSet.{string}

	this := !(Object^ tree,Object^ txt) -> void
	{
		ClassTree = tree.Clone()
		ClassTree.SetUp(this&)

		ConstTree = ClassTree.Down.Right.Right

		PopOutNode(ClassTree.Down.Right)		
		PopOutNode(ClassTree.Down.Right)
		ConstTree.SetUp(this&)

		if ClassTree.Down.Right.GetValue() == "extend"
		{
			ExtendTree = ClassTree.Down.Right.Right.Clone()
			ExtendTree.Up = this&

			PopOutNode(ClassTree.Down.Right)
			PopOutNode(ClassTree.Down.Right)
		}

		SyntaxCompress(ConstTree,PriorityData)
		MakeGoodConsts(ConstTree)

		for ite : ClassTree.Down.Right.Down
		{
			if ite.GetValue() == "~ind"
			{
				asInd := ite->{ObjIndent^}
				if asInd.MyStr[0] == '$'
					relAttrs.Push(asInd.MyStr)
			}
		}
	}
	GetItem := virtual !(string name) -> Object^
	{
		if TempConsts != null
		{
			if TempConsts^[^].ItName == name 
				return it.Down
		}
		return null
	}
	GetClass := !(FuncInputBox itBox) -> TypeClass^
	{
		itHash := itBox.GetConstsHash()
		if ItClasses[itHash][^].IsSameConsts(itBox)
			return it.ClassType

		newConsts := Queue.{ObjConstHolder^}()
		if not IsEqConsts(ConstTree,itBox,newConsts)
		{
			return null
		}

		newTree := ClassTree.Clone()
		newTree.Up = this&
		
		inher := BoxClass^()

		treeIter := newTree.Down.Right

		if ExtendTree != null
		{
			TempConsts = newConsts&
			inher = ParseType(ExtendTree,itBox.itAttrs&)->{BoxClass^}
			TempConsts = null
			if inher == null return null
			inher = ((inher->{TypeClass^}).ToClass)
		}
		
		newEx := Object^
		newEx = null
		if ExtendTree != null{
			newEx = ExtendTree.Clone()
		}
		newClass := new BoxClass(treeIter,inher,newEx)
		newClass.Up = this&
		if newEx != null{
			newEx.Up = newClass
		}

		newClass.Right = Down
		if Down != null Down.Left = newClass
		Down = newClass
	

		newClass.ItConsts.Push(itBox.itConsts[^])
		newClass.ItVals.Push(newConsts[^])
		
		templs := Queue.{QueueSet.{string}^}() ; $temp
		templs.Push(this.relAttrs&)

		clIter := inher
		while clIter != null
		{
			if clIter.Up?.GetValue() == "!{}{...}"
			{
				asTmpl := clIter.Up->{BoxClassTemplate^}
				templs.Push(asTmpl.relAttrs&)
			}
			clIter = clIter.Parent
		}
		
		
		for itr,itrK : itBox.itAttrs
		{
			for templs
			{
				if it.Contain(itrK)
				{
					newClass.ItAttrs[StrCopy(itrK)] = itr
				}
			}
		}

		ItClasses[itHash].Push(newClass)
		return newClass.ClassType
	}
	GetValue := virtual !() -> string
	{
		return "!{}{...}"
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
	}
}
