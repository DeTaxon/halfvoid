WordParser := class 
{
	//itCore := WordDetermMachine^

	ReadText := !(void^ itCorePre,char^ data,int siz, !(int,int,int)&->void toEmit) -> void
	{
		itCore := itCorePre->{WordDetermMachine^}
		i := 0
		while i < siz
		{
			nowState := 0
			nowPos := 0
			nowSize := 0
			lastGoodTerm := -1
			lastGoodSize := 0

			while nowPos != -1
			{
				if nowSize + i - 1 >= siz break

				if itCore.IsEndNode[nowPos] != -1
				{
					lastGoodTerm = itCore.IsEndNode[nowPos]
					lastGoodSize = nowSize
				}

				nowChar := data[i + nowSize]
				inTable := itCore.CharToGo[nowChar]
				nowPos = itCore.Table[nowPos][inTable]
				nowSize++
			}
			if lastGoodTerm != -1
			{
				toEmit(lastGoodTerm,i,lastGoodSize)
				i += lastGoodSize
			}else{
				i++
			}
		}
	}
}
