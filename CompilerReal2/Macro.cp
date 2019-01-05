#import "Tree.cp"

TryParseMacro := !(Object^ tr) -> Object^
{
	return TryParseMacro2(tr,tr)
}

TryParseMacro2 := !(Object^ tr ,Object^ itUp) -> Object^
{
	if tr == null return null
	if tr.GetValue() == "{}" return null
	if tr.Down == null return null
	
	itr := tr.Down
	while itr != null
	{
		mbRes := TryParseMacro2(itr,itUp)
		if mbRes != null return mbRes
		itr = itr.Right
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

	tmpNode := new Object
	ReplaceNode(itUp,tmpNode)
	fr := new BoxForOldFashionMulti(nms,indNames,itms,itUp)
	ReplaceNode(tmpNode,fr)
	return fr
}
