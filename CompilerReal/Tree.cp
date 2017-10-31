
Object_NewLine :: 1
Object_Int :: 2
Object_Double :: 3
Object_Char :: 4
Object_Null :: 5
Object_Ident :: 6
Object_String :: 7
Object_Suffix :: 8
Object_MathOpers :: 9
Object_CmpOpers :: 10
Object_PrisOpers :: 11
Object_File :: 12
Object_LineInfo :: 13
Object_KeyWord :: 14
Object_Type :: 15
Object_DoubleDot :: 16
Object_TripleDot :: 17
Object_Cmd :: 18
//Object_Symbol :: represented by theyr own code !(){}[]^:\|/


Object_Array :: 256

Object := class{
	Id := int
	Left,Right,Down,Up := Object^
	Line :=  Object^
	Bag := void^
	
	"this" := !() -> void
	{
		Id = 0
		Left = null
		Right = null
		Down = null
		Up = null
		Line = null
	}
	"this" := !(int id) -> void
	{
		Id = id
		Left = null
		Right = null
		Down = null
		Up = null
		Line = null
	}
	IsConst := !() -> bool
	{
		if Id == Object_Int 	return true
		if Id == Object_Double 	return true
		if Id == Object_Char 	return true
		if Id == Object_String 	return true
		
		// if Id == Object_Array ???

		return false
	}
	
	Print := !() -> void
	{
		if Id == Object_Int{
			printf("int %i",Bag->{int^}^)
		}
		if Id == Object_Double{
			printf("double %f",Bag->{double^}^)
		}
		if Id == Object_Suffix{
			printf("Suffix %s",Bag->{char^})
		}
		if Id == Object_String{
			printf("string %s",Bag->{char^})
		}	
		if Id == Object_Ident{
			printf("indent %s",Bag->{char^})
		}
		if Id in "!{}[]()*.\\|/^&:;"{
			printf("symbols %c",Bag->{char^})
		}
	}
}

PushObject := !(Object^ Ad,Object^ ToAdd) -> Object^
{
	Ad.Right = ToAdd
	ToAdd.Left = Ad
	return ToAdd
}

TokensToObjects := !(char^ filename, Queue.{Token^} Toks) -> Object^
{
	DaFile := new Object()
	iter := DaFile
	Adder := DaFile
	
	while Toks.NotEmpty()
	{
	// word-1 Hex-2 int-3 int_s-4 float-5 float_s-6 floate-7 floate_s-8 str 9 #-10 \n-11
		Tok := Toks.Pop()
		if Tok.Id == 11
		{
			NL := new Object(Object_NewLine)
			while iter.Right != null
			{
				iter = iter.Right
				iter.Line = NL
			}
		}
		if Tok.Id == 10
		{
			NS := new Object(Tok.Id)
			Adder = PushObject(Adder,NS)
		}
		if Tok.Id == 9
		{
			DaBuff := char[2048]
			j := 0
			k := 1
			while Tok.Buff[k] != '"'
			{
				if Tok.Buff[k] == '\\' {
					k += 1
					if Tok.Buff[k] == 'n' DaBuff[j] = 20
					j += 1
					k += 1
				}else{
					DaBuff[j] = Tok.Buff[k]
					k += 1
					j += 1
				}	
			}
			DaBuff[j] = 0
			NS := new Object(Object_String)
			NS.Bag = DaBuff.Copy()
			Adder = PushObject(Adder,NS)
		}
		if Tok.Id == 1
		{ 
			if Tok.Buff[0] in "$@"
			{
				if Tok.Buff[0] == '$'
				{
					NS := new Object(Object_Cmd)
					NS.Bag = Tok.Buff.Copy()
					Adder = PushObject(Adder,NS)
				}else{
					NS := new Object(Object_Cmd)
					NS.Bag = Tok.Buff.Copy()
					Adder = PushObject(Adder,NS)
				}
			}else{
				NS := new Object(Object_Ident)
				NS.Bag = Tok.Buff.Copy()
				Adder = PushObject(Adder,NS)
			}
		}
		if Tok.Id == 2
		{
			Value := int
			NV := new Object(Object_Int)
			
			Value = 0
			
			k := 2
		
			while Tok.Buff[k]
			{
				Value *= 16
				if Tok.Buff[k] in '0'..'9' Value += Tok.Buff[k] - '0'
				if Tok.Buff[k] in 'A'..'F' Value += Tok.Buff[k] - 'A' + 10
				if Tok.Buff[k] in 'a'..'f' Value += Tok.Buff[k] - 'a' + 10
				k += 1
			}
			PValue := new int
			PValue[0] = Value
			NV.Bag = PValue
			Adder = PushObject(Adder,NV)
		}
		if Tok.Id == 3 or Tok.Id == 4
		{
			Value := int
			NV := new Object(Object_Int)
			
			Value = 0
			
			k := 0
		
			while Tok.Buff[k] in '0'..'9'
			{
				Value *= 10
				Value += Tok.Buff[k] - '0'
				k += 1
			}
			PValue := new int
			PValue[0] = Value
			NV.Bag = PValue
			Adder = PushObject(Adder,NV)

			if Tok.Id == 4
			{
				NS := new Object(Object_Suffix)
				NS.Bag = Tok.Buff[k]&.Copy()
				Adder = PushObject(Adder,NS)
			}
		}
		if Tok.Id in 5..8
		{
			Value := 0.0
			k := 0
			Quant := 0.1
			SSing := 1.0

			while Tok.Buff[k] != '.'
			{
				Value *= 10
				Value += Tok.Buff[k] - '0' 
				k += 1 
			}
			k += 1 //.
			while Tok.Buff[k] in '0'..'9'
			{
				Value += Tok.Buff[k]*Quant
				k += 1
				Quant *= 0.1
			}
			if Tok.Buff[k] == 'e' 
			{
				// not now
			}
			NV := new Object(Object_Double)
			PValue := new double
			PValue[0] = Value
			NV.Bag = PValue
			Adder = PushObject(Adder,NV)

			if Tok.Buff[k] != 0
			{
				NS := new Object(Object_Suffix)
				NS.Bag = Tok.Buff[k]&.Copy()
				Adder = PushObject(Adder,NS)
			}
			
		}
		free(Tok)
	}
	DaFile.Down = DaFile.Right
	iter = DaFile.Down
	iter.Left = null

	while iter != null
	{
		iter.Up = DaFile
		iter = iter.Right
	}
	return DaFile
}

