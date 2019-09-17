IsSameTypeObject := !(Object^ obj, Object^ toTst,Queue.{ObjConstHolder^} res, resB) -> bool
{
	if obj is ObjTemplateType
	{
		sNeed := obj->{ObjTemplateType^}
		if res[^].ItName == sNeed.MyStr
			return true
		res.Push(new ObjConstHolder(sNeed.MyStr,toTst))
		return true
	}
	if not CmpConstObjs(obj,toTst)
	{
		resB[0] = false
		return false
	}
	return true
}

IsSameType := !(Object^ obj,Type^ itT ,Queue.{ObjConstHolder^} res, bool^ resB) -> Type^
{
	for it : res
	{
		if it == null return null
	}
	if obj == null {
		resB[0] = false
		return null
	}
	
	if obj is ObjIndent
	{
		val := ParseType(obj)
		if val != itT resB[0] = false
		return val
	}else
	if obj.GetValue() == "~{}type"
	{
		sNeed := obj->{ObjTemplateType^}
		if res[^].ItName == sNeed.MyStr
			return itT
		res.Push(new ObjConstHolder(sNeed.MyStr,(new ObjType(itT))->{Object^}))
		return itT
	}else
	if obj is ObjData
	{
		if obj.Down.Right == null return IsSameType(obj.Down,itT,res,resB)
		if obj.Down.Right.GetValue() == "^"
		{
			if itT.GetType() != "point"
			{
				resB[0] = false
				return null
			}
			preT :=  IsSameType(obj.Down,itT.Base,res,resB)
			if preT != null {
				return preT.GetPoint()
			}
			return null
		}
		if obj.Down.Right.GetValue() == "[]"
		{
			if itT.GetType() != "arr"{
				if itT.GetType() == "fatarr"
				{
					typD2 := IsSameType(obj.Down,itT.Base,res,resB)
					if typD2 == null 
					{
						resB[0] = false
						return null
					}
					
					if obj.Down.Right.Down != null 
					{
						resB[0] = false
						return null
					}
					return typD2.GetFatArray()
				}
				resB[0] = false
				return null
			}
			asNeedType := itT->{TypeArr^}

			typD := IsSameType(obj.Down,itT.Base,res,resB)
			if typD == null {
				resB[0] = false
				return null
			}

			itemD := obj.Down.Right.Down
			if itemD == null {
				resB[0] = false
				return null
			}
			
			if itemD.IsConst
			{
				if itemD.GetValue() != "~int"
				{
					resB[0] = false
					return null
				}
				asNeedDown := itemD->{ObjInt^}
				if asNeedDown.MyInt != asNeedType.Size
				{
					resB[0] = false
					return null
				}
				return null				
			}else{
				if itemD.GetValue() == "~{}type"
				{
					asNeedDown2 := itemD->{ObjTemplateType^}
					if res[^].ItName == asNeedDown2.MyStr
						return itT
					res.Push(new ObjConstHolder(asNeedDown2.MyStr,(new ObjInt(asNeedType.Size))->{Object^}))
					return itT
				}else{
					resB[0] = false
					return null
				}
			}
		}
	}
	if obj.Down?.Right?.Right?.GetValue() == "{}" and obj.Down.Right.GetValue() == "." and itT is TypeClass
		and obj.Down is ObjIndent
	{
		asInd := obj.Down->{ObjIndent^}
		asCl := itT->{TypeClass^}.ToClass
		if asInd.MyStr == asCl.ClassName
		{
			itms := Queue.{Object^}() ; $temp
			for it : asInd.Right.Right.Down
			{
				if it.GetValue() != ","
					itms.Push(it)
			}
			if itms.Size() != asCl.ItConsts.Size()
			{
				resB[0] = false
				return null
			}
			for inA : itms, inB : asCl.ItConsts
			{
				if inB is ObjType
				{
					IsSameType(inA,inB->{ObjType^}.MyType,res,resB)
					if resB^ == false
						return null
				}else
				{
					if not IsSameTypeObject(inA,inB,res,resB)
						return null
				}
			}
			return itT
		}
		
	}
	resB[0] = false
	return null
}

InHolder := class extend Object
{
	this := !(Object^ v) -> void
	{
		Down = v.Down
		Down.SetUp(this&)
		PopOutNode(Down.Right)
		v.Down = null
	}
	GetValue := virtual !() -> string
	{
		return "~{in}type"
	}
}

MakeGoodConsts := !(Object^ l) -> bool
{
	iter := l.Down

	while iter != null
	{
		if iter.GetValue() != ","
		{
			if not iter.IsConst 
			{
				gotIt := false
				if iter.GetValue() == "~d"
				{
					if iter.Down.Right != null
					{
						if iter.Down.Right.GetValue() == "in"
						{
							gotIt = true
							cr := new InHolder(iter)
							iter  = ReplaceNode(iter,cr)
						}
					}
				}
				if iter.GetValue() != "~{}type" and not gotIt
				{
					val := ParseType(iter)

					if val != null
					{
						iter = ReplaceNode(iter,new ObjType(val))
					}// else error
				}
			}
		}
		iter = iter.Right
	}
	return true
}

IsEqConsts := !(Object^ l, FuncInputBox itBox, Queue.{ObjConstHolder^} res) -> bool
{
	for it : itBox.itConsts
	{
		if it == null return false
	}
	if l == null return false
	re := bool
	iter := l.Down

	i := 0
	while iter != null
	{
		if iter.GetValue() != ","
		{
			if not iter.IsConst
			{
				if iter.GetValue() == "~{in}type"
				{
					if itBox.itConsts[i].GetValue() != "~type" return false
					asNeed := iter->{InHolder^}
					asN2 := itBox.itConsts[i]->{ObjType^}
					
					if asN2.MyType == null return false
					if not asN2.MyType is TypeClass return false

					val := ParseType(asNeed.Down.Right)
					if val is TypeClass return false
					val2 := val->{TypeClass^}
					re = true
					val3 := IsSameType(asNeed.Down,asN2.MyType,res,re&)
					if not re return false

					iterInT := ((val3->{TypeClass^}).ToClass)
					found := false

					while iterInT != null
					{
						if iterInT.ClassType
							found  = true
						iterInT = iterInT.Parent
					}

					if not found return false

				}else{
					if itBox.itConsts.Size() < i return false
					if itBox.itConsts[i].GetValue() == "~type"
					{
						asNeed := itBox.itConsts[i]->{ObjType^}
						re = true
						IsSameType(iter,asNeed.MyType,res,re&)
						if not re return false
					}else{
						if iter.GetValue() != "~{}type" return false
						asNeed := iter->{ObjTemplateType^}
						res.Push(new ObjConstHolder(asNeed.MyStr,itBox.itConsts[i]))
					}
				}
			}else{
				if not CmpConstObjs(iter,itBox.itConsts[i])
					return false
			}
			i += 1
		}
		iter = iter.Right
	}
	return true
}
