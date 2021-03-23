CommonMapIterator := class .{@DATA,@KEY,@VALUE}
{
	miniStack := List.{Tuple.{CommonTreeNode.{DATA}^,bool}}^ ; $keep $temp
	nNode := CommonTreeNode.{DATA}^

	this := !(CommonTreeNode.{DATA}^ nd) .{} -> void
	{
		miniStack = new List.{Tuple.{CommonTreeNode.{DATA}^,bool}} ; $temp $keep
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
	"^" := !() -> ref VALUE { return nNode.data.second }
	Ind := !() -> ref KEY { return nNode.data.first}
	Inc := !() .{} -> void {
		if miniStack.IsEmpty()
		{
			nNode = null
			return void
		}
		nNode = miniStack.Front().0
		part := miniStack.Front().1
		miniStack.Pop() ; $temp

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
	DoAMove := !() .{} -> void
	{
		if $reverse
		{
			DoAMoveRight()
		}else{
			DoAMoveLeft()
		}
	}
	DoAMoveRight := !() -> void
	{
		while nNode.Right != null
		{
			if nNode.Left != null
				miniStack.EmplaceFront(nNode.Left,true) ; $temp
			miniStack.EmplaceFront(nNode,false) ; $temp
			nNode = nNode.Right
		}
		if nNode.Left != null
			miniStack.EmplaceFront(nNode.Left,false) ; $temp
	}
	DoAMoveLeft := !() -> void
	{
		while nNode.Left != null
		{
			if nNode.Right != null
				miniStack.EmplaceFront(nNode.Right,true) ; $temp
			miniStack.EmplaceFront(nNode,false) ; $temp
			nNode = nNode.Left
		}
		if nNode.Right != null
			miniStack.EmplaceFront(nNode.Right,false) ; $temp
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
CommonSetIterator := class .{@DATA}
{
	miniStack := List.{Tuple.{CommonTreeNode.{DATA}^,bool}}^ ; $keep $temp
	nNode := CommonTreeNode.{DATA}^

	this := !(CommonTreeNode.{DATA}^ nd) .{} -> void
	{
		miniStack = new List.{Tuple.{CommonTreeNode.{DATA}^,bool}} ; $temp $keep
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
		if miniStack.IsEmpty()
		{
			nNode = null
			return void
		}
		nNode = miniStack.Front().0
		part := miniStack.Front().1
		miniStack.Pop() ; $temp

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
				miniStack.EmplaceFront(nNode.Left,true) ; $temp
			miniStack.EmplaceFront(nNode,false) ; $temp
			nNode = nNode.Right
		}
		if nNode.Left != null
			miniStack.EmplaceFront(nNode.Left,false) ; $temp
	}
	DoAMoveLeft := !() -> void
	{
		while nNode.Left != null
		{
			if nNode.Right != null
				miniStack.EmplaceFront(nNode.Right,true) ; $temp
			miniStack.EmplaceFront(nNode,false) ; $temp
			nNode = nNode.Left
		}
		if nNode.Right != null
			miniStack.EmplaceFront(nNode.Right,false) ; $temp
	}
}
