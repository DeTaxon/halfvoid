#import "AVLTree.cp"
#import "arrs.cp"

AVLMapIterator := class .{@DATA,@KEY,@VALUE}
{
	miniStack := Stack.{AVLTreeNode.{DATA}^,32}

	this := !(AVLTreeNode.{DATA}^ nd) -> void
	{
		miniStack."this"()
		if nd != null{
			miniStack.Push(nd)
		}
	}
	IsEnd := !() -> bool { return miniStack.Size() == 0 }
	"^" := !() -> ref VALUE { return miniStack.Front().data.second }
	Ind := !() -> ref KEY { return miniStack.Front().data.first}
	Inc := !() -> void { 
		nowNod := miniStack.Pop()
		if nowNod.Left != null miniStack.Push(nowNod.Left)
		if nowNod.Right != null miniStack.Push(nowNod.Right)
	}
}p

BadPair := class .{@A,@B}
{
	first := A
	second := B

	this := !() -> void	{	}
	this := !(A itA) -> void{ first = itA	}
	"<" := !(BadPair.{A,B} itR) -> bool {	return first < itR.first }
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
	"[]" := !(KEY dat) -> ref VALUE
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
