RBColorBlack := 0
RBColorRed := 1

RBTree := class .{@DATA} extend CommonTree.{DATA}
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
			Start.balance = RBColorBlack
			toRet^ = Start
			return true
		}
		prev := Start
		iter := Start
		
		while iter != null
		{
			setTest := iter.data <=> dat
			if setTest == 0{
				break
			}else{
				prev = iter
			}
			if setTest < 0	 	iter = iter.Right
			else 			iter = iter.Left
		}
		if iter != null {
			toRet^ = iter
			return false
		}
		
		if prev.data <=> dat < 0 {
			prev.Right = createNode()
			prev.Right.balance = RBColorRed
			prev.Right.Up = prev
			iter = prev.Right
		}else{
			prev.Left = createNode()
			prev.Left.balance = RBColorRed
			prev.Left.Up = prev
			iter = prev.Left
		}
		
		RBRepair(iter)

		toRet^ = iter
		return true
	}
	RBRepair := !(CommonTreeNode.{DATA}^ toRep) -> void
	{
		if toRep.Up == null
		{
			toRep.balance = RBColorBlack
			return void
		}
		if toRep.Up.balance == RBColorBlack return void

		GrandNode := toRep.Up.Up

		UnclColorB := true
		UnclNode := CommonTreeNode.{DATA}^
		UnclNode = null
		if GrandNode.Left == toRep.Up 	UnclNode = GrandNode.Right
		else				 UnclNode = GrandNode.Left

		if UnclNode != null UnclColorB = UnclNode.balance == RBColorBlack
		
		if not UnclColorB {
			toRep.Up.balance = RBColorBlack
			UnclNode.balance = RBColorBlack
			GrandNode.balance = RBColorRed
			RBRepair(GrandNode)
			return void
		}

		if GrandNode.Left == toRep.Up and toRep.Up.Right == toRep {
			RotateLeft(toRep.Up)
		}
		if GrandNode.Right == toRep.Up and toRep.Up.Left == toRep {
			RotateRight(toRep.Up)
		}
		if GrandNode.Left != null and GrandNode.Left.balance == RBColorRed{
			RotateRight(GrandNode)
		}else{
			RotateLeft(GrandNode)
		}
		GrandNode.balance = RBColorRed
		GrandNode.Up.balance = RBColorBlack
	}
	RemoveNode := !(CommonTreeNode.{DATA}^ toRem) -> void
	{
		if toRem.Left == null and toRem.Right == null
		{
			fth := toRem.Up
			if fth != null
			{
				if fth.Left == toRem
				{
					fth.Left = null
					if toRem.balance == RBColorRed return void
					RebalanceDown(fth,null)
				}else{
					fth.Right = null
					if toRem.balance == RBColorRed return void
					RebalanceDown(fth,null)
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
				Start.balance = RBColorBlack
			}else{
				fth := toRem.Up
				if fth.Left == toRem
				{
					fth.Left = toRem.Right
					toRem.Right.Up = fth
					if toRem.balance == RBColorRed return void
					if fth.Left.balance == RBColorRed
					{
						fth.Left.balance = RBColorBlack
						return void
					}
					RebalanceDown(fth,fth.Left)
				}else{
					fth.Right = toRem.Right
					toRem.Right.Up = fth
					if toRem.balance == RBColorRed return void
					if fth.Right.balance == RBColorRed
					{
						fth.Right.balance = RBColorBlack
						return void
					}
					RebalanceDown(fth,fth.Right)
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
				Start.balance = RBColorBlack
			}else{
				fth := toRem.Up
				if fth.Left == toRem
				{
					fth.Left = toRem.Left
					toRem.Left.Up = fth
					if toRem.balance == RBColorRed return void
					if fth.Left.balance == RBColorRed
					{
						fth.Left.balance = RBColorBlack
						return void
					}
					RebalanceDown(fth,fth.Left)
				}else{
					fth.Right = toRem.Left
					toRem.Left.Up = fth
					if toRem.balance == RBColorRed return void
					if fth.Right.balance == RBColorRed
					{
						fth.Right.balance = RBColorBlack
						return void
					}
					RebalanceDown(fth,fth.Right)
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
		
		tmpCol := cand.balance
		cand.balance = toRem.balance
		toRem.balance = tmpCol
		RemoveNode(toRem)
	}
	RebalanceDown := !(CommonTreeNode.{DATA}^ par, CommonTreeNode.{DATA}^ Repl) -> void
	{
		//par  - unknown
		//Repl - must be RBColorBlack

		if par == null return void

		sib := CommonTreeNode.{DATA}^
		if par.Left == Repl {
			sib = par.Right
		} else {
			sib = par.Left
		}

		if sib.balance == RBColorRed
		{
			par.balance = RBColorRed
			sib.balance = RBColorBlack
			if sib == par.Left
			{
				RotateRight(par)
			}else{
				RotateLeft(par)
			}
			if par.Left == Repl
			{
				sib = par.Right
			}else{
				sib = par.Left
			}
		}
		Sl := RBColorBlack
		Sl = sib.Left?.balance
		Sr := RBColorBlack
		Sr = sib.Right?.balance
		if par.balance == RBColorBlack 
		{	

			if Sl == Sr and Sl == RBColorBlack
			{
				sib.balance = RBColorRed
				RebalanceDown(par.Up,par)
				return void
			}
		}
		if par.balance == RBColorRed and Sl == Sr and Sr == RBColorBlack
		{
			sib.balance = RBColorRed
			par.balance = RBColorBlack
			return void
		}

		if par.Left == sib
		{
			if Sr == RBColorRed
			{
				RotateLeft(sib)
				sib.balance = RBColorRed
				sib.Up.balance = RBColorBlack
				sib = sib.Up
			}
			sib.Left.balance = RBColorBlack
			sib.balance = sib.Up.balance
			sib.Up.balance = RBColorBlack
			RotateRight(par)

		}else{
			if Sl == RBColorRed
			{
				RotateRight(sib)
				sib.balance = RBColorRed
				sib.Up.balance = RBColorBlack
				sib = sib.Up
			}
			sib.Right.balance = RBColorBlack
			sib.balance = sib.Up.balance
			sib.Up.balance = RBColorBlack
			RotateLeft(par)
		}
	}
}
