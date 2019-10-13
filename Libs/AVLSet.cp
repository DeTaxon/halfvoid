AVLSetIterator := class .{@DATA}
{
	miniStack := Stack.{Tuple.{AVLTreeNode.{DATA}^,bool},32}
	nNode := AVLTreeNode.{DATA}^

	this := !(AVLTreeNode.{DATA}^ nd) .{} -> void
	{
		miniStack."this"()
		nNode = nd
		if nd != null{
			if $reverse
			{
				DoAMoveRight()
			}else{
				DoAMoveLeft()
			}
		}
	}
	IsEnd := !() -> bool { return nNode == null }
	"^" := !() -> ref DATA { return nNode.data }
	Inc := !() .{} -> void { 
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
			if $reverse
			{
				DoAMoveRight()
			}else{
				DoAMoveLeft()
			}
		}else{
		}
	}
	DoAMoveRight := !() -> void
	{
		while nNode.Right != null
		{
			if nNode.Left != null
				miniStack.Emplace(nNode.Left,true)
			miniStack.Emplace(nNode,false)
			nNode = nNode.Right
		}
		if nNode.Left != null
			miniStack.Emplace(nNode.Left,false)
	}
	DoAMoveLeft := !() -> void
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
		this.Insert(data)
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
	"~For" := !() .{} -> AVLSetIterator.{DATA}
	{
		return AVLSetIterator.{DATA}(itTree.Start)
	}
}

//"in" := !(A itm, AVLSet.{@A} bag) -> bool
//{
//	return bag.Contain(itm)
//}
