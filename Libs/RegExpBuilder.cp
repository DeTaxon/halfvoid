#import "arrs.cp"
#import "BasicTree.cp"
#import "Bitset.cp"
#import "Set.cp"
#import "Pair.cp"
#import "Vector.cp"

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
	Table := int[][]
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
		if Table != null {
			for i : Table->len delete Table[i] //TODO: for table delete it //TODODO: delete table?.^
		}
		delete Table
		delete NodeId
		delete IsEndNode
	}
	PrintIt := !() -> void
	{
		printf("     ")
		for IsEndNode->len printf("%4i ",it)
		printf("\n")
		printf("     ")
		for IsEndNode printf("%4i ",it)
		printf("\n")
		for i : NodeId->len
		{
			printf("%4i ",NodeId[i])
			for j : IsEndNode->len
			{	
				printf("%4i ",Table[j][i])
			}
			printf("\n")
		}
	}
}
//WordDetermMachine := class
//{
//	Table := int[][]
//	IsEndNode := int[]
//	CharToGo := u8[256]
//
//	PrintIt := !() -> void
//	{
//		printf("           ")
//		for IsEndNode->len printf("%4i ",it)
//		printf("\n")
//		printf("lines %3i  ",Table[0]->len)
//		for IsEndNode printf("%4i ",it)
//		printf("\n")
//		chStart := 0
//		chEnd := 0
//		while chStart < 256
//		{
//			while CharToGo[chEnd] == CharToGo[chStart] {
//				chEnd++
//				if chEnd == 256 break
//			}
//			newEnd := chEnd - 1
//			if chStart == newEnd
//			{
//				if IsPrintable(chStart) printf("     %c     ",chStart) else printf("   0x%02X    ",chStart)
//			}else{
//				if IsPrintable(chStart) printf("   %c",chStart) else printf("0x%02X",chStart)
//				printf("..")
//				if IsPrintable(newEnd) printf("%c   ",newEnd) else printf("0X%02X",newEnd)
//				printf(" ")
//			}
//
//			nodLine := CharToGo[chStart]->{int}
//
//			for j : IsEndNode->len
//			{
//				printf("%4i ",Table[j][nodLine])
//			}
//			printf("\n")
//
//			chStart = chEnd
//
//		}
//		//printf("test\n")
//		//for CharToGo printf("%i ",it)
//		//printf("\n")
//		//printf("maybe\n")
//		//for i : Table[0]->len
//		//{
//		//	printf("%4i ",0)
//		//	for j : IsEndNode->len
//		//	{	
//		//		printf("%4i ",Table[j][i])
//		//	}
//		//	printf("\n")
//		//}
//
//	}
//}

//MakeWordDetermMachine := !(DetMachine input) -> WordDetermMachine
//{
//	kindaHashed := Vector.{int}(input.NodeId->len)
//	borrow := Vector.{int}(input.NodeId->len)
//
//	for it : kindaHashed, i : 0
//	{
//		it = 0
//		for j : input.IsEndNode->len
//		{
//			it = it*5 + input.Table[j][i]
//		}
//	}
//
//
//	totalItems := QueueSet.{int}()
//	for i : borrow.Size() //BUG: borrow->len crashes
//	{
//		found := i
//		for j : (0..(i-1))
//		{
//			if kindaHashed[j] == kindaHashed[i]
//			{
//				gotDiff := false
//				for k : input.IsEndNode->len
//				{
//					if input.Table[k][i] != input.Table[k][j]{
//						gotDiff = true
//						break
//					}
//				}
//				if not gotDiff
//				{
//					found = j
//					break
//				}
//			}
//		}
//		borrow[i] = found
//		totalItems.Push(found)
//	}
//	newSize := totalItems.Size()
//	containFake := false
//	if input.NodeId->len != 256 { newSize += 1 containFake = true}
//
//	for ch : 256
//		ToRet.CharToGo[ch] = newSize - 1
//	for lt : input.NodeId , i : 0
//	{
//		posInT := totalItems.GetPos(borrow[i])
//		ToRet.CharToGo[lt] = posInT
//	}
//	ToRet.Table = new int[][input.IsEndNode->len]
//	for it : ToRet.Table, i : 0
//	{
//		it = new int[newSize]
//		for  frm : totalItems, j : 0 
//		{
//			it[j] = input.Table[i][frm]
//		}
//		if containFake
//		{
//			it[totalItems.Size()] = -1
//		}
//	}
//	ToRet.IsEndNode = new int[input.IsEndNode->len]
//	for a : ToRet.IsEndNode, b : input.IsEndNode a = b
//}


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
CollectMove := !(NonDetMachine input, int sId, QueueSet.{int} frm, QueueSet.{int} to) -> void
{
	for line : input.Lines
	{
		for fNode : frm
		{
			if line.from == fNode and line.symbl == sId
				to.Push(line.to)
		}		
	}
}

//DeterminateMachine := !(NonDetMachine input) -> DetMachine
//{
//	NewNodes := QueueSet.{QueueSet.{int}}()
//	Letters := QueueSet.{int}()
//	itLines := Stack.{Pair.{int,Pair.{int,int}}}()
//
//	for input.Lines
//	{
//		if it.symbl != -1 Letters.Push(it.symbl)
//	}
//	{
//		nowNodeValue :=  QueueSet.{int}()
//		nowNodeValue.Push(0)
//		CollectByEpsilon(input,nowNodeValue)
//		NewNodes <<< nowNodeValue
//	}
//
//	for itNode : NewNodes, nowId : 0
//	{
//		for c : Letters
//		{
//			nowNodeValue := QueueSet.{int}()
//			CollectMove(input,c,itNode,nowNodeValue)
//			CollectByEpsilon(input,nowNodeValue)
//
//			if nowNodeValue.Size() != 0
//			{
//				resId := NewNodes <<< nowNodeValue
//				//TODO: itLines.Emplace(!{nowId,resId,c})
//				itLines.Emplace()
//				itLines[0].first = nowId
//				itLines[0].second.first = resId
//				itLines[0].second.second = c
//			}
//		}
//	}
//	ToRet.IsEndNode = new int[NewNodes.Size()]
//	
//	for itN : ToRet.IsEndNode,nowSet : NewNodes, i : 0
//	{
//		itN = -1
//
//		for itVal : nowSet
//		{
//			for  EndNode : input.EndNodeData
//			{
//				if itVal == EndNode.first and EndNode.second > itN
//				{
//					itN = EndNode.second
//				}
//			}
//		}
//	}
//
//	ToRet.Table = new int[][NewNodes.Size()]
//	for ToRet.Table {
//		it = new int[Letters.Size()]
//		for itm : it itm = -1
//	}
//
//	for move : itLines
//	{
//		chId := Letters.GetPos(move.second.second)	
//		ToRet.Table[move.first][chId] = move.second.first
//	}
//
//	ToRet.NodeId = new int[Letters.Size()]
//	for it : Letters , i : 0
//	{
//		ToRet.NodeId[i] = it
//	}
//}
//MinimizeMachine := !(DetMachine input) -> DetMachine
//{
//	EndStates := QueueSet.{int}()
//	for input.IsEndNode 
//		EndStates.Push(it)
//	
//	NodeSets := new int[input.IsEndNode->len]
//	for it : input.IsEndNode, i : 0
//		NodeSets[i] = EndStates.GetPos(it)
//
//	setCheck := 0
//	setSize := EndStates.Size()
//	foundMin := false
//	while setCheck < setSize
//	{
//		for j : input.NodeId->len
//		{
//			gotSets := QueueSet.{int}()
//			for i : input.IsEndNode->len
//				if setCheck == NodeSets[i]
//				{
//					if input.Table[i][j] != -1
//						gotSets.Push(NodeSets[input.Table[i][j]])
//					else gotSets.Push(-1)
//				}
//			if gotSets.Size() >= 2
//			{	
//				for i : input.IsEndNode->len
//				if setCheck == NodeSets[i]
//				{
//					goTo := input.Table[i][j]
//					inSet := 0 
//					if goTo != -1
//					{
//						inSet = gotSets.GetPos(NodeSets[goTo])
//					}else{
//						inSet = gotSets.GetPos(-1)
//					}
//					if inSet != 0
//					{
//						NodeSets[i] = setSize + inSet - 1
//					}
//				}
//				setSize += gotSets.Size() - 1
//				foundMin = true
//			}
//		}
//		setCheck++
//		if setCheck == setSize and foundMin
//		{
//			foundMin = false
//			setCheck = 0
//		}
//	}
//	ToRet.Table = new int[][setSize]
//	for ToRet.Table
//	{
//		it = new int[input.NodeId->len]
//		for itm : it itm = -1
//	}
//	ToRet.NodeId = new int[input.NodeId->len]
//	for a : ToRet.NodeId, b : input.NodeId a = b
//	ToRet.IsEndNode = new int[setSize]
//	transformer := new int[NodeSets->len]
//	defer delete transformer
//	{
//		someIter := 0
//		usedSets := QueueSet.{int}()
//		itNode := Queue.{int}()
//		for i : NodeSets->len
//		{
//			if usedSets.Contain(NodeSets[i])
//			{
//				ps := usedSets.GetPos(NodeSets[i])
//				transformer[i] = itNode[ps]
//			}else{
//				transformer[i] = someIter
//				usedSets.Push(NodeSets[i])
//				itNode.Push(someIter)
//				someIter++
//			}
//		}
//	}
//
//	for i : NodeSets->len
//	{
//		fromNew := transformer[i]
//		ToRet.IsEndNode[fromNew] = input.IsEndNode[i]
//		for j : input.NodeId->len
//		{
//			toOld := input.Table[i][j]
//			if toOld != -1
//			{
//				toNew := transformer[toOld]
//				ToRet.Table[fromNew][j] = toNew
//			}
//		}
//	}
//	//printf("     ")
//	//for NodeSets printf("%3i ",it)
//	//printf("\n")
//	
//}

LexTreeNode := class
{
	Left,Right,Up,Down := LexTreeNode^
	nodeType := int
	nodeValue := int
	nodeIdStart := int
	nodeIdEnd := int
	this := !(int typ) -> void
	{
		nodeType = typ
	}
	this := !(int typ,int cal) -> void
	{
		nodeType = typ
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
	this := !() -> void
	{
		Nfas."this"()
	}
	ApplyReg := !(string regEx) -> void
	{
		ApplyReg(regEx,1)
	}
	ApplyReg := !(string regEx, int val) -> void
	{

		//Words := new LexTreeNode(0)
		//iter := Words

		//i := 0
		//while regEx[i] != 0
		//{
		//	switch regEx[i] // BUG: can not use switch(regEx[i])
		//	{
		//		case "+-*[]()|?^"
		//			iter.Right = new LexTreeNode(regEx[i],' ')
		//			iter.Right.Left = iter
		//			iter = iter.Right
		//		case '\\'
		//			switch regEx[i+1]
		//			{
		//			case 'n'
		//				iter.Right = new LexTreeNode('2',10)
		//				iter.Right.Left = iter
		//				iter = iter.Right
		//				i += 1
		//			case void
		//				iter.Right = new LexTreeNode('2',regEx[i+1])
		//				iter.Right.Left = iter
		//				iter = iter.Right
		//				i += 1
		//			}
		//		case ' '
		//			//empty
		//		case void 
		//			iter.Right = new LexTreeNode('2',regEx[i])
		//			iter.Right.Left = iter
		//			iter = iter.Right

		//	}
		//	i += 1
		//}


		//while true
		//{
		//	if CheckRule(!['3','-','3'],'R',Words.Right) continue
		//	if CheckRule(!['[','4',']'],'S',Words.Right)  continue 
		//	if CheckRule(!['[','^','4',']'],'S',Words.Right)  continue 
		//	if CheckRule(!['3','+'],'P',Words.Right)  continue 
		//	if CheckRule(!['3','*'],'M',Words.Right)  continue 
		//	if CheckRule(!['3','?'],'Q',Words.Right)  continue 
		//	if CheckRule(!['3','3'],'A',Words.Right)  continue 
		//	if CheckRule(!['3','|','3'],'O',Words.Right)  continue 
		//	if CheckRule(!['(','3',')'],'B',Words.Right)  continue 
		//	break
		//}
		////Words.Right.Print(0)

		//Nfas.Emplace()
		//nowNodes := Stack.{NonDetNodeLine}()
		//itrId := 1
		//mstart := int
		//mend := int
		//BuildPartOfNode(nowNodes,Words.Right,itrId&,mstart&,mend&)
		//AddNodeLine(nowNodes,0,mstart,-1)
		//Nfas[0].Lines = nowNodes.ToArray()
		//Nfas[0].EndNodeData = new Pair.{int,int}[1]
		//Nfas[0].EndNodeData[0] = Pair.{int,int}(mend,val)

	}
	GenerateMachine := !() -> WordDetermMachine
	{
		//if Nfas.Size() == 0 return void //TODO: assert? or throw
		//if Nfas.Size() == 1
		//{
		//	newMach := DeterminateMachine(Nfas[0])
		//	printf("------------determ-------\n")
		//	newMach.PrintIt()
		//	MinMach := MinimizeMachine(newMach)
		//	printf("-------------minim--------\n")
		//	MinMach.PrintIt()
		//	printf("-------------word--------\n")
		//	return MakeWordDetermMachine(MinMach)
		//}else
		//{
		//	newLines := Stack.{NonDetNodeLine}()
		//	newEnds := Stack.{Pair.{int,int}}()
		//	multiNode := NonDetMachine()
		//	nowOffset := 0

		//	for itM : Nfas
		//	{
		//		maxNodes := 0
		//		for lin : itM.Lines
		//		{
		//			newLines.Emplace()
		//			newLines[0].from = lin.from
		//			if lin.from != 0 newLines[0].from += nowOffset
		//			newLines[0].to = lin.to
		//			if lin.to != 0 newLines[0].to += nowOffset
		//			newLines[0].symbl = lin.symbl

		//			if maxNodes < lin.from maxNodes = lin.from
		//			if maxNodes < lin.to maxNodes = lin.to
		//			//maxNodes = max(maxNodes,lin.from)
		//			//maxNodes = max(maxNodes,lin.to)
		//		}
		//		for ends : itM.EndNodeData
		//		{
		//			newEnds.Emplace()
		//			newEnds[0].first = ends.first + nowOffset
		//			newEnds[0].second = ends.second
		//		}
		//		nowOffset += maxNodes + 1
		//	}
		//	multiNode.Lines = newLines.ToArray()
		//	multiNode.EndNodeData = newEnds.ToArray()
		//	newMach := DeterminateMachine(multiNode)
		//	MinMach := MinimizeMachine(newMach)
		//	return MakeWordDetermMachine(MinMach)
		//}
		return void
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
			AddNodeLine(lines,itStart^,reses[2],-1)
			AddNodeLine(lines,reses[1],itEnd^,-1)
			AddNodeLine(lines,reses[3],itEnd^,-1)

			return true
		}
		case 'Q'{
			reses := int[2]
			itStart^ = itr^++
			itEnd^ = itr^++
			
			BuildPartOfNode(lines,nd.Down,itr,reses[0]&,reses[1]&)
			AddNodeLine(lines,itStart^,itEnd^,-1)
			AddNodeLine(lines,itStart^,reses[0],-1)
			AddNodeLine(lines,reses[1],itEnd^,-1)
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


