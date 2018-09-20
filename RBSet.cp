#import "RBTree.cp"
#import "FatStack.cp"

RBSetIterator := class .{@DATA}
{
	miniStack := FatStack.{RBTreeNode.{DATA}^,32}

	this := !(RBTreeNode.{DATA}^ nd) -> void
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

RBSet := class .{@DATA}
{
	itTree := RBTree.{DATA}

	this := !() -> void
	{
		itTree."this"()
	}

	Insert := !(DATA dat) -> void
	{
		resl := RBTreeNode.{DATA}^
		if(itTree.FindOrCreate(dat,resl&))
		{
			resl.data = dat
		}
		
	}
	"~For" := !() -> RBSetIterator.{DATA}
	{
		return RBSetIterator.{DATA}(itTree.Start)
	}
	"<<" := !(DATA dat) -> ref RBSet.{DATA}
	{
		Insert(dat)
		return this
	}
}

