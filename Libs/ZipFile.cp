
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
	total_in := u64

	next_out := void^
	avail_out := u32
	total_out := u64

	msg := char^
	state := void^

	zalloc := void^
	zfree := void^
	opaque := void^

	data_type := int
	adler := u64
	reserved := u64
}

inflateInit2_ := !(z_stream^ a,int b,char^ c,u64 d) -> int declare
inflate := !(z_stream^ a,int b) -> int declare
inflateEnd := !(z_stream^ a) -> int declare


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
		items := Queue.{char^}()
		DivideStr(fileNameI,"\\/",items) ; $temp
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

