CommonTreeNode := class .{@DATA}
{
	data := DATA
	balance := s8
	Left,Right,Up := CommonTreeNode.{DATA}^
	this := !() -> void
	{
		Left = null
		Right = null
		Up = null
		balance = 0
	}
}

CommonTree := class .{@DATA}
{
	Start := CommonTreeNode.{DATA}^
	if $keep
		CreatedNodes := CommonTreeNode.{DATA}^
	
	FindNode := !(DATA dat) -> CommonTreeNode.{DATA}^
	{
		itr := Start
		while itr != null
		{
			stRes := itr.data <=> dat
			if stRes == 0 return itr
			if stRes < 0 	itr = itr.Right
			else 		itr = itr.Left
		}
		return itr
	}
	createNode := !() .{} -> CommonTreeNode.{DATA}^
	{
		if $keep
		{
			if CreatedNodes != null
			{
				newNode := CreatedNodes
				CreatedNodes = CreatedNodes.Right
				newNode.Left = null
				newNode.Up = null
				newNode.Right = null
				newNode.balance = 0
				return newNode
			}else{
				return new CommonTreeNode.{DATA}
			}
		}	
		return new CommonTreeNode.{DATA}
	}
	RotateLeft := !(CommonTreeNode.{DATA}^ toRotate) -> void
	{
		dwNode := toRotate.Right
		toRotate.Right = dwNode.Left
		if toRotate.Right != null toRotate.Right.Up = toRotate
		dwNode.Up = toRotate.Up
		toRotate.Up = dwNode
		dwNode.Left = toRotate
		if dwNode.Up == null {
			Start = dwNode
		}else{
			if dwNode.Up.Left == toRotate dwNode.Up.Left = dwNode
			else dwNode.Up.Right = dwNode
		}
	}
	RotateRight := !(CommonTreeNode.{DATA}^ toRotate) -> void
	{
		dwNode := toRotate.Left
		toRotate.Left = dwNode.Right
		if toRotate.Left != null toRotate.Left.Up = toRotate
		dwNode.Up = toRotate.Up
		toRotate.Up = dwNode
		dwNode.Right = toRotate
		if dwNode.Up == null {
			Start = dwNode
		}else{
			if dwNode.Up.Left == toRotate dwNode.Up.Left = dwNode
			else dwNode.Up.Right = dwNode
		}
	}
	remNode := !(CommonTreeNode.{DATA}^ toRem) .{} -> void
	{
		if $keep
		{
			toRem.Right = CreatedNodes
			CreatedNodes = toRem
		}else
		{
			delete toRem
		}
	}
}