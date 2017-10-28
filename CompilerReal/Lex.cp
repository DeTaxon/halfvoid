
Line := class {
	GoTo := char[256]
	Id := s8
	RevertChars := u8
}

Machine := class{
	Lines := Line^

	"this" := !() -> void
	{
		Lines = null
	}	
}

GenerateMachine := !() -> Machine^
{
	begin := new Machine()
	
	// word-1 Hex-2 int-3 int_s-4 float-5 float_s-6 floate-7 floate_s-8 str 9

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
	//22: opers - 10
	//23: operstwo - 10

	Size := 24

	begin.Lines = new Line[Size]

	// clean
	for i : Size for j : 256 begin.Lines[i].GoTo[j] = 1
	for j : 256 
	{
		begin.Lines[0].GoTo[j] = 1
		begin.Lines[j].RevertChars = 0
	}
	begin.Lines[1].Id = -1
	begin.Lines[1].RevertChars = 1
	begin.Lines[16].Id = -1
	begin.Lines[16].RevertChars = 0
	begin.Lines[21].Id = -1
	begin.Lines[21].RevertChars = 2
	
	for i : 256 begin.Lines[0].GoTo[i] = 16	

	//symbols
	for i : "{}[]()@$^&;?." begin.Lines[0].GoTo[i] = 20
	begin.Lines[20].GoTo['.'] = 20
	begin.Lines[20].Id = 10
	
	//opers
	for i : "<>!:+-*/%=" begin.Lines[0].GoTo[i] = 22
	begin.Lines[22].Id = 10
	begin.Lines[22].GoTo['='] = 23
	begin.Lines[23].Id = 10
	

	// string
	begin.Lines[0].GoTo['"'] = 17
	for i : 256 begin.Lines[17].GoTo[i] = 17
	begin.Lines[17].GoTo['"'] = 19
	begin.Lines[17].GoTo['\\'] = 18
	for i : 256 begin.Lines[18].GoTo[i] = 17
	begin.Lines[19].Id = 9
	

	// word id 1
	for i : !['a'..'z','A'..'Z','_','$','@','#'] begin.Lines[0].GoTo[i] = 2
	for i : !['a'..'z','A'..'Z','_'] 	begin.Lines[2].GoTo[i] = 2
	begin.Lines[2].Id = 1

	//number
	for i : !['0'..'9'] begin.Lines[0].GoTo[i] = 3
	
	begin.Lines[3].GoTo['x'] = 4
	begin.Lines[3].Id = 3
	
	for i : !['a'..'f','A'..'F','0'..'9','_'] begin.Lines[4].GoTo[i] = 5
	begin.Lines[4].Id = 4
	for i : !['a'..'f','A'..'F','0'..'9','_'] begin.Lines[5].GoTo[i] = 5
	begin.Lines[5].Id = 2
	
	for i : !['0'..'9'] begin.Lines[3].GoTo[i] = 6

	for i : !['0'..'9'] begin.Lines[6].GoTo[i] = 6
	begin.Lines[6].Id = 3

	for i : !['a'..'z','A'..'Z'] begin.Lines[6].GoTo[i] = 7
	for i : !['a'..'z','A'..'Z'] begin.Lines[7].GoTo[i] = 7
	begin.Lines[7].Id = 4

	begin.Lines[6].GoTo['.'] = 8
	begin.Lines[3].GoTo['.'] = 8
	
	begin.Lines[8].GoTo['.'] = 21
	begin.Lines[9].Id = 3

	for i : '0'..'9' begin.Lines[8].GoTo[i] = 10
	for i : '0'..'9' begin.Lines[10].GoTo[i] = 10
	begin.Lines[10].Id = 5
	
	for i : !['a'..'z','A'..'Z'] begin.Lines[10].GoTo[i] = 11
	for i : !['a'..'z','A'..'Z'] begin.Lines[11].GoTo[i] = 11
	begin.Lines[11].Id = 6

	for i : !['e'] begin.Lines[10].GoTo[i] = 12
	for i : !['-','+'] begin.Lines[12].GoTo[i] = 13
	for i : !['0'..'9'] begin.Lines[12].GoTo[i] = 14
	for i : !['0'..'9'] begin.Lines[13].GoTo[i] = 14
	for i : !['0'..'9'] begin.Lines[14].GoTo[i] = 14
	begin.Lines[14].Id = 7
	
	for i : !['A'..'Z','a'..'z'] begin.Lines[14].GoTo[i] = 15
	for i : !['A'..'Z','a'..'z'] begin.Lines[15].GoTo[i] = 15
	begin.Lines[15].Id = 8
	
	return begin
}

Token := class{
	Id := int
	Buff := char[1024]
	AsDouble := double at Buff
	AsInt := s32 at Buff 
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

GetTokensFromFile := !(char^ Name, Queue.{Token^} ToFill) -> bool
{
	Cp := sfile
	Cp.open(Name,"r")

	M := GenerateMachine()	
	Buffer := char[1024]

	Reverted := Stack.{char}()
	while Cp.good()
	{
		CurLine := Cp.readline()
		if not Cp.good() return true		

		Reverted.Push(0->{char})
		L := CurLine.Size()
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
			//printf("M %i C %c\n",MPos,Ch)
			if M.Lines[MPos].Id == -1 and M.Lines[MPos].RevertChars == 0
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
					Tok := GenerateToken(Buffer,LastGoodId)
					ToFill.Push(Tok)
					//printf("%s %i\n",Tok.Buff,Tok.Id)
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
		free(CurLine)
	}
	Cp.close()
	return true
}


