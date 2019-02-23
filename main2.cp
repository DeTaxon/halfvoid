


LikeAStack := class
{
	x := int
	Push := !(a) .{} -> void
	{
		printf("wow\n")
	}
	Insert := !(a) -> void
	{	
		Push(a)
	}
}


main := !(int argc, char^^ argv) -> int
{
	asS := LikeAStack
	asS.Insert(1)
	//asS := new snikers ; $temp
	//asS.st.Push(1) 
	//asS := new LikeAStack ; $temp
	//asS.Push(1) 


	return 0
}

