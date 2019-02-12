
CheckVal := !() .{} -> void
{	
	if $temp {
		printf("Check temp\n")
	}else{
		printf("Check regular\n")
	}
}


LikeAStack := class
{
	x := int
	Push := !(a) -> void
	{
		if true
		{
			if $temp{
				printf("Push temp\n")
				CheckVal()
			}else{
				printf("Push regular\n")
				CheckVal()
			}
		}
	}
}

snikers := class
{
	st := LikeAStack
}

main := !(int argc, char^^ argv) -> int
{
	c := s64()
	printf("wut %i\n",c)
	//asS := new snikers ; $temp
	//asS.st.Push(1) 
	//asS := new LikeAStack ; $temp
	//asS.Push(1) 


	return 0
}

