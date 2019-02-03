
CheckVal := !(a) -> void
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
		if $temp{
			printf("Push temp\n")
		CheckVal(a)
		}else{
			printf("Push regular\n")
		CheckVal(a)
		}
	}
}

snikers := class
{
	st := LikeAStack
}

main := !(int argc, char^^ argv) -> int
{
	//asS := new snikers ; $temp
	//asS.st.Push(1) 
	asS := new LikeAStack ; $temp
	asS.Push(1) 


	return 0
}

