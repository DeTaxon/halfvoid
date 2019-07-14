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
}
