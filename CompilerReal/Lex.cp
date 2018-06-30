
Line := class {
	GoTo := char[256]
	Id := s8
	"=" := !(Line ot) -> void
	{
		for i : 256 this.GoTo[i] = ot.GoTo[i]
		this.Id = ot.Id
	}
}

Machine := class{
	Lines := Line^

	"this" := !() -> void
	{
		Lines = null
	}	
}


GenerateMachine := !(QueueSet.{string} Opers) -> Machine^
{
	begin := new Machine()
	
	forOpers := Queue.{Line}()
	// word-1 Hex-2 int-3 int_s-4 float-5 float_s-6 floate-7 floate_s-8 str 9 #-10 \n-11 char-12

	//0: begin
	//1: error
	//16: error.nodiscard
	//21: error revert 2
	
	//2: word 1

	//3: StartNum
	//4: HexPart
	//5: HexLoop  2
	//6: IntLoop  3
	//7: IntAndSuff  4
	//8: StartDouble 
	//9: IntAndDots 4 2
	//10: DoubleLoop  5
	//11: DoubleAndSuff 6
	//12: DoubleSciense 
	//13: DoubleScienseSign
	//14: DoubleSLoop 7
	//15: DoubleScienseAndSuffix 8

	//17: strloop 
	//18: ExtraChar
	//19: strEnd - 9

	//20: symbols - 10

	Size := 30 //bound check??

	//begin.Lines = new Line[Size]
	PreLines := Queue.{Line}()
	EmptyLine := Line
	for i : 256 EmptyLine.GoTo[i] = 1 
	for i : Size PreLines.Push(EmptyLine)

	PreLines[1].Id = -1
	PreLines[16].Id = -2
	
	for i : 256 PreLines[0].GoTo[i] = 16	

	//symbols
	for i : "{}[]()@$^&;?,|" PreLines[0].GoTo[i] = 20
	PreLines[20].Id = 10
	PreLines[0].GoTo['.'] = 21
	PreLines[21].GoTo['.'] = 21
	PreLines[21].Id = 10

	//char
	PreLines[0].GoTo['\''] = 24
	for i : !['a'..'z','A'..'Z'] PreLines[24].GoTo[i] = 25
	PreLines[24].GoTo['\\'] = 27
	for i : 255 PreLines[27].GoTo[i] = 25
	PreLines[25].GoTo['\''] = 26
	PreLines[26].Id = 12
	//opers

	iter := Opers.Start
	while iter != null
	{
		oper := iter.Data
		j := 0
		m := PreLines[0]&
		while oper[j] != 0
		{
			if m.GoTo[oper[j]] == 1 or m.GoTo[oper[j]] == 16 {
				newId := PreLines.Size()
				newLine := Line
				newLine.Id = 0
				for k : 256 newLine.GoTo[k] = 1
				if oper[j+1] == 0 
				{
					//printf("end? %s\n",oper)
					newLine.Id = 10
				}

				PreLines.Push(newLine)
				m.GoTo[oper[j]] = newId
			}

			m = PreLines[m.GoTo[oper[j]]]&
			if oper[j+1] == 0 m.Id = 10
			j += 1
		}
		iter = iter.Next
	}

	// string
	PreLines[0].GoTo['"'] = 17
	for i : 256 PreLines[17].GoTo[i] = 17
	PreLines[17].GoTo['"'] = 19
	PreLines[17].GoTo['\\'] = 18
	for i : 256 PreLines[18].GoTo[i] = 17
	PreLines[19].Id = 9
	

	// word id 1
	for i : !['a'..'z','A'..'Z','_','$','@','#'] PreLines[0].GoTo[i] = 2
	for i : !['a'..'z','A'..'Z','0'..'9','_'] 	PreLines[2].GoTo[i] = 2
	PreLines[2].Id = 1

	//number
	for i : !['0'..'9'] PreLines[0].GoTo[i] = 3
	for i : !['a'..'z','A'..'Z'] PreLines[3].GoTo[i] = 7
	//for i : !['0'..'9'] PreLines[3].GoTo[i] = 3
	
	PreLines[3].GoTo['x'] = 4
	PreLines[3].Id = 3
	
	for i : !['a'..'f','A'..'F','0'..'9','_'] PreLines[4].GoTo[i] = 5
	PreLines[4].Id = 4
	for i : !['a'..'f','A'..'F','0'..'9','_'] PreLines[5].GoTo[i] = 5
	PreLines[5].Id = 2
	
	for i : !['0'..'9'] PreLines[3].GoTo[i] = 6

	for i : !['0'..'9'] PreLines[6].GoTo[i] = 6
	PreLines[6].Id = 3

	for i : !['a'..'z','A'..'Z'] PreLines[6].GoTo[i] = 7
	for i : !['a'..'z','A'..'Z'] PreLines[7].GoTo[i] = 7
	PreLines[7].Id = 4

	PreLines[6].GoTo['.'] = 8
	PreLines[3].GoTo['.'] = 8
	
	PreLines[8].GoTo['.'] = 1
	PreLines[8].Id = 0
	PreLines[9].Id = 3

	for i : '0'..'9' PreLines[8].GoTo[i] = 10
	for i : '0'..'9' PreLines[10].GoTo[i] = 10
	PreLines[10].Id = 5
	
	for i : !['a'..'z','A'..'Z'] PreLines[10].GoTo[i] = 11
	for i : !['a'..'z','A'..'Z'] PreLines[11].GoTo[i] = 11
	PreLines[11].Id = 6

	for i : !['e'] PreLines[10].GoTo[i] = 12
	for i : !['-','+'] PreLines[12].GoTo[i] = 13
	for i : !['0'..'9'] PreLines[12].GoTo[i] = 14
	for i : !['0'..'9'] PreLines[13].GoTo[i] = 14
	for i : !['0'..'9'] PreLines[14].GoTo[i] = 14
	PreLines[14].Id = 7
	
	for i : !['A'..'Z','a'..'z'] PreLines[14].GoTo[i] = 15
	for i : !['A'..'Z','a'..'z'] PreLines[15].GoTo[i] = 15
	PreLines[15].Id = 8
	
	begin.Lines = PreLines.ToArray()
	return begin
}

Token := class{
	Id := int
	Buff := char[1024]
}

GenerateToken := !(char^ Buuf, int id) -> Token^
{
	i := 0
	res := new Token
	res.Id = id
	while Buuf[i]
	{
		res.Buff[i] = Buuf[i]
		i += 1
	}
	res.Buff[i] = 0
	return res
}

GetTokensFromFile := !(char^ Name, Machine M, Queue.{Token^} ToFill) -> bool
{
	Cp := sfile
	Cp.open(Name,"r")

	if not Cp.IsOpen() return false

	Buffer := char[1024]

	LineNum := 1

	Reverted := Stack.{char}()
	while Cp.good()
	{
		CurLine := Cp.readline()
		if not Cp.good() return true		


		Reverted.Push(0->{char})
		L := 0
		LikeALazy := true
		while LikeALazy
		{
			if CurLine[L] == 0 LikeALazy = false
			else
			{
				if CurLine[L] == '/' and CurLine[L+1] == '/'
				{
					LikeALazy = false
					L -= 1
				}else L += 1
			}
		}
		while L >= 0 
		{	
			Reverted.Push(CurLine[L])
			L -= 1
		}

		i := 0
		LastGoodPos := 0
		LastGoodId := 0
		MPos := 0
		pos := 0

		while Reverted.NotEmpty()
		{
			Ch := Reverted.Pop()
			Buffer[pos] = Ch
			pos += 1
			MPos = M.Lines[MPos].GoTo[Ch]
			if M.Lines[MPos].Id == -2 
			{
				pos = 0
				LastGoodPos = 0
				LastGoodId = 0
				MPos = 0
			}else if M.Lines[MPos].Id == -1
			{
				if LastGoodId > 0
				{
					RealSize := LastGoodPos + 1 
					k := pos - 1
					while k >= RealSize
					{
						Reverted.Push(Buffer[k])
						k -= 1	
					}
					Buffer[RealSize] = 0
					ToFill.Push(GenerateToken(Buffer,LastGoodId))
					//printf("%s \n",Buffer)
					pos = 0
					LastGoodPos = 0
					LastGoodId = 0
					MPos = 0
				}
			}else{
				if M.Lines[MPos].Id != 0
				{
					LastGoodPos = pos-1
					LastGoodId = M.Lines[MPos].Id
				}
			}
			
	
		}
		ToFill.Push(GenerateToken("",11)) // new line
		free(CurLine)
	}
	Cp.close()
	return true
}


