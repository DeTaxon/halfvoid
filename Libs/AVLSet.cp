#import "AVLTree.cp"
#import "FatArray.cp"

AVLSetIterator := class .{@DATA}
{
	miniStack := FatArray.{AVLTreeNode.{DATA}^,32}

	this := !(AVLTreeNode.{DATA}^ nd) -> void
	{
		miniStack."this"()
		if nd != null{
			miniStack.Push(nd)
		}
	}
	IsEnd := !() -> bool { return miniStack.IsEmpty() }
	"^" := !() -> ref DATA { return miniStack.Back().data }
	Inc := !() -> void { 
		nowNod := miniStack.Pop()
		if nowNod.Left != null miniStack.Push(nowNod.Left)
		if nowNod.Right != null miniStack.Push(nowNod.Right)
	}
}

AVLSet := class .{@DATA}
{
	itTree := AVLTree.{DATA}

	this := !() -> void
	{
		itTree."this"()
	}

	"<<" := !(DATA data) .{} -> ref AVLSet.{DATA}
	{
		Insert(data)
		return this
	}
	Insert := !(DATA dat) .{} -> void
	{
		resl := AVLTreeNode.{DATA}^
		if(itTree.FindOrCreate(dat,resl&))
		{
			resl.data = dat
		}
		
	}
	Remove := !(DATA dat) .{} -> void
	{
		resl := itTree.FindNode(dat)
		if resl != null{
			itTree.RemoveNode(resl)
		}
	}
	Contain := !(DATA dat) -> bool
	{
		resl := itTree.FindNode(dat)
		return resl != null
	}
	"~For" := !() -> AVLSetIterator.{DATA}
	{
		return AVLSetIterator.{DATA}(itTree.Start)
	}
}

//"in" := !(A itm, AVLSet.{@A} bag) -> bool
//{
//	return bag.Contain(itm)
//}
