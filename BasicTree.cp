
BasicTree := class{
	Left,Right,Down,Up := BasicTree^

	SetUp := !(BasicTree^ nUp) -> void
	{
		Up = nUp
		if Right != null Right.SetUp(nUp)
	}
	"~this" := virtual !() -> void
	{
	}
}
UNext := !(BasicTree^ where,BasicTree^ nObj, int count) -> BasicTree^
{
	printf("wyyt %p %p %p %i\n",where, where.Right,nObj,count)
	Last2 := where
	for count-1 {
		printf("wuut %p %p\n",Last2, Last2.Right)
		Last2 = Last2.Right
	}
	UNext(where,nObj,Last2)
	return nObj
}
UNext := !(BasicTree^ where,BasicTree^ nObj, BasicTree^ Last) -> BasicTree^
{
	printf("maybe\n")
	End := Last.Right

	if End != null {
		End.Left.Right = null
		End.Left = nObj
		nObj.Right = End
	}

	if where.Left == null{
		Upp := where.Up

		Upp.Down = nObj
		nObj.Down = where
		nObj.Up = Upp

	}else{
		Ll := where.Left

		Ll.Right = nObj
		nObj.Left = Ll
		nObj.Down = where
		nObj.Up = Ll.Up
		where.Left = null
	}

	End = where
	while End != null {
		End.Up = nObj
		End = End.Right
	}
	return nObj
}
ReplaceNode := !(BasicTree^ what, BasicTree^ with) -> BasicTree^
{
	wiEnd := with

	while wiEnd.Right != null 
	{
		wiEnd.Up = what.Up
		wiEnd = wiEnd.Right
		if wiEnd.Line == null wiEnd.Line = what.Line
	}
	if wiEnd.Line == null wiEnd.Line = what.Line
	wiEnd.Up = what.Up

	if what.Left == null
	{
		if what.Up != null
		{
			what.Up.Down = with
		}
		with.Left = null
	}else{
		with.Left = what.Left
		with.Left.Right = with
	}

	wiEnd.Right = what.Right
	if wiEnd.Right != null wiEnd.Right.Left = wiEnd

	with.SetUp(what.Up)

	what.Up = null
	what.Left = null
	what.Right = null

	return with
}

PopOutNode := !(BasicTree^ what) -> void
{
	if what.Left != null
	{
		what.Left.Right = what.Right
		if what.Right != null	what.Right.Left = what.Left
	}else
	{
		if what.Up != null what.Up.Down = what.Right
		if what.Right != null what.Right.Left = null
	}
	what.Up = null
	what.Left = null
	what.Right = null
}
