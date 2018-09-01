#import "lib.cp"
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
		//delete Lines //Todo: unbug
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
	Down,Right := LexTreeNode^
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


LexBuilder := class
{
	Nfas := Stack.{NonDefMachine}
	ApplyReg := !(string regEx) -> void
	{
		ApplyReg(regEx,1)
	}
	ApplyReg := !(string regEx, int val) -> void
	{
	}
}



