
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
		while c >= byteOffset
		{
			preRes = preRes << 8
			preRes += sPoint[c]->{u32}->{int}
			c--
		}
		preRes = preRes >> bitOffset
		return preRes and_b msk
	}
	MoveUp := !() -> void
	{
		if bitOffset != 0
		{
			bitOffset = 0
			byteOffset += 1
		}
	}
	CopyTo := !(u8^  toSet,int blkSize) -> void
	{
		memcpy(toSet,sPoint[byteOffset]&,blkSize)
		byteOffset += blkSize
	}
	GetAndMove := !(int siz) -> int
	{
		preRes := Get(siz)
		Move(siz)
		return preRes
	}
}



NotHuffmanTree9 := class  {
	treeInfo := Tuple.{s16,u8}[512] // character, size
	extraNodes := Tuple.{u16, u16}[][16] //charact,value [bitsize]

	GetAndMove := !(BW bw) -> int
	{
		itVal := bw.Get(9)
		inFast := ref treeInfo[itVal]

		if inFast.0 != -1 {
			bw.Move(inFast.1)
			return inFast.0
		}
		for i : 10..15
		{
			if extraNodes[i] == null 
				continue
			itVal = bw.Get(i)
			if extraNodes[i][^].0 == itVal
			{
				bw.Move(i)
				return it.1
			}
		}
		return -1
	}
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

ApplyHuffmanTree := !(char^ valueSizes,int arrLen, NotHuffmanTree9^ toSet) -> void
{
	maxSize := 0
	for c : arrLen
		if maxSize < valueSizes[c]
			maxSize = valueSizes[c]
	blCount := new int[maxSize+1] ; $temp

	for i : arrLen
		blCount[valueSizes[i]]++
	blCode := new int[maxSize + 1] ; $temp
	blCount[0] = 0
	toSet.treeInfo[^].0 = -1


	code := 0
	for i : 1..maxSize
	{
		code = (code + blCount[i-1]) << 1
		blCode[i] = code
	}
	resCodes := new int[arrLen] ; $temp
	for i : arrLen
	{
		c := valueSizes[i]
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
			toPut := inverseSomeBit(c,valSize)
			for j : 0..((1 << diff) - 1)
			{
				preSet := toPut + (j << valSize)
				ts := ref toSet.treeInfo[preSet]
				ts.0 = i
				ts.1 = valSize
			}
		}
	}
	
	if maxSize > S
	{
		memItr := int[16]
		memItr[^] = 0

		for i : (S+1)..maxSize
		{
			if blCount[i] != 0
				toSet.extraNodes[i] = new Tuple.{u16,u16}[blCount[i]] ; $temp
		}
		for i : arrLen
		{
			it := valueSizes[i]
			if it <= S continue
			apl := ref toSet.extraNodes[it][memItr[it]]
			apl.0 = inverseSomeBit(resCodes[i],it)
			apl.1 = i
			memItr[it]++
		}
	}
}
DecodeSmallHuffmanTable := !(NotHuffmanTree9^ toApply,BW bw,u8^ result,int codesCount) -> void
{
	prevCode := 0
	outIter := result->{u8^}
	while codesCount != 0
	{
		itVal := toApply.GetAndMove(bw)	
		switch itVal
		{
			case 0..15
				prevCode = itVal
				outIter^ =itVal
				outIter = outIter[1]&
				codesCount--
			case 16
				c1 := bw.GetAndMove(2)
				for c1+3
				{
						outIter^ = prevCode
						outIter = outIter[1]&
				}
				codesCount -= c1+3
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


fixedHuffmanLiteral := NotHuffmanTree9
fixedHuffmanDistance := NotHuffmanTree9

isInitedFixed := bool
InitFixedHuffman := !() -> void
{
	litLens := char[2897] 
	for 0..143 litLens[it] = 8
	for 144..255 litLens[it] = 9
	for 256..279 litLens[it] = 7
	for 280..287 litLens[it] = 8

	ApplyHuffmanTree(litLens,287,fixedHuffmanLiteral&)

	dists := char[32]
	dists[^] = 5
	ApplyHuffmanTree(dists,32,fixedHuffmanDistance&)

	isInitedFixed = true
}



codeLenTable := ![16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15]
codeLenExtraBits := ![0,0,0,0,0,0,0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  5,  5,  5,  5,  0]
codeLenExtraBias := ![3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258] 
codeDistBits := ![0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13]
codeDistBias := ![1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577]
CrappyDeflateInflate := !(void^ inpPoint,size_t inpSize,u8^ outpPoint,size_t outpSize) -> bool
{
	bw := BW(inpPoint,inpSize)

	while true
	{
		isEnd := bw.GetAndMove(1)
		compType := bw.GetAndMove(2)


		if compType == 0
		{
		  	bw.MoveUp()
			toCpy := bw.Get(2)
			bw.Move(4)
			bw.CopyTo(outpPoint,toCpy)
			outpPoint = outpPoint[toCpy]&
			assert(false)
		}else{
			litsTree := NotHuffmanTree9^
			distTree := NotHuffmanTree9^

			if compType == 1
			{
				if not isInitedFixed
					InitFixedHuffman()
				litsTree = fixedHuffmanLiteral&
				distTree = fixedHuffmanDistance&
			}else{
				HLIT := bw.GetAndMove(5) + 257
				HDIST := bw.GetAndMove(5) + 1
				HCLEN := bw.GetAndMove(4) + 4

				codeLens := new char[19] ; $temp

				for it : HCLEN
				{
					codeLens[codeLenTable[it]] = bw.GetAndMove(3)
				}

				newTree := new NotHuffmanTree9 ; $temp
				ApplyHuffmanTree(codeLens,codeLens->len,newTree)

				resultData := new u8[HLIT + HDIST] ; $temp
				DecodeSmallHuffmanTable(newTree,bw,resultData,resultData->len)

				litsTree = new NotHuffmanTree9 ; $temp
				distTree = new NotHuffmanTree9 ; $temp
				
				ApplyHuffmanTree(resultData->{char^},HLIT,litsTree)
				ApplyHuffmanTree(resultData[HLIT]&->{char^},HDIST,distTree)
			}

			while true
			{
				itVal := litsTree.GetAndMove(bw)

				if itVal == -1 return false

				if itVal == 256
					break
				if itVal >= 256 
				{
					toAdd := bw.GetAndMove(codeLenExtraBits[itVal-257])
					toAdd += codeLenExtraBias[itVal-257]
					ct := distTree.GetAndMove(bw)

					dist := codeDistBias[ct]
					dist += bw.GetAndMove(codeDistBits[ct])
					
					cpyIter := outpPoint[-dist]&
					for toAdd 
					{
						outpPoint^ = cpyIter^
						outpPoint = outpPoint[1]&
						cpyIter = cpyIter[1]&
					}
					//return true
				}else{
					outpPoint^ = itVal
					outpPoint = outpPoint[1]&
				}
			}
		}
		if isEnd
			return true
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
