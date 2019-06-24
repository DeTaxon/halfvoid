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
	TestSize := !(int i, !(int)&-> void toSay) -> void
	{
		if Left != null Left.TestSize(i+1,toSay)
		if Right != null Right.TestSize(i+1,toSay)
		if Left == null and Right == null toSay(i)
	}
	IsCorrect := !() -> bool
	{
		if Left != null and Left.data > data return false
		if Right != null and Right.data < data return false

		if Left != null and not Left.IsCorrect() return false
		if Right != null and not Right.IsCorrect() return false
		return true
	}
	MinSize := !() -> int
	{
		sizL := 0
		sizR := 0

		if Left != null sizL = Left.MinSize()
		if Right != null sizR = Right.MinSize()
		if sizL > sizR return sizR + 1
		return sizL + 1
	}
	CheckHeight := !(bool^ res) -> int
	{

		sizL := 0
		sizR := 0

		if Left != null sizL = Left.CheckHeight(res)
		if Right != null sizR = Right.CheckHeight(res)

		if sizR - sizL != balance res^ = false
		if balance >= 2 or balance <= -2 res^ = false
		if sizR > sizL return sizR + 1
		return sizL + 1
	}
}

AVLTree := class .{@DATA}
{
	Start := AVLTreeNode.{DATA}^
	
	this := !() -> void
	{
		Start = null
	}
	FindNode := !(DATA dat) -> AVLTreeNode.{DATA}^
	{
		iter := Start
		while iter != null
		{
			cmpRes := iter.data <=> dat
			if cmpRes == 0 return iter
			if cmpRes == 1 {
				iter = iter.Right
			}else{
				iter = iter.Left
			}
		}
		return null->{AVLTreeNode.{DATA}^}
	}
	FindOrCreate := !(DATA dat,AVLTreeNode.{DATA}^^ toRet) .{} -> bool //bool isCreated
	{
		if Start == null 
		{
			Start = new AVLTreeNode.{DATA}()
			toRet^ = Start
			Start.data = dat
			return true
		}
		prev := Start
		iter := Start
		
		while iter != null
		{
			cmpRes := iter.data <=> dat
			if cmpRes == 0{
				break
			}else{
				prev = iter
			}
			if  cmpRes == 1 	iter = iter.Right
			else 			iter = iter.Left
		}
		if iter != null {
			toRet^ = iter
			return false
		}
		
		if prev.data <=> dat == 1{
			prev.Right = new AVLTreeNode.{DATA}()
			prev.Right.Up = prev
			iter = prev.Right
		}else{
			prev.Left = new AVLTreeNode.{DATA}()
			prev.Left.Up = prev
			iter = prev.Left
		}
		iter.data = dat
		
		AVLRepair(iter)

		toRet^ = iter
		return true
	}
	RotateLeft := !(AVLTreeNode.{DATA}^ toRotate) -> void
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
	RotateRight := !(AVLTreeNode.{DATA}^ toRotate) -> void
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
			if toRep.balance > 0 {
				RotateLeft(toRep)
				RotateRight(P)
				if P.Up.balance == 0 {
					P.balance = 0
					toRep.balance = 0
				}else{
					if P.Up.balance > 0 {
						P.balance = 0
						toRep.balance = -1
					}else{
						P.balance = 1
						toRep.balance = 0
					}
				}
				P.Up.balance = 0
				return void
			}else{
				RotateRight(P)
				if toRep.balance == 0{
					toRep.balance = 1
					P.balance = -1
				}else{
					toRep.balance = 0
					P.balance = 0
				}
				return void
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
				if P.Up.balance == 0{
					P.balance = 0
					toRep.balance = 0
				}else{
					if P.Up.balance > 0 {
						P.balance = -1
						toRep.balance = 0
					}else{
						P.balance = 0
						toRep.balance = 1
					}
					
				}
				P.Up.balance = 0
				return void
			}else{
				RotateLeft(P)
				if toRep.balance == 0{
					toRep.balance = -1
					P.balance = 1
				}else{
					toRep.balance = 0
					P.balance = 0
				}
				return void
			}
		}
	}

	TryLeft := !(AVLTreeNode.{DATA}^^ toTry) -> void
	{
		if toTry^.Left == null {
			toTry^ = null	
		}else{
			toTry^ = toTry^.Left
			while toTry^.Right != null toTry^ = toTry^.Right
		}
	}
	TryRight := !(AVLTreeNode.{DATA}^^ toTry) -> void
	{
		if toTry^.Right == null {
			toTry^ = null	
		}else{
			toTry^ = toTry^.Right
			while toTry^.Left != null toTry^ = toTry^.Left
		}
	}
	RemoveNode := !(AVLTreeNode.{DATA}^ toRem) -> void
	{
		candToRepl := toRem

		if candToRepl.balance >= 0{
			TryRight(candToRepl&)
			if candToRepl == null{
				candToRepl = toRem
				TryLeft(candToRepl&)
			}
		}else{
			TryLeft(candToRepl&)
			if candToRepl == null{
				candToRepl = toRem
				TryRight(candToRepl&)
			}
		}
		Fath := toRem.Up

		if candToRepl == null
		{
			Fath = toRem
			if Fath == null {
				Start = null
			}else{
				RebalanceDown(toRem)
			}
			delete toRem
			return void
		}else{
			if candToRepl.balance != 0 {
				if candToRepl.balance > 0 {
					RotateLeft(candToRepl)
					candToRepl.Up.balance = -1
				}else{
					RotateRight(candToRepl)
					candToRepl.Up.balance = 1
				}
			}
			RebalanceDown(candToRepl)
			candToRepl.balance = toRem.balance
			candToRepl.Left = toRem.Left
			candToRepl.Right = toRem.Right
			oldUp := candToRepl.Up
			if oldUp != null{
				if oldUp.Left == candToRepl oldUp.Left = null
				if oldUp.Right == candToRepl oldUp.Right = null
			}
			candToRepl.Up = toRem.Up
			if candToRepl.Left != null candToRepl.Left.Up = candToRepl
			if candToRepl.Right != null candToRepl.Right.Up = candToRepl
			if candToRepl.Up == null Start = candToRepl
		}

	}
	RebalanceDown := !(AVLTreeNode.{DATA}^ toRem) -> void
	{
		P := toRem.Up
		if P == null return void

		if P.Left == toRem {
			if P.balance  < 0{
				P.balance++
				RebalanceDown(P)
				return void
			}
			if P.balance == 0 {
				P.balance++
				return void
			}
			RotateLeft(P)
			P.Up.balance++
		}else{
			if P.balance > 0 {
				P.balance--
				RebalanceDown(P)
				return void
			}
			if P.balance == 0{
				P.balance--
				return void
			}
			RotateRight(P)
			P.Up.balance--
		}


	}
}
