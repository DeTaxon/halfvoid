AVLMap.{@KEY,@VALUE} := type ContainerCommonMap.{AVLTree,KEY,VALUE}
RBMap.{@KEY,@VALUE} := type ContainerCommonMap.{RBTree,KEY,VALUE}

ContainerCommonMap := class .{@TreeType,@KEY,@VALUE}
{
	$keep

	itTree := TreeType.{BadPair.{KEY,VALUE}} ; #outer_class
	itSize := int

	this := !() -> void
	{
		itTree."this"()
		itSize = 0
	}
	"[]" := !(KEY dat) .{}  -> ref VALUE
	{
		resl := CommonTreeNode.{BadPair.{KEY,VALUE}}^
		if(itTree.FindOrCreate(BadPair.{KEY,VALUE}(dat),resl&))
		{
			resl.data.first = dat
			//resl.data.second."this"()
			itSize += 1
		}
		return resl.data.second
	}

	TryFind := !(KEY dat) -> VALUE^
	{
		resl := itTree.FindNode(BadPair.{KEY,VALUE}(dat))
		if resl == null return null
		return resl.data.second&
	}
	Contain := !(KEY dat) -> bool
	{
		resl := itTree.FindNode(BadPair.{KEY,VALUE}(dat))
		return resl != null
	}
	Remove := !(DATA dat) .{} -> void
	{
		resl := itTree.FindNode(BadPair.{KEY,VALUE}(dat))
		if resl != null{
			itTree.RemoveNode(resl)
			itSize -= 1
		}
	}
	"~For" := !() .{} -> CommonMapIterator.{BadPair.{KEY,VALUE},KEY,VALUE}
	{
		return CommonMapIterator.{BadPair.{KEY,VALUE},KEY,VALUE}(itTree.Start)
	}
	Size := !() -> int { return itSize }
	"in" := !(KEY val) -> bool
	{
		return Contain(val)
	}
	values := fake
	{
		"in" := !(VALUE val) -> bool
		{
			if this[^] == val
				return true
			return false
		}
		Clone := !() .{} -> VALUE[] 
		{
			res := new VALUE[itSize]
			for frm : this, to : res
			{
				to = frm
			}
			return res
		}
		Get := !() -> VALUE[]
		{
			return Clone() ; $temp
		}
	}
}
