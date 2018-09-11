#import "arrs.cp"
#import "BasicTree.cp"
#import "Bitset.cp"
#import "Set.cp"
#import "Pair.cp"

NonDetNodeLine := class
{
	from := int
	to := int
	symbl := int
	this := !(int fr, int tt , int s) -> void
	{
		from = fr
		to = tt
		symbl = s
	}
	"=" := !(NonDetNodeLine newVal) -> void
	{
		from = newVal.from
		to = newVal.to
		symbl = newVal.symbl
	}
}
NonDetMachine := class
{
	Lines := NonDetNodeLine[]
	EndNodeData := Pair.{int,int}[] // nodeId,resultId
	this := !() -> void
	{
		EndNodeData = null
		Lines = null
	}
	"~this" := !() -> void
	{
		delete EndNodeData
		delete Lines
	}
}

DetMachine := class
{
	table := int[][]
	NodeId := int[]
	IsEndNode := int[]
	this := !() -> void
	{
		table = null
		NodeId = null
		IsEndNode = null
	}
	"~this" := !() -> void
	{
		if table != null {
			for i : table->len delete table[i] //TODO: for table delete it //TODODO: delete table?.^
		}
		delete table
		delete NodeId
		delete IsEndNode
	}
}


CollectByEpsilon := !(NonDetMachine input,QueueSet.{int} OldStates) -> void
{
	for it : OldStates
	{	
		for line : input.Lines
		{
			if line.from == it and line.symbl == -1
				OldStates.Push(line.to)
		}
	}
}

DeterminateMachine := !(NonDetMachine input) -> DetMachine
{
	NewNodes := QueueSet.{Set.{int}}
	Letters := QueueSet.{int}
	nowNodeValue :=  QueueSet.{int}()

	for input.Lines
	{
		if it.symbl != -1 Letters.Push(it.symbl)
	}

	nowNodeValue.Push(0)
	CollectByEpsilon(input,nowNodeValue)

	res := Set.{int}()
	for nowNodeValue res.Add(it)
	NewNodes.Push(res)
	//nowNodeValue.Clean()

	//for NewNodes
	//{
	//	
	//}
}


LexTreeNode := class
{
	Left,Right,Up,Down := LexTreeNode^
	nodeType := int
	nodeValue := int
	nodeIdStart := int
	nodeIdEnd := int
	this := !(int type) -> void
	{
		nodeType = type
	}
	this := !(int type,int cal) -> void
	{
		nodeType = type
		nodeValue = cal
	}
	Print := !(int s) -> void
	{	
		for s printf("->")
		printf("Node %c %c\n",nodeType,nodeValue)
		if Down != null Down.Print(s+1)
		if Right != null Right.Print(s)
	}

}

CheckRule := !(int[@S] rule,int res, LexTreeNode^ nowNode) -> bool
{
	gotSome := false
	itr := nowNode
	while itr != null{
		c := itr
		siz := 0
		failed := false
		for i : S
		{
			if c == null return gotSome
			switch rule[i] //BUG: can not return/continue/break from switch
			{
				case '3'
					if c.nodeType in "2RSPMQBOA" {
						siz += 1
					}else failed = true
				case '4'
					if c.nodeType in "2RSPMQBOA^"
					{
						while c.Right.nodeType in "2RSPMQBOA^"
						{
							c = c.Right
							if c == null return gotSome
							siz += 1
						}
						siz += 1
					}else failed = true
				case void
					if c.nodeType == rule[i]
					{
						siz += 1
					}else failed = true
			}
			if failed break
			c = c.Right
		}

		if not failed 
		{
			newNd := new LexTreeNode(res)
			itr = UNext(itr,newNd,siz)
			gotSome = true
		}
		else
		{
			itr = itr.Right
		}
	}
	return gotSome	
}

LexBuilder := class
{
	Nfas := Stack.{NonDetMachine}
	ApplyReg := !(string regEx) -> void
	{
		ApplyReg(regEx,1)
	}
	ApplyReg := !(string regEx, int val) -> void
	{

		Words := new LexTreeNode(0)
		iter := Words

		i := 0
		while regEx[i] != 0
		{
			switch regEx[i] // BUG: can not use switch(regEx[i])
			{
				case "+-*[]()|?^"
					iter.Right = new LexTreeNode(regEx[i],' ')
					iter.Right.Left = iter
					iter = iter.Right
				case '\\'
					iter.Right = new LexTreeNode('2',regEx[i+1])
					iter.Right.Left = iter
					iter = iter.Right
					i += 1
				case ' '
					//empty
				case void 
					iter.Right = new LexTreeNode('2',regEx[i])
					iter.Right.Left = iter
					iter = iter.Right

			}
			i += 1
		}

		//i3 := Words.Right->{LexTreeNode^}
		//while i3 != null
		//{
		//	printf("Node %c %c\n",i3^.nodeType,i3^.nodeValue)
		//	i3 = i3.Right
		//}
		//printf("---------\n")

		while true
		{
			if CheckRule(!['3','-','3'],'R',Words.Right) continue
			if CheckRule(!['[','4',']'],'S',Words.Right)  continue 
			if CheckRule(!['[','^','4',']'],'S',Words.Right)  continue 
			if CheckRule(!['3','+'],'P',Words.Right)  continue 
			if CheckRule(!['3','*'],'M',Words.Right)  continue 
			if CheckRule(!['3','?'],'Q',Words.Right)  continue 
			if CheckRule(!['3','3'],'A',Words.Right)  continue 
			if CheckRule(!['3','|','3'],'O',Words.Right)  continue 
			if CheckRule(!['(','3',')'],'B',Words.Right)  continue 
			break
		}
		Words.Right.Print(0)

		Nfas.Emplace()
		//nowItm := ref Nfas[0]
		nowNodes := Stack.{NonDetNodeLine}()
		itrId := 1
		mstart := int
		mend := int
		BuildPartOfNode(nowNodes,Words.Right,itrId&,mstart&,mend&)
		AddNodeLine(nowNodes,0,mstart,-1)
		Nfas[0].Lines = nowNodes.ToArray()
		Nfas[0].EndNodeData = new Pair.{int,int}[1]
		Nfas[0].EndNodeData[0] = Pair.{int,int}(mend,val)

		DeterminateMachine(Nfas[0])
	}
}

AddNodeLine := !(Stack.{NonDetNodeLine} lines,int frm,int toNode, int val) -> void
{
	lines.Emplace()
	lines[0].from = frm
	lines[0].to = toNode
	lines[0].symbl = val
}

BuildPartOfNode := !(Stack.{NonDetNodeLine} lines,LexTreeNode^ nd,int^ itr,int^ itStart, int^ itEnd) -> bool 
{
	switch nd.nodeType
	{
		case 'R' { return false }
		case 'O'{
			reses := int[4]
			BuildPartOfNode(lines,nd.Down,itr,reses[0]&,reses[1]&)
			BuildPartOfNode(lines,nd.Down.Right.Right,itr,reses[2]&,reses[3]&)
			itStart^ = itr^++
			itEnd^ = itr^++

			AddNodeLine(lines,itStart^,reses[0],-1)
			AddNodeLine(lines,itStart^,reses[1],-1)
			AddNodeLine(lines,reses[2],itEnd^,-1)
			AddNodeLine(lines,reses[3],itEnd^,-1)

			return true
		}
		case 'Q'{
			BuildPartOfNode(lines,nd.Down,itr,itStart,itEnd)
			AddNodeLine(lines,itStart^,itEnd^,-1)
			return true
		}
		case 'M'{
			reses := int[2]
			itStart^ = itr^++
			itEnd^ = itr^++
			BuildPartOfNode(lines,nd.Down,itr,reses[0]&,reses[1]&)
			AddNodeLine(lines,itStart^,itEnd^,-1)
			AddNodeLine(lines,reses[1],reses[0],-1)
			AddNodeLine(lines,itStart^,reses[0],-1)
			AddNodeLine(lines,reses[1],itEnd^,-1)
			return true
		}
		case 'P'{
			BuildPartOfNode(lines,nd.Down,itr,itStart,itEnd)

			AddNodeLine(lines,itEnd^,itStart^,-1)
			return true
		}
		case 'A'{
			someLine := int[2]
			BuildPartOfNode(lines,nd.Down,itr,itStart,someLine[0]&)
			BuildPartOfNode(lines,nd.Down.Right,itr,someLine[1]&,itEnd)

			AddNodeLine(lines,someLine[0],someLine[1],-1)
			return true
		}
		case '2'{
			itStart^ = itr^++
			itEnd^ = itr^++
			AddNodeLine(lines,itStart^,itEnd^,nd.nodeValue)
			return true
		}
		case 'B'{
			return BuildPartOfNode(lines,nd.Down.Right,itr,itStart,itEnd)
		}
		case 'S'
		{
			itStart^ = itr^++
			itEnd^ = itr^++
			nowSet := Bitset.{32}()
			reverseIt := false
			
			iterat := nd.Down

			while iterat != null
			{
				switch iterat.nodeType
				{
					case '2'{
						nowSet << iterat.nodeValue
					}
					case '^'{
						reverseIt = true
					}
					case 'R'{
						itmStart := iterat.Down.nodeValue
						itmEnd := iterat.Down.Right.Right.nodeValue
						nowSet << itmStart..itmEnd
					}
				}
				iterat = iterat.Right
			}
			if reverseIt nowSet.Inverse()

			for nowSet
			{
				AddNodeLine(lines,itStart^,itEnd^,it)
			}
			return true
		}
	}
	return true	
}


