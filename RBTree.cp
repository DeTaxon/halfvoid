RBColorBlack := 0
RBColorRed := 1

RBTreeNode := class .{@DATA}
{
	data := DATA
	color := s8
	Left,Right,Up := RBTreeNode.{DATA}^
	this := !() -> void
	{
		color = RBColorRed
		Left = null
		Right = null
		Up = null
	}
	PrintIt := !(int i) -> void
	{
		for i printf("-")
		printf("%i\n",data)
		if Left != null Left.PrintIt(i+1)
		if Right != null Right.PrintIt(i+1)
	}
	TestSize := !(int i, !(int)&-> void toSay) -> void
	{
		if Left != null TestSize(i+1,toSay)
		if Right != null TestSize(i+1,toSay)
		if Left != null and Right != null toSay(i)
	}
}

RBTree := class .{@DATA}
{
	Start := RBTreeNode.{DATA}^
	
	this := !() -> void
	{
		Start = null
	}
	FindNode := !(DATA dat) -> RBTreeNode.{DATA}^
	{
		return null
	}
	FindOrCreate := !(DATA dat,RBTreeNode.{DATA}^^ toRet) -> bool //bool isCreated
	{
		if Start == null 
		{
			Start = new RBTreeNode.{DATA}()
			Start.color = RBColorBlack
			toRet^ = Start
			return true
		}
		prev := Start
		iter := Start
		
		while iter != null
		{
			if iter.data == dat{
				break
			}else{
				prev = iter
			}
			if dat > iter.data 	iter = iter.Right
			else 			iter = iter.Left
		}
		if iter != null {
			toRet^ = iter
			return false
		}
		
		if prev.data < dat{
			prev.Right = new RBTreeNode.{DATA}()
			prev.Right.Up = prev
			iter = prev.Right
		}else{
			prev.Left = new RBTreeNode.{DATA}()
			prev.Left.Up = prev
			iter = prev.Left
		}
		
		RBRepair(iter)

		toRet^ = iter
		return true
	}
	RotateLeft := !(RBTreeNode.{DATA}^ toRotate) -> void
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
	RotateRight := !(RBTreeNode.{DATA}^ toRotate) -> void
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
	RBRepair := !(RBTreeNode.{DATA}^ toRep) -> void
	{
		if toRep.Up == null
		{
			if toRep.color != RBColorBlack
				toRep.color = RBColorBlack
			return void
		}
		if toRep.Up.color == RBColorBlack return void

		GrandNode := toRep.Up.Up

		UnclColorB := true
		UnclNode := RBTreeNode.{DATA}^
		UnclNode = null
		if GrandNode.Left == toRep.Up 	UnclNode = GrandNode.Right
		else				 UnclNode = GrandNode.Left

		if UnclNode != null UnclColorB = UnclNode.color == RBColorBlack
		
		if not UnclColorB {
			toRep.Up.color = RBColorBlack
			UnclNode.color = RBColorBlack
			GrandNode.color = RBColorRed
			RBRepair(GrandNode)
			return void
		}

		if GrandNode.Left == toRep.Up and toRep.Up.Right == toRep {
			RotateLeft(toRep.Up)
		}
		if GrandNode.Right == toRep.Up and toRep.Up.Left == toRep {
			RotateRight(toRep.Up)
		}
		if GrandNode.Left != null and GrandNode.Left.color == RBColorRed{
			RotateRight(GrandNode)
		}else{
			RotateLeft(GrandNode)
		}
		GrandNode.color = RBColorRed
		GrandNode.Up.color = RBColorBlack
	}
}
