RBSet := class .{@DATA}
{
	$keep

	itTree := RBTree.{DATA} ; #outer_class
	itSize := int

	this := !() -> void
	{
		itTree."this"()
		itSize = 0
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
		if resl != null
		{
			itTree.RemoveNode(resl)
			itSize -= 1
		}
	}
	Contain := !(DATA dat) -> bool
	{
		resl := itTree.FindNode(dat)
		return resl != null
	}
	Size := !() -> int { return itSize }
	"~For" := !() -> CommonSetIterator.{DATA}
	{
		return CommonSetIterator.{DATA}(itTree.Start)
	}
	"<<" := !(DATA dat) .{} self_return
	{
		Insert(dat)
		return this
	}
}

"in" := !(SetType this, RBSet.{@SetType} bag) -> bool
{
	return bag.Contain(this)
}
