#import "Tree.cp"
#import "QuestionBox.cp"

reParseDown := !(Object^ itm, LocalParam^ prm,string vargName) -> void
{
	if itm == null return void
	reParseDown(itm.Right,prm,vargName)
	reParseDown(itm.Down ,prm,vargName)

	if itm.GetValue() == "~ind"
	{
		itInd := itm->{ObjIndent^}
		if itInd.MyStr == vargName
		{
			ReplaceNode(itm,new ParamNaturalCall("",prm))
		}
	}
}
TryParseMacro := !(Object^ tr) -> Object^
{
	return TryParseMacro(tr,tr)
}

TryParseMacro := !(Object^ tr ,Object^ itUp) -> Object^
{
	if tr == null return null
	if tr.GetValue() == "{}" return null
	if tr.Down == null return null
	
		
	if tr.GetValue() == "()" and tr.Left == null and tr.Down != null and tr.Down.Right == null
	{
		prevUp := tr.Up
		ReplaceNode(tr.Up,tr.Down)
		if prevUp == itUp {
			return tr.Down
		}
		return itUp
	}
	if tr.Down != null and tr.Down.Right != null
	{
		if tr.Down.Right.GetValue() == "or"
		{
			newNode := new FlowOr()
			ReplaceNode(tr,newNode)
			newNode.Down = tr.Down
			PopOutNode(tr.Down.Right)
			newNode.Down.SetUp(newNode)
			if tr == itUp return newNode
			return itUp
		}
		if tr.Down.Right.GetValue() == "and"
		{
			newNode := new FlowAnd()
			ReplaceNode(tr,newNode)
			newNode.Down = tr.Down
			PopOutNode(tr.Down.Right)
			newNode.Down.SetUp(newNode)
			if tr == itUp return newNode
			return itUp
		}
		if tr.Down.Right.GetValue() == "?"
		{
			forceToBool := false
			toDownd := tr
			prevNode := tr

			while toDownd != null
			{
				if toDownd.GetValue() == "while()" or toDownd.GetValue() == "~if()"
					or toDownd.GetValue() == "if()"
					or toDownd.GetValue() == "?or??"
				{
					if toDownd is QuestionBox
					{
					}else{
						toDownd = prevNode
						forceToBool = true
						break
					}
				}
				if toDownd == itUp
					break
				prevNode = toDownd
				toDownd = toDownd.Up
			}
			if toDownd == null{
				itUp.EmitError("problem at macro\n")
				return null
			}

			replObject := new Object
			qObject := new QuestionBox(replObject,forceToBool)
			qTree := tr.Down
			ReplaceNode(toDownd,qObject)
			ReplaceNode(tr,replObject)
			qObject.Down = qTree
			qObject.Down.Right = toDownd
			qObject.Down.SetUp(qObject)
			toDownd.Left = qTree
			if toDownd == itUp{
				return qObject
			}
			return itUp
		}
		if tr.Down.Right.GetValue() == "..."
		{
			vargName := null->{string}
			lastFunc := null->{BoxFuncBody^}
			lastPreSkob := itUp
			meetBoxFunc := false
			ups := itUp
			while ups != null
			{
				if ups.Up != null and ups.Up.GetValue() == "()" 
				{
					if not meetBoxFunc
						lastPreSkob = ups
				}
				if ups is BoxFuncBody
				{
					meetBoxFunc = true
					asNN := ups->{BoxFuncBody^}
					if asNN.vargsName != null
					{
						vargName = asNN.vargsName
						lastFunc = asNN
					}
				}
				ups = ups.Up
			}
			if lastFunc == null return null
			
			parsCount := lastFunc.MyFuncType.ParsCount - lastFunc.funcUserParamsCount

			if parsCount == 0 {
				preRes := lastPreSkob
				PopOutNode(lastPreSkob)
				if preRes == lastPreSkob
				{
					return preRes.Right
				}else{
					return itUp
				}
				return itUp
			}else{
				addIter := tr

				objList := Stack.{Object^}() ; $temp
				objParms := Stack.{LocalParam^}() ; $temp

				for i : parsCount
				{
					newObj := tr.Down.Clone()
					newObj.Right = addIter.Right
					newObj.Up = addIter.Up
					if newObj.Right != null newObj.Right.Left = newObj
					newObj.Left = addIter
					addIter.Right = newObj
					//reParseDown(newObj,lastFunc.ItParams[i+lastFunc.funcUserParamsCount],vargName)
					objList.Push(newObj)
					objParms.Push(lastFunc.ItParams[i+lastFunc.funcUserParamsCount])

					addIter = newObj
				}

				for a : objList, b : objParms
				{
					reParseDown(a,b,vargName)
				}
				preRes := tr.Right
				PopOutNode(tr)
				return preRes
			}
		}
	}

	itr := tr.Down
	while itr != null
	{
		mbRes := TryParseMacro(itr,itUp)
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

	tmpNode := new Object ; $temp
	ReplaceNode(itUp,tmpNode)
	fr := new BoxForOldFashionMulti(nms,indNames,itms,itUp)
	ReplaceNode(tmpNode,fr)
	return fr
}
