
BW := class
{
	sPoint := u8^
	byteOffset,bitOffset := int
	endSize := int
	this := !(void^ data, int size) -> void 
	{
		sPoint = data->{u8^}
		byteOffset = 0
		bitOffset = 0
		endSize = size
	}
	Move := !(int toMov) -> void 
	{
		bitOffset += toMov
		byteOffset += bitOffset div 8
		bitOffset = bitOffset % 8
	}
	Get := !(int siz) -> int 
	{
		msk := 0
		msk = not_b msk
		msk = msk << siz
		msk = not_b msk
		preRes := 0
		c := byteOffset + 4
		if c >= endSize c = endSize
		while c >= byteOffset
		{
			preRes = preRes << 8
			preRes += sPoint[c]->{u32}->{int}
			c--
		}
		preRes = preRes >> bitOffset
		return preRes and_b msk
	}
	GetAndMove := !(int siz) -> int
	{
		preRes := Get(siz)
		Move(siz)
		return preRes
	}
}



NotHuffmanTree9 := class  {
	treeInfo := Pair.{s16,u8}[512] // character, size
	extraNodes : = Tuple.{u16, u16, u16}[]
}

inverseSomeBit := !(int inp,int size) -> int
{
	preRes := 0
	for size
	{
		preRes = preRes << 1
		preRes += inp and_b 1
		inp = inp >> 1
	}
	return preRes
}

ApplyHuffmanTree := !(char[] valueSizes, NotHuffmanTree9^ toSet) .{} -> void
{
	maxSize := 0
	if maxSize < valueSizes[^]
		maxSize = it
	blCount := new int[maxSize+1] ; $temp
	for i : valueSizes
		blCount[i]++
	blCode := new int[maxSize + 1] ; $temp
	blCount[0] = 0

	code := 0
	for i : 1..maxSize
	{
		code = (code + blCount[i-1]) << 1
		blCode[i] = code
		printf("max %i %X %i\n",maxSize,code,blCount[i-1])
	}
	resCodes := new int[valueSizes->len] ; $temp
	for c,i : valueSizes
	{
		if c != 0
		{
			resCodes[i] = blCode[c]
			blCode[c]++
		}
	}
	S := 9
	for c,i : resCodes
	{
		valSize := valueSizes[i]
		if  valueSizes[i] <= S
		{
			if valSize == 0 continue
			diff := S - valSize
			toPut := c << diff
			for j : 0..((1 << diff) - 1)
			{
				preSet := inverseSomeBit(toPut + j,S)
				ts := ref toSet.treeInfo[preSet]
				ts.first = i
				ts.second = valSize
			}
		}
	}
	//printf("test %i\n",toSet.treeInfo[^].second)
	
	if maxSize > S
	{
		//extraNodesCount := 0
		//for i : S..maxSize
		//{
		//	extraNodesCount += blCount[i]
		//}
		//toSet.extraNodes = new HuffmanExtraNodes[extraNodesCount]
		//indIter := 0
		//for i : S+1..maxSize
		//{
		//	for c,j : valueSizes
		//	{
		//		if c == i
		//		{
		//			appl := ref toSet.extraNodes[indIter]
		//			appl.size = i
		//			appl.value = j
		//			appl.code = resCodes[j]
		//			indIter++
		//		}
		//	}
		//}
	}
}
DecodeSmallHuffmanTable := !(NotHuffmanTree9^ toApply,BW^ bw,u8^ result,int codesCount) -> void
{
	prevCode := 0
	outIter := result->{u8^}
	while codesCount != 0
	{
		itInt := bw.Get(9)
		nowP := ref toApply.treeInfo[itInt]
		bw.Move(nowP.second)
		printf("res %i %i\n",nowP.first,nowP.second)
		
		switch nowP.first
		{
			case 0..15
				prevCode = nowP.first
				outIter^ =nowP.first
				outIter = outIter[1]&
				codesCount--
			case 16
				c1 := bw.GetAndMove(2)
				for c1+2
				{
						outIter^ = prevCode
						outIter = outIter[1]&
				}
				codesCount -= c1+2
			case 17
				c2 := bw.GetAndMove(3)
				for c2+3
				{
					outIter^ = 0
					outIter = outIter[1]&
				}
				codesCount -= c2+3
			case 18
				c3 := bw.GetAndMove(7)
				for c3+11
				{
					outIter^ = 0
					outIter = outIter[1]&
				}
				codesCount -= c3+11
		}
	}
}


codeLenTable := int[19] //![16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15]
iDeflateInflate := !(void^ inpPoint,size_t inpSize,void^ outpPoint,size_t outpSize) -> bool
{
	//TODO: to the global data
	for i,j : ![16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15]
	{
		codeLenTable[j] = i
	}
	bw := BW(inpPoint,inpSize)
	isEnd := bw.GetAndMove(1)
	compType := bw.GetAndMove(2)

	printf("isEnd %i compType %i\n",isEnd,compType)
	//if compType == 2
	HLIT := bw.GetAndMove(5)
	HDIST := bw.GetAndMove(5)
	HCLEN := bw.GetAndMove(4)

	printf("lit %i dist %i clen %i\n",HLIT,HDIST,HCLEN)


	codeLens := new char[19] ; $temp

	for it : HCLEN + 4
	{
		codeLens[codeLenTable[it]] = bw.GetAndMove(3)
	}
	printf("da len %i %i\n",i,codeLens[^i])

	newTree := new NotHuffmanTree9 ; $temp
	ApplyHuffmanTree(codeLens,newTree)

	resultData := new u8[258 + HCLEN + HDIST] ; $temp
	DecodeSmallHuffmanTable(newTree,bw&,resultData,resultData->len - 4)

	for i : resultData->len
	{
		switch i
		{
			case 'A'..'Z'
				printf("code %4i = %i\n",i,resultData[i])
			case 'a'..'z'
				printf("code %4i = %i\n",i,resultData[i])
			case void
				printf("code %4i = %i\n",i,resultData[i])
		}
	}

	return true
}

//lensSize := ![0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5]
//DecodeHuffmanData: = !(NotHuffmanTree.{9}^ toApply, BW^ bw, void^ result) -> void
//{
//	while true
//	{
//		itValue := bw.GetValue(9)
//		inStart := ref toApply.NotHuffmanTree[itValue]
//		resValue := 0
//		if inStart.1 != 0
//		{
//			resValue = inStart.0
//			bw.Move(inStart.1)
//		}
//		else {
//			//TODO: remake
//			//code , size, value
//			for it : toApply.extraNodes
//			{
//				if it.1 == bw.GetValue(it.1) {
//					resValue = it.2
//					bw.Move(it.1)
//					break
//				}
//			}
//		}
//
//		if( resValue == 256)
//			return 
//		if (resValue < 256) {
//			result^ = resValue
//			result++
//		}
//		else {
//			distance := 0
//			lengh := 0
//		}
//	}
//}
//
//fixedHuffmanTree := NotHuffmanTree.{9}
//
//initFixedHuffmanTree := !() -> void
//{
//	miniData := new char[287] ; $temp
//	for i : 0..143 miniData[i] = 9
//	for i : 144..255 miniData[i] = 8
//	for i : 256..279 miniData[i] = 7
//	for i : 280..287 miniData[i] = 8
//	ApplyHuffmanTree(miniData,fixedHuffmanTree^)
//}
//#run initFixedHuffmanTree
//Inflate := !(void^ inpBuffer, int inpSize, void^ outpBuffer, int outpSize) -> bool
//{
//		bw := BW(inptBuffer,inpSize)
//		outIter := outpBuffer
//		
//		dynTreeData := new char[319]//287 + 32
//		subTree := new NotHuffmanTree.{7} ; $temp
//		hufTree := new NotHuffmanTree.{9} ; $temp
//		while true
//		{
//			blckInf := bw.GetInt(1)
//			bw.Move(1)
//			typ := bw.GetInt(2)
//			
//			switch typ
//			{
//				case 0
//					bw.MoveToByte()
//					itSize := bw.Get(16)
//					bw.Move(32)
//					memcpy(outIter,bw.GetPoint(),itSize)
//					bw.MoveByte(itSize)
//				case 1
//					//???
//				case 2
//					wordLen := bw.GetAndMove(5)
//					distLen := bw.GetAndMove(5)
//					subW := bw.GetAndMove(4)
//					
//					shuffledLens := ![16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15]
//					
//					preLen := int[19]
//					preLen[^] = 0
//					
//					for i : subW+4
//					{
//						preLen[shuffledLens[i]] = bw.GetAndMove(3)
//					}
//					ApplyHuffmanTree(preLen,subTree)					
//					DecodeSmallHuffmanTable(subTree,bw&,dynTreeData,wordLen + distLen + 257 + 1)
//					
//					
//			}
//			
//			
//			if blckInf == 1
//				return true
//		}
//		return true
//}
