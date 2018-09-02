#import "arrs.cp"

NonDefNodeLines := class
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
	Lines := NonDefNodeLines[]
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
	nodeType := int
	nodeValue := int
	Down,Right:= LexTreeNode^
	this := !(int type) -> void
	{
		nodeType = type
		Down = null
		Right = null
	}
	this := !(int type,int cal) -> void
	{
		nodeType = type
		nodeValue = cal
		Down = null
		Right = null
	}
	Print := !(int s) -> void
	{	
		for s printf("->")
		printf("node %c %c\n",nodeType,nodeValue)
		it3 := Down
		while it3 != null
		{
			it3^.Print(s+1)
			it3 = it3.Right
		}
	}

}
UNextLex := !(LexTreeNode^ prev, int nType, int nVal,int Size) -> LexTreeNode^
{
	preEnd := prev
	frst := prev.Right
	for Size preEnd = preEnd.Right
	Last := preEnd.Right

	preEnd.Right = null
	prev.Right = new LexTreeNode(nType,nVal)
	prev.Right.Right = Last
	prev.Right.Down = frst
	
	return prev.Right
}

CheckRule := !(int[@S] rule,int res, LexTreeNode^ prev) -> bool
{
	gotSome := false
	itr := prev.Right
	prv := prev
	while itr != null{
		c := itr
		siz := 0
		failed := false
		for i : S
		{
			if c == null return gotSome
			if rule[i] == '3'
			{	
				if c.nodeType in "23" {
					siz += 1
				}else failed = true
			}else{
				if c.nodeType == '1' and rule[i] == c.nodeValue 
				{
					siz += 1
				}else{
					if rule[i] == '4'
					{
						if c.Right == null return gotSome
						if c.nodeType in "23"
						{
							while c.Right.nodeType in "23"
							{
								c = c.Right
								siz += 1
							}
							siz += 1
						}else failed = true
					}else{
						failed = true
					}
				}
			}
			c = c.Right
		}

		if not failed 
		{
			itr = UNextLex(prv,'3',res,siz)
			gotSome = true
		}
		else
		{
			prv = itr
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
		ApplyReg(regEx,'1')
	}
	ApplyReg := !(string regEx, int val) -> void
	{
		Nfas.Emplace()

		nowItm := ref Nfas[0]

		Words := new LexTreeNode(0)
		iter := Words

		i := 0
		while regEx[i] != 0
		{
			if regEx[i] in "+-*[]()|"
			{
				iter.Right = new LexTreeNode('1',regEx[i])
				iter = iter.Right
			}else{
				if regEx[i] == '\\'
				{
					iter.Right = new LexTreeNode('2',regEx[i+1])
					iter = iter.Right
					i += 1
				}else{
					if regEx[i] != ' '{
						iter.Right = new LexTreeNode('2',regEx[i])
						iter = iter.Right
					}
				}
			}
			i += 1
		}

		i3 := Words.Right
		while i3 != null
		{
			printf("Node %c %i\n",i3.nodeType,i3.nodeValue)
			i3 = i3.Right
		}
		printf("---------\n")

		while true
		{
			if CheckRule(!['3','-','3'],'-',Words) continue
			if CheckRule(!['3','3'],'&',Words)  continue 
			if CheckRule(!['3','|','3'],'|',Words)  continue 
			if CheckRule(!['(','3',')'],'(',Words)  continue 
			if CheckRule(!['[','4',']'],'[',Words)  continue 
			if CheckRule(!['3','+'],'0',Words)  continue 
			if CheckRule(!['3','*'],'*',Words)  continue 
			if CheckRule(!['3','?'],'?',Words)  continue 
			break
		}
		it3 := Words.Right
		while it3 != null
		{
			it3^.Print(0)
			it3 = it3.Right
		}

	}
}



