
zipFileHeader := packed_class
{
	signature := u32
	versNeededToExtract := u16
	flags := u16
	compressionMethod := u16
	lastModifTime := u16
	lastModifDate := u16
	crc32 := u32
	compressedSize := u32
	realSize := u32
	fileNameLen := u16
	extraFieldsLen := u16
	//fileName
	//extraFields
}
zipCentralDirectory := packed_class
{
	signature := u32
	madeWithVersion := u16
	versionNeedToExtract := u16
	flags := u16
	compressionMethod := u16
	lastModifTime := u16
	lastModifDate := u16
	crc32 := u32
	compressedSize := u32
	realSize := u32
	fileNameLen := u16
	extraFieldsLen := u16
	commentLen := u16
	fileStartsAtDisk := u16
	internalFileAttrs := u16
	externalFileAttrs := u32
	offsetToFileHeader := u32
	// file name
	// extra fields
	// comments
}
zipEndOfDirectory := packed_class
{
	signature := u32
	diskNumber := u16
	centrDirStartDisk := u16
	numberOfCentralDirectoryHere := u16
	totalNumberOfCentralDirectory := u16
	sizeOfCentralDirectoryBytes := u32
	offsetToStartOfCD := u32
	commentLength := u16
	//comments[len]
}
z_stream := class
{
	next_in := void^
	avail_in := u32
	total_in := c_long

	next_out := void^
	avail_out := u32
	total_out := c_long

	msg := char^
	state := void^

	zalloc := void^
	zfree := void^
	opaque := void^

	data_type := int

	adler := c_long
	reserved := c_long
}


vZipEntry := class
{
	ptrToObj := ZipFile^
	offset := int
	realSize := u32
	zipSize := u32
	objName := StringSpan
	fullName := StringSpan
	comprType := int
	subFolders := List.{vZipEntry^}
	compressedPointer := void^

	"this" := !() -> void {}

	Size := !() -> size_t
	{
		return realSize
	}
	Name := !() -> StringSpan
	{
		return objName
	}
	Path := !() -> StringSpan
	{
		return fullName
	}

	Map := !() -> void^
	{
		ptrToObj.AddUser()
		if comprType == 8
		{
			compressedPointer = malloc(realSize)
			resPtr := ptrToObj.asMapped.point[offset]&

			CrappyDeflateInflate(resPtr,zipSize,compressedPointer->{u8^},realSize)
			return compressedPointer
		}
		resPtr := ptrToObj.asMapped.point[offset]&

		return resPtr
	}
	Unmap := !() -> void
	{
		if comprType == 8
		{
			free(compressedPointer)
		}
		ptrToObj.DecUser()
	}

	Print := !(int de) -> void
	{
		for i : de printf("-")
		printf("%s %i %i %i\n",objName.Str(),realSize,zipSize,comprType)
		subFolders[^].Print(de+1)
	}
}
zipIterator := class 
{
	miniStack := List.{vZipEntry^}

	this := !(vZipEntry^ nd) .{} -> void
	{
		miniStack."this"()
		if nd != null 
			miniStack.Push(nd) ; $temp
	}
	IsEnd := !() -> bool { return miniStack.IsEmpty() }
	"^" := !() -> ref vZipEntry^ { return miniStack.Front() }
	Inc := !() .{} -> void {
		nnd := miniStack.Front()
		miniStack.Pop()
		for nnd.subFolders
		{
			miniStack.Push(it)
		}
	}
}

ZipFile := class
{
	filesInUse := int
	examined := bool
	asMapped := MappedFile
	fileName := char^

	zipRoot := vZipEntry

	"this" := !() -> void
	{
		zipRoot.objName = StringSpan("/")
		zipRoot.ptrToObj = this&
	}
	AddUser := !() -> void
	{
		filesInUse++
		if filesInUse == 1
			asMapped."this"(fileName)
	}
	DecUser := !() -> void
	{
		filesInUse--
		if filesInUse == 0
			asMapped.Close()
	}
	"~For" := !() -> zipIterator
	{
		return zipIterator(zipRoot&)
	}
	GetFile := !(char^ fileNameI) -> vZipEntry^
	{
		items := DivideStr(fileNameI,"\\/") ; $temp
		if items.Size() == 0
			return null

		itr := zipRoot&
		for it : items
		{
			found := false
			for subs : itr.subFolders
			{
				if subs.objName == it
				{
					found = true
					itr = subs
					break
				}
			}
			if not found
				return null
		}
		return itr
	}
		

	AnalizeFile := !(char^ fileToLoad) -> bool
	{
		fileName = StrCopy(fileToLoad)
		AddUser()

		ptrToFl := asMapped.point
		eod := ptrToFl[asMapped.Size() - zipEndOfDirectory->TypeSize]&->{zipEndOfDirectory^}

		if eod.signature != 0x06054b50
		{
			return false
		}

		tableCount := eod.numberOfCentralDirectoryHere
		cdTable := ptrToFl[eod.offsetToStartOfCD]&->{zipCentralDirectory^}
	

		for i : tableCount
		{	
			ptrToStr := cdTable[1]&->{char^}
			newStr := StringSpan(ptrToStr,cdTable.fileNameLen)
			if newStr[-1..0] == "/"
			{
				cdTable = cdTable->{u8^}[zipCentralDirectory->TypeSize 
				+ cdTable.fileNameLen 
				+ cdTable.extraFieldsLen
				+ cdTable.commentLen
				]&->{zipCentralDirectory^}
				continue
			}
			fldrs := newStr.DivideStr("/") ; $temp
			itmIter := zipRoot&

			for itm,j : fldrs
			{	
				found := false
				if itmIter.subFolders[^].objName == itm
				{
					itmIter = it
					found = true
					break
				}
				if not found 
				{
					itmIter.subFolders.Push(new vZipEntry)
					nI := itmIter.subFolders.Back()
					nI.ptrToObj = this&
					nI.objName = StringSpan(itm.Str())
					nI.fullName = newStr

					if  j == fldrs.Size() - 1
					{
						nI.realSize = cdTable.realSize
						nI.zipSize = cdTable.compressedSize
						nI.comprType = cdTable.compressionMethod

						ptTH := ptrToFl[cdTable.offsetToFileHeader]&->{zipFileHeader^}

						nI.offset = cdTable.offsetToFileHeader + zipFileHeader->TypeSize + ptTH.fileNameLen + ptTH.extraFieldsLen
					}else{
						itmIter = nI
					}
				}
			}

			cdTable = cdTable->{u8^}[zipCentralDirectory->TypeSize 
			+ cdTable.fileNameLen 
			+ cdTable.extraFieldsLen
			+ cdTable.commentLen
			]&->{zipCentralDirectory^}
		}
		return true
	}
}
zipIntEntry := class
{
	offsetToData := u64
	dataSize := u64
	offsetToFileHeader := u64
	fhSize := int
	offsetToCDEntry := u64
	cdEntrySize := int
	fileName := StringSpan
	
	this := !() -> void {}
}
ZipParse := !(u8^ ptrToFl,u64 fileSize,List.{zipIntEntry} result)-> bool
{
	eod := ptrToFl[fileSize - zipEndOfDirectory->TypeSize]&->{zipEndOfDirectory^}

	if eod.signature != 0x06054b50
	{
		return false
	}

	tableCount := eod.numberOfCentralDirectoryHere
	cdTable := ptrToFl[eod.offsetToStartOfCD]&->{zipCentralDirectory^}
	

	for i : tableCount
	{	
		ptrToStr := cdTable[1]&->{char^}
		newStr := StringSpan(ptrToStr,cdTable.fileNameLen)

		result.Emplace() ; $temp
		newEn := ref result.Back()
		newEn.fileName = newStr
		newEn.offsetToCDEntry = cdTable&->{u64^}^ - ptrToFl&->{u64^}^
		newEn.cdEntrySize = zipCentralDirectory->TypeSize
					+ cdTable.fileNameLen 
					+ cdTable.extraFieldsLen
					+ cdTable.commentLen
		if newStr[-1..0] != "/"
		{
			ptTH := ptrToFl[cdTable.offsetToFileHeader]&->{zipFileHeader^}
			newEn.fhSize = zipFileHeader->TypeSize + ptTH.fileNameLen + ptTH.extraFieldsLen
			dataStart := ptTH->{u8^}[newEn.fhSize]&
			newEn.offsetToData = dataStart&->{u64^}^ - ptrToFl&->{u64^}^
			newEn.offsetToFileHeader = ptTH&->{u64^}^ - ptrToFl&->{u64^}^
			newEn.dataSize = cdTable.compressedSize
		}else{
			newEn.offsetToData = 0
		}

		cdTable = cdTable->{u8^}[newEn.cdEntrySize]&->{zipCentralDirectory^}
	}
	return true
}
ZipConCat := !(char^ inpFileI,char^ zipFileI, char^ outFileI) -> void
{
	inpFile := MappedFile(inpFileI)
	defer inpFile.Close()

	zipFile := MappedFile(zipFileI)
	defer zipFile.Close()

	inpSize := inpFile.Size()
	zipSize := zipFile.Size()
	totalSize := inpSize + zipSize
	outFile := file(outFileI,"wb")//MappedFile(outFileI,FILE_CREATE,totalSize)
	defer outFile.close()
	outFile.write(inpFile.point,inpSize)

	eod := zipFile.point[zipFile.Size() - zipEndOfDirectory->TypeSize]&->{zipEndOfDirectory^}

	ignores := Queue.{void^}() ; $temp
	ignores.Push(eod.offsetToStartOfCD&)

	filesCount := eod.numberOfCentralDirectoryHere
	cdTable := zipFile.point[eod.offsetToStartOfCD]&->{zipCentralDirectory^}
	for i : filesCount
	{
		ignores.Push(cdTable.offsetToFileHeader&)
		cdTable = cdTable->{u8^}[zipCentralDirectory->TypeSize 
			+ cdTable.fileNameLen 
			+ cdTable.extraFieldsLen
			+ cdTable.commentLen
			]&->{zipCentralDirectory^}
	}
	endOfPoint := zipFile.point[zipSize]&
	startPoint := zipFile.point->{u8^}
	for ignores.Size() + 1
	{
		metIgnore := false
		maxValue := endOfPoint
		for it : ignores
		{
			if it < startPoint
				continue
			if it < maxValue
			{
				maxValue = it
				metIgnore = true
			}
		}
		toWrite := maxValue - startPoint
		outFile.write(startPoint,toWrite)
		startPoint = startPoint[toWrite]&
		if metIgnore{
			offstValue := startPoint->{u32^}^
			offstValue += inpSize
			startPoint = startPoint[4]&
			outFile.write(offstValue&,4)
		}
	}
}

ZipAppend := !(char^ inpFile,List.{char^} toAdd) -> void
{
	ownFile := MappedFile(inpFile)

	aFF := List.{MappedFile}() ; $temp
	aFa := List.{List.{zipIntEntry}}() ; $temp
	for it : toAdd
	{
		aFF.Emplace(it)
		aFa.Emplace()
		if not ZipParse(aFF.Back().Get(),aFF.Back().Size(),aFa.Back())
			throw new Exception(0,"failed loading zip file")
	}
	meAsZP := List.{zipIntEntry}()
	itZip := ZipParse(ownFile.Get(),ownFile.Size(),meAsZP)

	oldZipCDData := List.{char[]}()
	
	newSize := ownFile.Size()
	oldSize := newSize


	if not itZip 
	{
		newSize += zipEndOfDirectory->TypeSize
	}else{
		for meAsZP
		{
			preAdd := new char[it.cdEntrySize] ; $temp
			memcpy(preAdd->{void^},ownFile.Get()->{u8^}[it.offsetToCDEntry]&,it.cdEntrySize)
			oldZipCDData.Push(preAdd)
			it.fileName = StringSpan(it.fileName.Str())
		}
	}
	
	preAdd := AVLSet.{StringSpan}()
	preAdd.Insert(meAsZP[^].fileName)

	for it : aFa
	{
		for entr : it
		{
			if preAdd.Contain(entr.fileName)
				continue
			preAdd.Insert(entr.fileName)
			newSize += entr.fhSize
			newSize += entr.cdEntrySize
			newSize += entr.dataSize
		}
	}
	if newSize == oldSize
		return void
	
	ownFile.Close()
	ownFile.Open(inpFile,FILE_WRITE,newSize)
	
	writeIter := u8^

	if itZip {
		writeIter = ownFile.Get()->{u8^}[meAsZP.Front().offsetToCDEntry]&
	}else{
		writeIter = ownFile.Get()->{u8^}[oldSize]&
	}
	fileStart := ownFile.Get()
	preAdd2 := AVLSet.{StringSpan}()
	for meAsZP preAdd2.Insert(it.fileName)
	for fils : aFa, itMap : aFF
	{
		for fils
		{
			if it.offsetToData == 0
				continue
			if preAdd2.Contain(it.fileName)
				continue
			preAdd2.Insert(it.fileName)
			newOffset := writeIter&->{u64^}^ - fileStart&->{u64^}^
			memcpy(writeIter,itMap.Get()[it.offsetToFileHeader]&,it.dataSize + it.fhSize)
			writeIter = writeIter[it.dataSize + it.fhSize]&
			it.offsetToFileHeader = newOffset
		}
	}
	preAdd3 := AVLSet.{StringSpan}()
	for meAsZP preAdd3.Insert(it.fileName)

	startOfCD := writeIter

	entrs := 0
	entrsSize := 0
	for it : meAsZP, oldD : oldZipCDData
	{
		memcpy(writeIter,oldD->{void^},it.cdEntrySize)
		if it.offsetToData != 0
			writeIter->{zipCentralDirectory^}.offsetToFileHeader = it.offsetToFileHeader
		writeIter = writeIter[it.cdEntrySize]&
		entrs += 1
		entrsSize += it.cdEntrySize
	}

	for fils : aFa, itMap : aFF
	{
		for fils
		{
			if preAdd3.Contain(it.fileName)
				continue
			preAdd3.Insert(it.fileName)
			memcpy(writeIter,itMap.Get()[it.offsetToCDEntry]&,it.cdEntrySize)
			if it.offsetToData != 0
				writeIter->{zipCentralDirectory^}.offsetToFileHeader = it.offsetToFileHeader
			writeIter = writeIter[it.cdEntrySize]&
			entrs += 1
			entrsSize += it.cdEntrySize
		}
	}
	lastData := writeIter->{zipEndOfDirectory^}
	memset(lastData,0,zipEndOfDirectory->TypeSize)
	lastData.offsetToStartOfCD = startOfCD&->{u64^}^ - fileStart&->{u64^}^ 
	lastData.sizeOfCentralDirectoryBytes = entrsSize
	lastData.numberOfCentralDirectoryHere = entrs
	lastData.totalNumberOfCentralDirectory = entrs
	lastData.signature = 0x06054b50
	printf("entrys %i\n",entrs)

}
