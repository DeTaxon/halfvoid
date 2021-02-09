jsonTypeField := 0
jsonTypeRecord := 1
jsonTypeArray := 2

jsonNode := class
{
	this := !(int setType) -> void {itType = setType}

	haveName := bool
	itType := u8
	itKey := StringSpan
	itValue := StringSpan
	itSub := List.{jsonNode^}
	
	Key := !() -> ref StringSpan { return itKey }
	Print := !(int x) -> void
	{
		switch itType
		{	
			case jsonTypeField
				for x printf(" ")
				if haveName
				{
					printf("%s : %s\n",itKey,itValue)
				}else{
					printf("%s\n",itValue)
				}
			case jsonTypeRecord
				for x printf(" ")
				if haveName
				{
					printf("%s : {\n",itKey)
				}else{
					printf("{\n")
				}
				itSub[^].Print(x+1)
				for x printf(" ")
				printf("}\n")
			case jsonTypeArray
				for x printf(" ")
				if haveName
				{
					printf("%s : [\n",itKey)
				}else{
					printf("[\n")
				}
				itSub[^].Print(x+1)
				for x printf(" ")
				printf("]\n")
		}
	}
	IsField := !() -> bool { return itType == jsonTypeField}
	IsRecord := !() -> bool { return itType == jsonTypeRecord}
	IsArray := !() -> bool { return itType == jsonTypeArray}

	GetStr := !() -> StringSpan { if itType != jsonTypeField throw new Exception("Node is not field") result = itValue }
	GetInt := !() -> int { if itType != jsonTypeField throw new Exception("Node is not field") return ToInt(itValue)}
	GetFloat := !() -> float { if itType != jsonTypeField throw new Exception("Node is not field") return ToFloat(itValue)}

	"[]" := !(char^ walkPath) -> ref jsonNode
	{
		itms := DivideStr(walkPath,' ') ; $temp
		iter  := this&
		for it : itms
		{
			found := false
			for subs : iter.itSub
			{
				if subs.haveName and subs.itKey == it 
				{
					iter = subs
					found = true
					break
				}
			}
			if not found 
				throw new Exception("Path not found")
		}
		return iter^
	}
	
	"~For" := !() -> ListIter.{jsonNode^} { if itType == jsonTypeField throw new Exception("Node is field") return itSub."~For"() }
}


jsonRegMachine := WordDetermMachine^
jsonLoadMachine := !() -> void
{
	regFile := gRepo.GetFile("hres/json.stm")
	if regFile == null
		throw new Exception("can not initialize json state machine")
	ptr := regFile.Map()
	jsonRegMachine = new WordDetermMachine
	jsonRegMachine.LoadFromMap(ptr,regFile.Size())
	regFile.Unmap()
	while true yield void
}

json := class extend jsonNode
{
	startNode := jsonNode^
	tokens := List.{Tuple.{int,StringSpan}} ; $temp

	ParseString := !(char^ fileData,int size) -> void
	{
		jsonLoadMachine()
		memset(this&,0,json->TypeSize)
		d := WordParser
		tokens."this"()
		d.ReadText(jsonRegMachine,fileData,size, (a,b,c) ==>
		{
			if a == 5
				return void
			ptr := fileData[b]&
			tok := StringSpan(ptr,c)
			tokens << !{a,tok}
		})
		if tokens.Size() == 0 or tokens[0].0 != 4 or tokens[0].1 != "{"
			throw new Exception("Incorrect json")
		tokens.Pop()
		checkRecord(this&)
		itType = jsonTypeRecord
	}

		
	checkRecord := !(jsonNode^ toCheck) -> void
	{
		while tokens.Size() != 0 and not (tokens[0].0 == 4 and tokens[0].1 == "}")
		{
			if tokens[0].0 == 4 and tokens[0].1 == ","
				tokens.Pop()
			toCheck.itSub << checkNode()
		}
		if tokens.Size() == 0
			throw new Exception("Incorrect")
		tokens.Pop()
	}
	checkArray := !(jsonNode^ toCheck) -> void
	{
		while tokens.Size() != 0 and not (tokens[0].0 == 4 and tokens[0].1 == "]")
		{
			if tokens[0].0 == 4 and tokens[0].1 == ","
				tokens.Pop()
			newNode := checkNode2(new jsonNode)
			toCheck.itSub << newNode
		}
		if tokens.Size() == 0
			throw new Exception("Incorrect")
		tokens.Pop()
	}
	checkNode := !() -> jsonNode^
	{
		thisNode := new jsonNode

		if tokens.Size() == 0 or tokens[0].0 != 1
			throw new Exception("Incorrect")
		thisNode.itKey = tokens[0].1[1..-1]
		thisNode.haveName = true
		tokens.Pop()

		if tokens.Size() == 0 or tokens[0].0 != 4 or tokens[0].1 != ":" 
			throw new Exception("Incorrect")
		tokens.Pop()
		if tokens.Size() == 0
			throw new Exception("Incorrect")
		return checkNode2(thisNode)
	}
	checkNode2 := !(jsonNode^ thisNode) -> jsonNode^
	{
		switch tokens[0].0
		{
		case ![1,2,3]
			thisNode.itType = jsonTypeField
			if tokens[0].0 == 1
				thisNode.itValue = tokens[0].1[1..-1]
			else
				thisNode.itValue = tokens[0].1
			
			tokens.Pop()
		case 4
			switch tokens[0].1
			{
			case "["
				thisNode.itType = jsonTypeArray
				tokens.Pop()
				checkArray(thisNode)
			case "{"
				thisNode.itType = jsonTypeRecord
				tokens.Pop()
				checkRecord(thisNode)
			}
		}
		return thisNode
	}
}
