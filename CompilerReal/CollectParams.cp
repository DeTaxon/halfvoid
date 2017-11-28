
CollectParams := !(Object^ start) -> void
{
	Bag := Stack.{Object^}

	Bag.Push(start)

	while Bag.NotEmpty()
	{
		Curr := Bag.Pop()

		if Curr.GetValue() == "i:=0"
		{
			printf("item %s\n",Curr.Down.Right.Right.GetValue())
		}else{
		      if Curr.Down  != null Bag.Push(Curr.Down)
		      if Curr.Right != null Bag.Push(Curr.Right)
		}
	}
}
