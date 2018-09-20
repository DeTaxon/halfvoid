AVLTreeNode := class .{@DATA}
{
	data := DATA
	balance := s8
	Left,Right,Up := AVLTreeNode.{DATA}^
	this := !() -> void
	{
		Left = null
		Right = null
		Up = null
		balance = 0
	}
	PrintIt := !(int i) -> void
	{
		for i printf("-")
		printf("%i %i\n",data,balance)
		if Left != null Left.PrintIt(i+1)
		if Right != null Right.PrintIt(i+1)
	}
}

AVLTree := class .{@DATA}
{
	Start := AVLTreeNode.{DATA}^
	
	this := !() -> void
	{
		Start = null
	}
	FindNode := !(DATA dat) -> RBTreeNode.{DATA}^
	{
		return null
	}
	FindOrCreate := !(DATA dat,AVLTreeNode.{DATA}^^ toRet) -> bool //bool isCreated
	{
		if Start == null 
		{
			Start = new AVLTreeNode.{DATA}()
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
			prev.Right = new AVLTreeNode.{DATA}()
			prev.Right.Up = prev
			iter = prev.Right
		}else{
			prev.Left = new AVLTreeNode.{DATA}()
			prev.Left.Up = prev
			iter = prev.Left
		}
		
		AVLRepair(iter)

		toRet^ = iter
		return true
	}
	RotateLeft := !(AVLTreeNode.{DATA}^ toRotate) -> void
	{
		dwNode := toRotate.Right
		toRotate.Right = dwNode.Left
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
	RotateRight := !(AVLTreeNode.{DATA}^ toRotate) -> void
	{
		dwNode := toRotate.Left
		toRotate.Left = dwNode.Right
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
	AVLRepair := !(AVLTreeNode.{DATA}^ toRep) -> void
	{
		if toRep == null return void

		P := toRep.Up
		if P == null return void 
		if P.Left == toRep {
			if P.balance > 0{
				P.balance--
				return void
			}
			if P.balance == 0 {
				P.balance--
				AVLRepair(P)
				return void
			}
			if toRep.balance > 0{
				RotateLeft(toRep)
				RotateRight(P)
				if toRep.balance == 0{
					P.balance = 0
					P.Up.balance = 0
				}else{
					if toRep.balance > 0{
						P.Up.balance = 1
						toRep.balance = 0
					}else{
						P.Up.balance = 0
						toRep.balance = -1
					}
					
				}
				AVLRepair(P.Up)
			}else{
				RotateRight(P)
				if toRep.balance == 0{
					toRep.balance = 1
					P.balance = -1
				}else{
					toRep.balance = 0
					P.balance = 0
				}
				AVLRepair(P.Up)
			}
		}else{
			if P.balance < 0{
				P.balance++
				return void
			}
			if P.balance == 0 {
				P.balance++
				AVLRepair(P)
				return void
			}
			if toRep.balance < 0{
				RotateRight(toRep)
				RotateLeft(P)
				if toRep.balance == 0{
					P.balance = 0
					P.Up.balance = 0
				}else{
					if toRep.balance > 0{
						P.Up.balance = 0
						toRep.balance = 1
					}else{
						P.Up.balance = -1
						toRep.balance = 0
					}
				}
				AVLRepair(P.Up)
			}else{
				RotateLeft(P)
				if toRep.balance == 0{
					toRep.balance = -1
					P.balance = 1
				}else{
					toRep.balance = 0
					P.balance = 0
				}
				AVLRepair(P.Up)
			}
		}
	}
}
