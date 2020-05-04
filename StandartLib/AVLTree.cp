
AVLTree := class .{@DATA} extend CommonTree.{DATA}
{
	$keep
	
	this := !() -> void
	{
		Start = null
		if $keep
			CreatedNodes = null
	}
	FindOrCreate := !(DATA dat,CommonTreeNode.{DATA}^^ toRet) .{} -> bool //bool isCreated
	{
		if Start == null 
		{
			Start = this.createNode()
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
			if  cmpRes < 0 		iter = iter.Right
			else 			iter = iter.Left
		}
		if iter != null {
			toRet^ = iter
			return false
		}
		
		if (prev.data <=> dat) < 0{
			prev.Right = createNode()
			prev.Right.Up = prev
			iter = prev.Right
		}else{
			prev.Left = createNode()
			prev.Left.Up = prev
			iter = prev.Left
		}
		iter.data = dat
		
		AVLRepair(iter)

		toRet^ = iter
		return true
	}
	AVLRepair := !(CommonTreeNode.{DATA}^ toRep) -> void
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
	RemoveNode := !(CommonTreeNode.{DATA}^ toRem) .{} -> void
	{
		if toRem.Left == null and toRem.Right == null
		{
			fth := toRem.Up
			if fth != null
			{
				if fth.Left == toRem
				{
					fth.Left = null
					RebalanceDown(fth,true)
				}else{
					fth.Right = null
					RebalanceDown(fth,false)
				}
			}else{
				Start = null
			}
			remNode(toRem)
			return void
		}
		if toRem.Left == null
		{
			if toRem.Up == null
			{
				Start = toRem.Right
				Start.Up = null
			}else{
				fth := toRem.Up
				if fth.Left == toRem
				{
					fth.Left = toRem.Right
					toRem.Right.Up = fth
					RebalanceDown(fth,true)
				}else{
					fth.Right = toRem.Right
					toRem.Right.Up = fth
					RebalanceDown(fth,false)
				}
			}
			remNode(toRem)
			return void
		}
		if toRem.Right == null
		{
			if toRem.Up == null
			{
				Start = toRem.Left
				Start.Up = null
			}else{
				fth := toRem.Up
				if fth.Left == toRem
				{
					fth.Left = toRem.Left
					toRem.Left.Up = fth
					RebalanceDown(fth,true)
				}else{
					fth.Right = toRem.Left
					toRem.Left.Up = fth
					RebalanceDown(fth,false)
				}
			}
			remNode(toRem)
			return void
		}
		//else Left != null and Right != null

		cand := toRem.Right
		while cand.Left != null
			cand = cand.Left

		cUp := cand.Up
		remUp := toRem.Up
		if cUp.Left == cand
		{
			cUp.Left = toRem
		}else{
			cUp.Right = toRem
		}
		if remUp != null
		{
			if remUp.Left == toRem
			{
				remUp.Left = cand
			}else{
				remUp.Right = cand
			}
		}else{
			Start = cand
		}
		toRem.Up = cUp
		cand.Up = remUp

		cand.Left = toRem.Left
		toRem.Left = null

		tmp := toRem.Right
		toRem.Right = cand.Right
		cand.Right = tmp

		if cand.Left != null cand.Left.Up = cand
		if cand.Right != null cand.Right.Up = cand
		if toRem.Right != null toRem.Right.Up = toRem

		cand.balance = toRem.balance

		//swap(toRem.Up,cand.Up)
		//swap(toRem.Left,cand.Left)
		//swap(toRem.Right,cand.Right)
		//printf("swapped\n")
		//Start.PrintIt(0)
		RemoveNode(toRem)
	}
	RebalanceDown := !(CommonTreeNode.{DATA}^ toBal,bool leftPull) -> void
	{
		while true
		{
			if toBal == null return void
			fth := toBal.Up
			if leftPull
			{
				switch toBal.balance
				{
					case -1
						toBal.balance++
						if fth != null {
							leftPull = fth.Left == toBal
							toBal = fth
						}else{
							return void
						}
					case 0
						toBal.balance++
						return void
					case 1
						//TODO: rotations
						sib := toBal.Right
						if sib.balance == -1
						{
							zNode := sib.Left
							RotateRight(sib)
							RotateLeft(toBal)
							
							switch zNode.balance
							{
							case -1
								sib.balance = 1
								toBal.balance = 0
							case 0
								sib.balance = 0
								toBal.balance = 0
							case 1
								sib.balance = 0
								toBal.balance = -1
							}
							zNode.balance = 0

							if zNode.Up == null return void
							toBal = zNode.Up
							leftPull = zNode.Up.Left == zNode
						}else{
							oldSibB := sib.balance
							oldVB := toBal.balance
							RotateLeft(toBal)
							if sib.balance == 0
							{
								toBal.balance = 1
								sib.balance = -1
							}else{
								toBal.balance = 0
								sib.balance = 0
							}
							if oldVB == 1 and oldSibB != 1 return void
							if sib.Up == null return void
							leftPull = sib.Up.Left == sib
							toBal = sib.Up
						}
				}
			}else{
				switch toBal.balance
				{
					case -1
						sib := toBal.Left
						if sib.balance == 1
						{
							zNode := sib.Right
							RotateLeft(sib)
							RotateRight(toBal)

							switch zNode.balance
							{
							case 1
								toBal.balance = 0
								sib.balance = -1
							case 0
								toBal.balance = 0
								sib.balance = 0
							case -1
								toBal.balance = 1
								sib.balance = 0
							}
							zNode.balance = 0

							if zNode.Up == null return void
							leftPull = zNode.Up.Left == zNode
							toBal = zNode.Up

						}else{
							oldSibB := sib.balance
							oldToBalB := toBal.balance
							if sib.balance == 0
							{
								toBal.balance = -1
								sib.balance = 1
							}else{
								toBal.balance = 0
								sib.balance = 0
							}
							RotateRight(toBal)
							if oldSibB != -1 and oldToBalB == -1 return void
							if sib.Up == null return void
							leftPull = sib.Up.Left == sib
							toBal = sib.Up
						}
					case 0
						toBal.balance--
						return void
					case 1
						toBal.balance--
						if fth != null
						{
							leftPull = fth.Left == toBal
							toBal = fth
						}else{
							return void
						}
				}
			}
		}
	}
}
