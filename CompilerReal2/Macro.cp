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

	printf("before\n")

	indName := null->{string}
	if tr.Down.Right.Down.Right != null and tr.Down.Right.Down.Right is ObjIndent
	{
		asNeed := tr.Down.Right.Down.Right->{ObjIndent^}
		indName = asNeed.MyStr
	}
	
	itName := "it" + GetNewId()

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
	printf("after\n")
	return fr
}
