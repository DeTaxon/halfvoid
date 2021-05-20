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

	if tr is ObjIndent and tr->{ObjIndent^}.isLambdaNum
	{
		prevUp := tr
		mvUp := tr.Up
		while mvUp != null
		{
			isGood := false
			if mvUp.Right != null and IsOper(mvUp.Right.GetValue())
			{
				isGood = true
			}else{
				if mvUp.Left != null and IsOper(mvUp.Left.GetValue())
					isGood = true
			}
			if not isGood
				break
			prevUp = mvUp
			mvUp = mvUp.Up
		}
		if mvUp == null
			return null
		if IsSLambda(mvUp)
			return null
		if mvUp.GetValue() != "~d"
			return null
		if mvUp.Up?.GetValue() == "return()"
			return null
		nL := new SBoostLambda()
		UNext(mvUp,nL,1)
		return null
	}

	if tr.Down == null return null
		
	if tr.GetValue() == "()" and tr.Left == null and tr.Down?.Right == null
	{
		//if tr.Down.Right == null
		//{
			prevUp := tr.Up
			ReplaceNode(tr.Up,tr.Down)
			if prevUp == itUp {
				return tr.Down
			}
			return itUp
		//}else{
		//	prevUp := tr.Up
		//	prevUp.Down = tr.Down
		//	tr.Down.SetUp(prevUp)
		//	if prevUp == itUp {
		//		return tr.Down
		//	}
		//	return itUp
		//}
	}
	//if tr.GetValue() == "()" and tr.Left == null
	//{
	//	lastTr := tr.Down
	//	if lastTr != null
	//	{
	//		ReplaceNode(tr,new ObjData())
	//	}
	//	return itUp
	//}
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
		if tr.Down.Right.GetValue() == "??"
		{
			newNode := new QAtleastBox()
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
				if toDownd.GetValue() in !["while()","~if()","if()","?or??","~~for()"]
				{
					if toDownd is QAtleastBox
					{
						toDownd = prevNode
						break
					}
					if IsQBox(toDownd) or toDownd is QAtleastBox
					{
					}else if toDownd.GetValue() == "~~for()"
					{
						toDownd = prevNode
						break
					}else{
						toDownd = prevNode
						forceToBool = true
						break
					}
				}
				if toDownd == itUp
				{
					break
				}
				prevNode = toDownd
				toDownd = toDownd.Up
			}
			if toDownd == null {
				itUp.EmitError("problem at macro\n")
				return null
			}

			if toDownd.Up?.GetValue() in !["if()","~~for()"]
			{
				qObject2 := new QuestionBox2()
				if toDownd.Up.GetValue() == "if()"
				{
					qObject2.onBadLabel = toDownd.Up->{BoxIf^}.onBadLabel
				}
				if toDownd.Up.GetValue() == "~~for()"
				{
					asFor := toDownd.Up->{BoxForOldFashionMulti^}
					qObject2.onBadLabel = asFor.endLabel
				}
				qObject2.Down = tr.Down
				PopOutNode(tr.Down.Right)
				ReplaceNode(tr,qObject2)
				qObject2.Down.Up = qObject2
				return itUp
			}else{
				replObject := new Object
				qObject := new QuestionBox(replObject,forceToBool)
				if toDownd is BoxSwitch
				{
					qObject.passValue = true
					toDownd->{BoxSwitch^}.addedQ.Push(qObject)
					WorkBag.Push(qObject,State_Start)
				}
				toReturn := itUp
				if toDownd == itUp{
					toReturn = qObject
				}
				if toDownd.Up? is QAtleastBox
				{
					prev2 := toDownd
					itr2 := toDownd.Up
					while itr2 != itUp
					{
						if itr2 is QAtleastBox and itr2.Down == prev2
						{
							break
						}
						prev2 = itr2
						itr2 = itr2.Up
					}
					if itr2 is QAtleastBox
					{
						qObject.jmpLabel = itr2->{QAtleastBox^}.onFalse
					}else{
						jLand := new QJumpLand()
						toReturn = UNext(itUp,jLand,1)
						qObject.jmpLabel = jLand.GetEndLabel()
					}

					qObject.passValue = true
				}
				qTree := tr.Down
				ReplaceNode(toDownd,qObject)
				ReplaceNode(tr,replObject)
				qObject.Down = qTree
				qObject.Down.Right = toDownd
				qObject.Down.SetUp(qObject)
				toDownd.Left = qTree
				return toReturn
			}
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
				if ups.Up?.GetValue() == "()" 
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
	itName := null->{char^}
	itName = "_"//->{char^} //TODO: for some reason itName does not gets compiled right
	deSkob := tr.Down.Right.Down.Right
	if deSkob != null and deSkob is ObjIndent
	{
		asNeed := deSkob->{ObjIndent^}
		indName = asNeed.MyStr
		
		if deSkob.Right?.GetValue() == ","
		{
			deSkob = deSkob.Right.Right
			if deSkob? is ObjIndent
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
			itName = StrCopy("it"sbt + GetNewId())
		}
		else {
			itName = "it"
		}
	}

	if deSkob?.GetValue() == "^"
	{
		if not (deSkob.Up.Up.Down is ObjIndent)
			return null
		asInd := deSkob.Up.Up.Down->{ObjIndent^}.MyStr
		funcs := new Queue.{BoxFunc^} ; $temp
		tmpls := new Queue.{BoxTemplate^} ; $temp
		sts := QueueSet.{int}()
		CollectFuncsByName(asInd,deSkob,funcs^,tmpls^,false,false,sts,false)

		walkMap := new Stack.{int} ; $temp
		itr := deSkob.Up.Up.Down
		while itr != itUp
		{
			itW := 0
			while itr.Left != null
			{
				itr = itr.Left
				itW += 1
			}
			itr = itr.Up
			walkMap.Push(itW)
		}

		tmpNode := new Object ; $temp
		ReplaceNode(itUp,tmpNode)
		for it,i : funcs^
		{
			newNode := itUp.Clone()
			newNode.Right = tmpNode.Right
			newNode.Up = tmpNode.Up
			tmpNode.Right = newNode
			itr2 := newNode.Down
			prevItr2 := Object^
			for it2 : walkMap^
			{
				for it2 itr2 = itr2.Right
				prevItr2 = itr2
				itr2 = itr2.Down
			}
			itr2 = prevItr2
			itr2 = itr2.Up
			assert(it.Up is ObjParam)
			itCall := new ParamFuncCall("(none)",it.Up->{ObjParam^})
			itCall.MacroCreated = true
			ReplaceNode(itr2,itCall)
		}
		PopOutNode(tmpNode)
		return MacroRestart
	}

	tr.Down.Right = null
	itm := tr.Down
	PopOutNode(itm)
	newNode := ReplaceNode(tr,new ObjIndent(itName))
	newNode.Line = tr.Line

	nms := Queue.{string}() ; $temp
	nms.Push(itName)
	indNames := Queue.{string}() ; $temp
	indNames.Push(indName)
	itms := Queue.{Object^}() ; $temp
	itms.Push(itm)

	tmpNode := new Object ; $temp
	ReplaceNode(itUp,tmpNode)
	fr := new BoxForOldFashionMulti(nms,indNames,itms,itUp)
	fr.Line = newNode.Line
	ReplaceNode(tmpNode,fr)
	return fr
}
