#import "AVLTree.cp"
#import "arrs.cp"

AVLMapIterator := class .{@DATA,@KEY,@VALUE}
{
	miniStack := Stack.{Pair.{AVLTreeNode.{DATA}^,bool},32}
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
	"^" := !() -> ref VALUE { return nNode.data.second }
	Ind := !() -> ref KEY { return nNode.data.first}
	Inc := !() -> void {
		if miniStack.Empty()
		{
			nNode = null
			return void
		}
		nNode = miniStack.Front().first
		part := miniStack.Front().second
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

BadPair := class .{@A,@B}
{
	first := A
	second := B

	this := !() -> void	{	}
	this := !(A itA) -> void{ first = itA	}
	"<" := !(BadPair.{A,B} itR) -> bool {	return first < itR.first }
	"<=>" := !(BadPair.{A,B} itR) -> int {	return first <=> itR.first }
	"=" := !(BadPair.{A,B} itR) -> void {	first = itR.first	}
	"==" := !(BadPair.{A,B} itR) -> bool {	return first == itR.first }
}

AVLMap := class .{@KEY,@VALUE}
{
	itTree := AVLTree.{BadPair.{KEY,VALUE}}
	itSize := int

	this := !() -> void
	{
		itTree."this"()
		itSize = 0
	}
	"[]" := !(KEY dat) .{}  -> ref VALUE
	{
		resl := AVLTreeNode.{BadPair.{KEY,VALUE}}^
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
	"~For" := !() -> AVLMapIterator.{BadPair.{KEY,VALUE},KEY,VALUE}
	{
		return AVLMapIterator.{BadPair.{KEY,VALUE},KEY,VALUE}(itTree.Start)
	}
	Size := !() -> int { return itSize }
}
