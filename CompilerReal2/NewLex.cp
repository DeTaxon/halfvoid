#import "Lex.cp"
#import "BoxBlock.cp"
#import "../Libs/MappedFile.cp"

lexWordMachine := WordDetermMachine 

LoadLexMachine := !() -> void
{
	machFile := MappedFile("Mach.m")
	lexWordMachine.LoadFromMap(machFile.point,machFile.Size())
	machFile.Close()
}


GetObjectsFromFile2 := !(Path fileName) -> Object^
{
	DaFile := new BoxFile(fileName)
	inputFile := MappedFile(fileName.itStr)

	d := WordParser
	itLine := null->{ObjLine^}
	treeIter := null->{Object^}
	prevId := -1
	linePos := 0
	d.ReadText(lexWordMachine&->{void^},inputFile.point,inputFile.Size(), (a,b,c) =>
	{
		ptr := inputFile.point->{char^}[b]&

		tok := StringSpan(ptr,c)

		ns := null->{Object^}
		if a != 20{
			if itLine == null {
				itLine = new ObjLine()
			}
		}
		if a == 1 {
			switch tok
			{
				case "null"
					ns = new ObjNULL()
				case "true"
					ns = new ObjBool(true)
				case "false"
					ns = new ObjBool(false)
				case "continue"
					ns = new WayControl(tok.Str())
				case "break"
					ns = new WayControl(tok.Str())
				case void
					if IsKeyword(tok)
					{
						ns = new ObjKeyword(tok.Str())
					}else{
						if prevId == 3 or prevId == 5 or prevId ==  6
						{
							ns = new ObjSuffix(tok.Str())
						}else{
							ns = new ObjIndent(tok.Str())
						}
					}
			}
		}
		if a == 2{
			if tok[0] == '#'
			{
				ns = new ObjCmd(tok.Str())
			}else{
				ns = new ObjTemplateType(tok[1..0].Str())
			}
		}
		if a == 3 {
			DaBuff := char[2048]
			j := 0
			k := 1
			while tok[k] != '"'
			{
				if tok[k] == '\\'{
					k += 1
					switch tok[k]
					{
						case 't' DaBuff[j] = 9
						case 'n' DaBuff[j] = 10
						case void DaBuff[j] = tok[k]
					}
					j += 1
					k += 1
				}else{
					DaBuff[j] = tok[k]
					k += 1
					j += 1
				}	
			}
			DaBuff[j] = 0
			ns = new ObjStr(DaBuff.Copy())
		}
		if a == 4 {
			newVal := int
			if tok[1] == '\\'
			{
				switch tok[2]
				{
					case 't' newVal = 9
					case 'n' newVal = 10
					case void newVal = tok[2]
				}
			}else{
				newVal = tok[1]
			}
			ns = new ObjInt(newVal)
		}
		if a == 5{
			Value := int
			Value = 0
			
			k := 0
		
			while tok[k] in '0'..'9'
			{
				Value *= 10
				Value += tok[k] - '0'
				k += 1
			}
			ns = new ObjInt(Value)
		}
		if a == 6{
			Value := 0.0
			k := 0
			Quant := 0.1
			SSing := 1.0

			while tok[k] != '.'
			{
				Value *= 10
				Value += tok[k] - '0' 
				k += 1 
			}
			k += 1 //.
			while tok[k] in '0'..'9'
			{
				Value += (tok[k] - '0')*Quant
				k += 1
				Quant *= 0.1
			}
			if tok[k] == 'e' 
			{
				// not now
			}
			ns = new ObjDouble(Value)
		}
		if a == 7 {
			Value := int
			Value = 0
			
			k := 2
		
			while k != tok.Size()
			{
				Value *= 16
				if tok[k] in '0'..'9' Value += tok[k] - '0'
				if tok[k] in 'A'..'F' Value += tok[k] - 'A' + 10
				if tok[k] in 'a'..'f' Value += tok[k] - 'a' + 10
				k += 1
			}
			ns = new ObjInt(Value)
		}
		if a == 8 {
			ns = new ObjSymbol(tok.Str()) 
		}
		if a == 9{
			//comments
		}
		if ns != null {
			if treeIter == null{
				DaFile.Down = ns
				treeIter = ns
			}else{
				treeIter.Right = ns
				ns.Left = treeIter
				treeIter = ns
			}
			ns.Line = itLine
			ns.Up = DaFile
		}
		if a == 20 {
			linePos += 1
			if itLine != null{
				itLine.LinePos = linePos
				itLine.inFile = fileName
			}else{
			}
			//new line
			itLine = null
		}
		if a == 21 {
			if itLine != null itLine.LoadAttrs(tok.Str())
			// attrs
		}
		prevId = a
	})
	inputFile.Close()
	UniteSkobs(DaFile.Down)
	return DaFile
}

