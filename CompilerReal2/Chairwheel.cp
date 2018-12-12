#import "Tree.cp"


CVStuf := Queue.{string}

TryCheckSuffix := !(string inConst,string sufff) -> Object^
{
	if sufff == "cv"
	{
		for it : CVStuf , i : 0
		{
			if it == inConst return new ObjInt(i)
		}
		preRet := new ObjInt(CVStuf.Size())
		CVStuf.Push(inConst)
		return preRet
	}
	return null
}
