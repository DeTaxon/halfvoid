#import "arrs.cp"
#import "BasicTree.cp"

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


LexTreeNode := class extend BasicTree
{
	nodeType := int
	nodeValue := int
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
		it3 := Down
		printf("Node %c %c\n",nodeType,nodeValue)
		while it3 != null
		{
			it3->{LexTreeNode^}^.Print(s+1)
			it3 = it3.Right
		}
	}

}

CheckRule := !(int[@S] rule,int res, LexTreeNode^ nowNode) -> bool
{
	gotSome := false
	itr := nowNode
	while itr != null{
		c := itr->{LexTreeNode^}
		siz := 0
		failed := false
		for i : S
		{
			if c == null return gotSome
			if c.Right == null return gotSome
			switch rule[i] //BUG: can not return/continue/break from switch
			{
				case '3'
					if c.nodeType in "23" {
						siz += 1
					}else failed = true
				case '4'
					if c.nodeType in "23"
					{
						while c.Right->{LexTreeNode^}.nodeType in "23"
						{
							c = c.Right->{LexTreeNode^}
							siz += 1
						}
						siz += 1
					}else failed = true
				case void
					if c.nodeType == '1'
					{
						siz += 1
					}else failed = true
			}
			c = c.Right->{LexTreeNode^}
		}

		if not failed 
		{
			newNd := new LexTreeNode('3')
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
			switch regEx[i] // BUG: can not use switch(regEx[i])
			{
				case "+-*[]()|"
					iter.Right = new LexTreeNode('1',regEx[i])->{BasicTree^}
					iter.Right.Left = iter->{BasicTree^}
					iter = iter.Right->{LexTreeNode^}
				case '\\'
					iter.Right = new LexTreeNode('2',regEx[i+1])->{BasicTree^}
					iter.Right.Left = iter->{BasicTree^} //BUG:auto type to BasicTree not working
					iter = iter.Right->{LexTreeNode^}
					i += 1
				case ' '
					//empty
				case void 
					iter.Right = new LexTreeNode('2',regEx[i])->{BasicTree^}
					iter.Right.Left = iter->{BasicTree^}
					iter = iter.Right->{LexTreeNode^}

			}
			i += 1
		}

		i3 := Words.Right->{LexTreeNode^}
		while i3 != null
		{
			printf("Node %c %i\n",i3^.nodeType,i3^.nodeValue)
			i3 = i3.Right->{LexTreeNode^}
		}
		printf("---------\n")

		while true
		{
			if CheckRule(!['3','-','3'],'-',Words.Right) continue
			if CheckRule(!['3','3'],'&',Words.Right)  continue 
			if CheckRule(!['3','|','3'],'|',Words.Right)  continue 
			if CheckRule(!['(','3',')'],'(',Words.Right)  continue 
			if CheckRule(!['[','4',']'],'[',Words.Right)  continue 
			if CheckRule(!['3','+'],'0',Words.Right)  continue 
			if CheckRule(!['3','*'],'*',Words.Right)  continue 
			if CheckRule(!['3','?'],'?',Words.Right)  continue 
			break
		}
		it3 := Words.Right
		while it3 != null
		{
			it3->{LexTreeNode^}^.Print(0)
			it3 = it3.Right
		}

	}
}



