#import "AVLTree.cp"
#import "FatArray.cp"

AVLSetIterator := class .{@DATA}
{
	miniStack := Stack.{Tuple.{AVLTreeNode.{DATA}^,bool},32}
	nNode := AVLTreeNode.{DATA}^

	this := !(AVLTreeNode.{DATA}^ nd) -> void
	{
		miniStack."this"()
		nNode = nd
		if nd != null{
			while nNode.Left != null
			{
				if nNode.Right != null
					miniStack.Emplace(nNode.Right,true)
				miniStack.Emplace(nNode,false)
				nNode = nNode.Left
			}
			if nNode.Right != null
				miniStack.Emplace(nNode.Right,false)
		}
	}
	IsEnd := !() -> bool { return nNode == null }
	"^" := !() -> ref DATA { return nNode.data }
	Inc := !() -> void { 
		if miniStack.Empty()
		{
			nNode = null
			return void
		}
		nNode = miniStack.Front().0
		part := miniStack.Front().1
		miniStack.Pop()

		if part
		{
			while nNode.Left != null
			{
				if nNode.Right != null
					miniStack.Emplace(nNode.Right,true)
				miniStack.Emplace(nNode,false)
				nNode = nNode.Left
			}
			if nNode.Right != null
				miniStack.Emplace(nNode.Right,false)
			//None
		}else{
		}
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
