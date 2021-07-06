WordDetermMachine := class
{
	Table := int[][]
	IsEndNode := int[]
	CharToGo := u8[256]

	"this" := !() -> void
	{
		Table = null
		IsEndNode = null
		CharToGo[^] = 0
	}
	ComputeFileSize := !() -> int{
		return 256 + 4 + 4 + Table->len*(Table[0]->len + 1)*4
	}
	SerializeToMap := !(u8^ data) -> void{
		asInt := data->{int^}
		asInt[0] = Table->len
		asInt[1] = Table[0]->len
		itr := data[8]&
		memcpy(itr,CharToGo[0]&,256)
		itr = itr[256]&
		lineSize := IsEndNode->len*4
		memcpy(itr,IsEndNode->{int^},lineSize)
		itr = itr[lineSize]&
		lineSize = Table[0]->len*4

		for state : Table
		{
			memcpy(itr,state->{int^},lineSize)
			itr = itr[lineSize]&
		}
	}
	LoadFromMap := !(u8^ data, int size) -> void
	{
		asInt := data->{int^}
		states := asInt[0]
		lines := asInt[1]
		itr := data[8]&
		
		memcpy(CharToGo[0]&,itr,256)
		itr = itr[256]&

		lineSize := states*4
		IsEndNode = new int[states]
		memcpy(IsEndNode->{void^},itr,lineSize)
		itr = itr[lineSize]&
		lineSize = lines*4

		Table = new int[][states]
		for tb : Table
		{
			tb = new int[lines]
			memcpy(tb->{void^},itr,lineSize)
			itr = itr[lineSize]&
		}

	}
	"=" := !(WordDetermMachine wut) -> void {}
	PrintIt := !() -> void
	{
		printf("           ")
		for IsEndNode->len printf("%4i ",it)
		printf("\n")
		printf("lines %3i  ",Table[0]->len)
		for IsEndNode printf("%4i ",it)
		printf("\n")
		chStart := 0
		chEnd := 0
		while chStart < 256
		{
			while CharToGo[chEnd] == CharToGo[chStart] {
				chEnd++
				if chEnd == 256 break
			}
			newEnd := chEnd - 1
			if chStart == newEnd
			{
				if IsPrintable(chStart) printf("     %c     ",chStart) else printf("   0x%02X    ",chStart)
			}else{
				if IsPrintable(chStart) printf("   %c",chStart) else printf("0x%02X",chStart)
				printf("..")
				if IsPrintable(newEnd) printf("%c   ",newEnd) else printf("0X%02X",newEnd)
				printf(" ")
			}

			nodLine := CharToGo[chStart]->{int}

			for j : IsEndNode->len
			{
				printf("%4i ",Table[j][nodLine])
			}
			printf("\n")

			chStart = chEnd

		}
	}
	IsMatch := !(char^ toTst) -> bool
	{
		toRet := false
		stSize := strlen(toTst)
		RegExpReadText(this&,toTst,stSize,(a,b,c) ==> {
			if b == 0 and c == stSize
				toRet = true
		})
		return toRet
	}
}

RegExpReadText := !(void^ itCorePre,char^ data,int siz, !(int,int,int)&->void toEmit) -> void
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
