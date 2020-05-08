//FuncTypesArray := type FatArray.{Pair.{Type^,bool},1024}
//FuncConstsArray := type FatArray.{Object^,1024}
//FuncAttrsArray := type AVLMap.{string,Object^}

AttrArrayType := type AVLMap.{string,Object^}

FuncInputBox := class
{	
	//itPars := FatArray.{Pair.{Type^,bool},1024} //FuncTypesArray
	itPars := HybridQueue.{Pair.{Type^,bool},16} //FuncTypesArray
	//itConsts := FatArray.{Object^,1024} //FuncConstsArray
	itConsts := HybridQueue.{Object^,16} //FuncConstsArray
	itAttrs := AVLMap.{string,Object^} //FuncAttrsArray

	this := !() -> void
	{
		itPars."this"()
		itConsts."this"()
		itAttrs."this"()
	}
	GetConstsHash := !() -> int
	{
		retHash := 0

		for it : itConsts
		{
			if it is ObjType{
				asNeed := it->{ObjType^}
				if asNeed.MyType != null
					retHash += asNeed.MyType.ItHash
			}
			if it is ObjInt{
				asNeed := it->{ObjInt^}
				retHash += asNeed.MyInt
			}
		}

		return retHash
	}
	Clone := !() -> FuncInputBox^
	{
		preRet := new FuncInputBox
		preRet.itConsts.Push(itConsts[^])
		preRet.itAttrs[i] = itAttrs[^i]
		return preRet
	}
	"<=>" := !(FuncInputBox itBox) -> int
	{
		sizeCmpRes := this.itConsts.Size() <=> itBox.itConsts.Size()
		if sizeCmpRes != 0 return sizeCmpRes

		for c,i : itBox.itConsts
		{
			if not CmpConstObjs(c,this.itConsts[i])
				return -1
		}

		for itr,ind : itAttrs
		{
			inMap := itBox.itAttrs.TryFind(ind)
			if inMap == null {
				if itr is ObjBool and not itr->{ObjBool^}.MyBool
				{}else{
					return -1
				}
			}
			if not CmpConstObjs(itr,inMap^)
				return -1
		}
		for itr,ind : itBox.itAttrs
		{
			inMap := this.itAttrs.TryFind(ind)
			if inMap == null {
				if itr is ObjBool and not itr->{ObjBool^}.MyBool
				{}else{
					return -1
				}
			}
		}
		return 0
	}
}
