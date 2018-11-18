#import "AVLTree.cp"
#import "FatStack.cp"

//AVLSetIterator := class .{@DATA}
//{
//	miniStack := FatStack.{AVLTreeNode.{DATA}^,32}
//
//	this := !(AVLTreeNode.{DATA}^ nd) -> void
//	{
//		miniStack."this"()
//		if nd != null{
//			miniStack.Push(nd)
//		}
//	}
//	IsEnd := !() -> bool { return miniStack.IsEmpty() }
//	"^" := !() -> ref DATA { return miniStack.Back().data }
//	Inc := !() -> void { 
//		nowNod := miniStack.Pop()
//		if nowNod.Left != null miniStack.Push(nowNod.Left)
//		if nowNod.Right != null miniStack.Push(nowNod.Right)
//	}
//}

BadPair := class .{@A,@B}
{
	first := A
	second := B

	this := !() -> void
	{
	}
	this := !(A itA) -> void
	{
		first = itA
	}
	"<" := !(BadPair.{A,B} itR) -> bool
	{
		return first < itR.first
	}
	"=" := !(BadPair.{A,B} itR) -> void
	{
		first = itR.first
	}
	"==" := !(BadPair.{A,B} itR) -> bool
	{
		return first == itR.first
	}
}

AVLMap := class .{@KEY,@VALUE}
{
	itTree := AVLTree.{BadPair.{KEY,VALUE}}

	this := !() -> void
	{
		itTree."this"()
	}
	"[]" := !(KEY dat) -> ref VALUE
	{
		resl := AVLTreeNode.{BadPair.{KEY,VALUE}}^
		if(itTree.FindOrCreate(BadPair.{KEY,VALUE}(dat),resl&))
		{
			resl.data.first = dat
			resl.data.second."this"()
		}
		return resl.data.second
	}

	TryFind := !(KEY dat) -> VALUE^
	{
		resl := itTree.FindNode(BadPair.{KEY,VALUE}(dat))
		if resl == null return null
		return resl.data.second&
	}
}
