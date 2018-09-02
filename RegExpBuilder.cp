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

		gotWork := true
		while gotWork
		{
			gotWork = false
			
			itr := Words.Right
			prv := Words
			while itr != null{
				if itr.Right == null break
				if itr.nodeType in "23" and itr.Right.nodeType in "23"
				{
					itr = UNextLex(prv,'3','&',3)
					gotWork = true
				}else{
					prv = itr
					itr = itr.Right
				}
			}
			if gotWork continue

			itr = Words.Right
			prv = Words
			while itr != null{
				if itr.Right == null break
				if itr.Right.Right == null break
				if itr.nodeType in "23" and itr.Right.nodeType in "23"
				{
					itr = UNextLex(prv,'3','&',3)
					gotWork = true
				}else{
					prv = itr
					itr = itr.Right
				}
			}
		}

	}
}



