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
	inputFile := MappedFile(fileName.itStr)
	DaFile := new BoxFile(fileName)

	d := WordParser
	bufff := char[1024]
	itLine := null->{ObjLine^}
	subIter := null->{Object^}
	d.ReadText(lexWordMachine&->{void^},inputFile.point,inputFile.Size(), (a,b,c) =>
	{
		ptr := inputFile.point->{char^}[b]&
		for i : c {
			bufff[i] = ptr[i]
		}
		bufff[c] = 0
		ns := null->{Object^}
		if a != 21{
			if itLine == null {
				itLine = new ObjLine()
			}
		}
		if a == 1 {
			if bufff[0] in "@#"
			{
				if bufff[0] == '#'
				{
					ns = new ObjCmd(StrCopy(bufff))
				}else{
					ns = new ObjTemplateType(StrCopy(bufff))
				}
			}else{
				if bufff == "null"
				{
					ns = new ObjNULL()
				}else{
					if IsKeyword(bufff)
					{
						ns = new ObjKeyword(StrCopy(bufff))
					}else{
						if bufff == "true" or bufff == "false"
						{
							ns = new ObjBool(bufff == "true")
						}else{
							if bufff == "continue" or bufff == "break"{
								ns = new WayControl(bufff)
							}else{
								ns = new ObjIndent(StrCopy(bufff))
							}
						}
					}
				}
			}
		}
		if a == 3 {
			DaBuff := char[2048]
			j := 0
			k := 1
			while bufff[k] != '"'
			{
				if bufff[k] == '\\' {
					k += 1
					if bufff[k] == 'n' bufff[j] = 10
					else 
					{
						DaBuff[j]  = bufff[k]
					}
					j += 1
					k += 1
				}else{
					DaBuff[j] = bufff[k]
					k += 1
					j += 1
				}	
			}
			DaBuff[j] = 0
			ns = new ObjStr(DaBuff.Copy())
		}
		if a == 4 {
			newVal := int
			if bufff[1] == '\\'
			{
				if bufff[2] == 'n'
				{	
					newVal = 0x0A
				}else
				{
					newVal = bufff[2]
				}
			}else{
				newVal = bufff[1]
			}
			ns = new ObjInt(newVal)
		}
		if a == 5{
			Value := int
			Value = 0
			
			k := 0
		
			while bufff[k] in '0'..'9'
			{
				Value *= 10
				Value += bufff[k] - '0'
				k += 1
			}
			ns = new ObjInt(Value)
		}
		if a == 6{
			Value := 0.0
			k := 0
			Quant := 0.1
			SSing := 1.0

			while bufff[k] != '.'
			{
				Value *= 10
				Value += bufff[k] - '0' 
				k += 1 
			}
			k += 1 //.
			while bufff[k] in '0'..'9'
			{
				Value += (bufff[k] - '0')*Quant
				k += 1
				Quant *= 0.1
			}
			if bufff[k] == 'e' 
			{
				// not now
			}
			ns = new ObjDouble(Value)
		}
		if a == 7 {
			Value := int
			Value = 0
			
			k := 2
		
			while bufff[k] != 0
			{
				Value *= 16
				if bufff[k] in '0'..'9' Value += bufff[k] - '0'
				if bufff[k] in 'A'..'F' Value += bufff[k] - 'A' + 10
				if bufff[k] in 'a'..'f' Value += bufff[k] - 'a' + 10
				k += 1
			}
			ns = new ObjInt(Value)
		}
		if a == 8 {
			ns = new ObjSymbol(bufff.Copy()) 
		}
		if a == 9{
			//comments
		}
		if a == 20 {
			if subIter == null {
				if DaFile.Down != null{
					while subIter.Right != null{
						subIter = subIter.Right
						subIter.Line = itLine
					}
				}
			}else{
				if subIter.Right != null{
					while subIter.Right != null{
						subIter = subIter.Right
						subIter.Line = itLine
					}
				}
			}
			//new line
			itLine = null
		}
		if a == 21 {
			itLine.LoadAttrs(bufff)
			// attrs
		}
	})
	inputFile.Close()
	if subIter != null and subIter.Right != null{
		while subIter.Right != null{
			subIter = subIter.Right
			subIter.Line = itLine
		}
	}
	UniteSkobs(DaFile)

	return DaFile
}

	//itBuilder.ApplyReg("[a-zA-Z_][a-zA-Z0-9_]*",1)
	//itBuilder.ApplyReg("[$#][a-zA-Z0-9_]*",2)
	//itBuilder.ApplyReg("\"([^\"] | (\\\\ \"))*\"",3)
	//itBuilder.ApplyReg("'( (\\\\ [^]) |  [^'\\\\])'",4)
	//itBuilder.ApplyReg("[0-9]+",5)
	//itBuilder.ApplyReg("[0-9]+.[0-9]+(e[+-]?[0-9]+)?",6)
	//itBuilder.ApplyReg("0x[0-9a-fA-F]+",7)

	//for pr.Opers itBuilder.ApplyReg(it,8)
	//for keywords itBuilder.ApplyReg(it,8)

	//itBuilder.ApplyReg("/ /[^\\n]*",9)
	//itBuilder.ApplyReg("\\{ | \\} | \\( | \\) | \\[ | \\]",10)
	//itBuilder.ApplyReg("[?,|]",10)
	//itBuilder.ApplyReg("\\n",20)
	//itBuilder.ApplyReg("; [^\\n]*",21)
