

ReplaceKeywords := !(char^ txt, char^^ keys,int keysCount,!(StringSpan,int)&->char^ cb) -> char^
{
	preRes := ""sbt
	states := new Tuple.{int,int}[keysCount] ; &temp
	
	for i : keysCount
	{
		states[i].1 = StrSize(keys[i])
	}

	lastDumped := 0
	s := 0
	while txt[s] != 0
	{
		for st,i : states
		{
			if txt[s] == keys[i][st.0]
			{
				st.0 += 1
				if st.0 == st.1
				{
					endSize := s - lastDumped - st.1 + 1
					if endSize > 0
						preRes << txt[lastDumped..endSize]
					//preRes << cb(txt[(s-st.1)..st.1])
					preRes << cb(txt[(s-st.1+1)..st.1],s)
					lastDumped = s + 1
				}
			}else{
				st.0 = 0
			}
		}
		s++
	}
	if s != lastDumped
	{
		preRes << txt[lastDumped..(s - lastDumped)]
	}
	return preRes.Str() ; $temp
}

main := !() -> int
{
	keyv := new char^[3]
	keyv[0] = "Hello"
	keyv[1] = "friend"
	keyv[2] = "meet"

	t := "Hello Greg, nice to meet you friend of mine"
	res := ReplaceKeywords(t,keyv,3,(x,y) ==>{
		switch x
		{
			case "Hello"
				return "Good to see you"
			case "friend"
				return "idiot"
			case "meet"
				return "meat"
		}
		return "BOB"
	})

	printf("res <%s>\n",res)
	return 0
}


//tst := !(@T... args) -> T
//{
//	return 0.0
//}
//
//main := !() -> int
//{
//	x := tst(1.0f,3.0f,4)
//	return 0
//}
//
