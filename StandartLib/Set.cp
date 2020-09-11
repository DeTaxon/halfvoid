AVLSet.{@DATA} := type ContainerCommonSet.{AVLTree,DATA}
RBSet.{@DATA} := type ContainerCommonSet.{RBTree,DATA}
ContainerCommonSet := class .{@TreeType,@DATA}
{
	$keep

	itTree := TreeType.{DATA} ; #outer_class
	itSize := int

	this := !() -> void
	{
		itTree."this"()
		itSize = 0
	}

	"<<" := !(DATA data) .{} self_return 
	{
		Insert(data)
		return this
	}
	Insert := !(DATA dat) .{} -> void
	{
		resl := CommonTreeNode.{DATA}^
		if(itTree.FindOrCreate(dat,resl&))
		{
			resl.data = dat
			itSize += 1
		}
		
	}
	Remove := !(DATA dat) .{} -> void
	{
		resl := itTree.FindNode(dat)
		if resl != null{
			itTree.RemoveNode(resl)
			itSize -= 1
		}
	}
	Contain := !(DATA dat) -> bool
	{
		resl := itTree.FindNode(dat)
		return resl != null
	}
	"~For" := !() .{} -> CommonSetIterator.{DATA}
	{
		return CommonSetIterator.{DATA}(itTree.Start)
	}
	Size := !() -> int { return itSize }
	"in" := !(DATA val) -> bool
	{
		return Contain(val)
	}
}
