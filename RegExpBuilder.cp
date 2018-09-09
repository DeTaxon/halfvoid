#import "arrs.cp"
#import "BasicTree.cp"
#import "Bitset.cp"

NonDefNodeLine := class
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
}
NonDefMachine := class
{
	IsEndNode := int[]
	Lines := NonDefNodeLine[]
	this := !() -> void
	{
		IsEndNode = null
		Lines = null
	}
	"~this" := !() -> void
	{
		delete IsEndNode
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
					if c.nodeType in "23-+*([&?" {
						siz += 1
					}else failed = true
				case '4'
					if c.nodeType in "23"
					{
						while c.Right.nodeType in "23"
						{
							c = c.Right
							if c == null return gotSome
							siz += 1
						}
						siz += 1
					}else failed = true
				case void
					if c.nodeType == '1'
					{
						if c.nodeValue == rule[i] 
							siz += 1
						else failed = true
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
	Nfas := Stack.{NonDefMachine}
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
					iter.Right = new LexTreeNode('1',regEx[i])
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
			if CheckRule(!['3','-','3'],'-',Words.Right) continue
			if CheckRule(!['[','4',']'],'[',Words.Right)  continue 
			if CheckRule(!['[','^','4',']'],'[',Words.Right)  continue 
			if CheckRule(!['3','+'],'+',Words.Right)  continue 
			if CheckRule(!['3','*'],'*',Words.Right)  continue 
			if CheckRule(!['3','?'],'?',Words.Right)  continue 
			if CheckRule(!['3','3'],'&',Words.Right)  continue 
			if CheckRule(!['3','|','3'],'|',Words.Right)  continue 
			if CheckRule(!['(','3',')'],'(',Words.Right)  continue 
			break
		}
		Words.Right.Print(0)

		Nfas.Emplace()
		//nowItm := ref Nfas[0]
		nowNodes := Stack.{NonDefNodeLine}()
		itrId := int
		mstart := int
		mend := int
		BuildPartOfNode(nowNodes,Words.Right,itrId&,mstart&,mend&)
	}
}

BuildPartOfNode := !(Stack.{NonDefNodeLine} lines,LexTreeNode^ nd,int^ itr,int^ itStart, int^ itEnd) -> bool
{
	switch nd.nodeType
	{
		case '2'{
			itStart^ = itr^++
			itEnd^ = itr^++
			lines.Emplace()
			lines[0].from = itStart^
			lines[0].to = itEnd^
			lines[0].symbl = nd.nodeValue

			return true
		}
		case '('{
			return BuildPartOfNode(lines,nd.Down.Right,itr,itStart,itEnd)
		}
		case '['
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
					case '-'{
						itmStart := iterat.Down.nodeValue
						itmEnd := iterat.Down.Right.Right.nodeValue
						nowSet << itmStart..itmEnd
					}
				}
				iterat = iterat.Right
			}
		}
	}
}


