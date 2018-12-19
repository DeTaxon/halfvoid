TreeNode := class {
	NodeName := string
	Childs := Queue.{Pair.{bool,void^}} // bool ? TreeNode : string
	Attrs := AVLMap.{string,string}
}

FillXMLNode := !(char^ str, TreeNode toFil) -> int{
	buffer := new char[4096] ; $temp
	i := 0
	
	inf := true
	while inf {
		while str[i] != 0 and str[i] != '<' i += 1
		if str[i] == 0 return -1
		
		i += 1
		if str[i] == '?'{
			while str[i] != 0 and str[i] != '>' i += 1
			if str[i] == 0 return -1
			i += 1
		}else if str[i] == '!'{
			i += 3
			stt := 0
			inf3 := true
			while str[i] != 0 and inf3{
				switch stt
				{
					case 0 if str[i] == '-' stt = 1
					case 1 if str[i] == '-' stt = 2 else stt = 0
					case 2 if str[i] == '>' { inf3 = false } else {
						if str[i] != '-' stt = 0
					}
				}
				i += 1
			}
		}else if str[i] == '/'{
			return -1
		}else{
			k := 0
			while str[i] in 'a'..'z'
			 or str[i] in 'A'..'Z'
			 or str[i] in '0'..'9'
			 or str[i] == '_'
			 {
				buffer[k] = str[i]
				i += 1
				k += 1
			}
			buffer[k] = 0
			toFil.NodeName = StrCopy(buffer)
			
			inf2 := true
			while inf2{
				while str[i] in " \t\n" i += 1
				if str[i] == 0 return -1
				
				if str[i] in "/>"{
					inf2 = false
				}else{
					attrName := string
					quote := char
					k = 0
					while str[i] in 'a'..'z'
					or str[i] in 'A'..'Z'
					or str[i] in '0'..'9'
					or str[i] == '_' {
						buffer[k] = str[i]
						i += 1
						k += 1
					}
					if str[i] != '=' return -1
					i += 1
					buffer[k] = 0
					attrName = StrCopy(buffer)
					if not str[i] in "\"'" return -1
					quote = str[i]
					i += 1
					k = 0
					while str[i] != quote{
						buffer[k] = str[i]
						i += 1
						k += 1
						if str[i] == 0 return -1
					}
					buffer[k] = 0
					toFil.Attrs[attrName] = StrCopy(buffer)
					i += 1
				}
			}
			
			if str[i] == '/'{
				if str[i + 1] == '>' return i + 2
			}
			if str[i] != '>' return -1
		
			inf = false
			i += 1
		}
	}
	inf = true
	while inf{
		containGoods := false
		k := 0
		while str[i] != '<' {
			if str[i] == 0 return -1
			buffer[k] = str[i]
			if not str[i] in " \t\n" containGoods = true
			i += 1
			k += 1
		}
		if k != 0 and containGoods {
			buffer[k] = 0
			toFil.Childs.Emplace(false,StrCopy(buffer))
		}
		if str[i] == '<' and str[i+1] == '/'{
			while str[i] != '>' {
				if str[i] == 0 return -1
				i += 1
			}
			return i + 1
		}else if str[i+1] == '!'{
			i += 2
			stt := 0
			inf3 := true
			while str[i] != 0 and inf3{
				switch stt
				{
					case 0 if str[i] == '-' stt = 1
					case 1 if str[i] == '-' stt = 2 else stt = 0
					case 2 if str[i] == '>' { inf3 = false } else {
						if str[i] != '-' stt = 0
					}
				}
				i += 1
			}
		}else{
			chl := new TreeNode
			toFil.Childs.Emplace(true,chl)
			res := FillXMLNode(str[i]&,chl^)
			if res == -1 return -1
			i += res
		}		
	}
	
	return i
}

ParseXML := !(string str) -> TreeNode^ {
	preRes := new TreeNode
	te := FillXMLNode(str,preRes^)
	return preRes
}

//XMLTreeIterator := class {
//	noda := TreeNode^
//	ind := int
//	this := !(TreeNode^ nd)
//}
