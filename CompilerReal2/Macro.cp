#import "Tree.cp"

TryParseMacro := !(Object^ tr) -> Object^
{
	return TryParseMacro(tr,tr)
}

TryParseMacro := !(Object^ tr ,Object^ itUp) -> Object^
{
	if tr == null return null
	if tr.GetValue() == "{}" return null
	if tr.Down == null return null
	
	itr := tr.Down
	while itr != null
	{
		mbRes := TryParseMacro(itr,itUp)
		if mbRes != null return mbRes
		itr = itr.Right
	}

	if tr.Down != null and tr.Down.GetValue() == "~ind" and tr.Down.Right != null and tr.Down.Right.GetValue() == "..."
	{
		itNam := (tr.Down->{ObjIndent^}).MyStr
		ups := itUp
		while ups != null
		{
			if ups is BoxFuncBody
			{
				asNN := ups->{BoxFuncBody^}
				if asNN.vargsName == itNam
					break
			}
			ups = ups.Up
		}
		if ups == null return null

		asBF := ups->{BoxFuncBody^}

		calls := Queue.{Object^}()

		for i : asBF.funcUserParamsCount .. (asBF.MyFuncType.ParsCount - 1)
		{
			calls.Push(new ParamNaturalCall("",asBF.ItParams[i]))
		}
		if calls.Size() == 0{
			PopOutNode(tr)
		}else{
			iitr := calls[0]
			ReplaceNode(tr,calls[0])

			for it, i : calls
			{
				if i == 0 continue
				it.Left = iitr
				it.Right = iitr.Right
				it.Line = iitr.Line
				it.Up = iitr.Up
				iitr.Right = it
				iitr = it
			}
		}

		return itUp
	}

	if tr.Down.Right == null return null
	if tr.Down.Right.GetValue() != "[]" return null
	if tr.Down.Right.Down == null return null
	if tr.Down.Right.Down.GetValue() != "^" return null


	indName := null->{string}
	itName := "_"
	deSkob := tr.Down.Right.Down.Right
	if deSkob != null and deSkob is ObjIndent
	{
		asNeed := deSkob->{ObjIndent^}
		indName = asNeed.MyStr
		
		if deSkob.Right != null and deSkob.Right.GetValue() == ","
		{
			deSkob = deSkob.Right.Right
			if deSkob != null and deSkob is ObjIndent
			{
				asNeed := deSkob->{ObjIndent^}
				itName = asNeed.MyStr
			}
		}
	}
	if indName == "_" indName = null
	if itName == "_"
	{
		foundIt := false
		tstr := tr
		while tstr != null
		{
			if tstr.GetValue() == "~~for()" 
			{
				if tstr is BoxForOldFashionMulti
				{
					asNeed := tstr->{BoxForOldFashionMulti^}
					for asNeed.Names{
						if it == "it"{
							foundIt = true
							break
						}
					}
					for asNeed.IndNames
					{
						if it == "it"{
							foundIt = true
							break
						}
					}
					

				}else{
					foundIt = true
					break
				}
			}
			tstr = tstr.Up
		}
		if foundIt{
			itName = "it" + GetNewId()
		}
		else {
			itName = "it"
		}
	}

	tr.Down.Right = null
	itm := tr.Down
	PopOutNode(itm)
	newNode := ReplaceNode(tr,new ObjIndent(itName))
	newNode.Line = tr.Line

	nms := Queue.{string}()
	nms.Push(itName)
	indNames := Queue.{string}()
	indNames.Push(indName)
	itms := Queue.{Object^}()
	itms.Push(itm)

	tmpNode := new Object ; $temp
	ReplaceNode(itUp,tmpNode)
	fr := new BoxForOldFashionMulti(nms,indNames,itms,itUp)
	ReplaceNode(tmpNode,fr)
	return fr
}
