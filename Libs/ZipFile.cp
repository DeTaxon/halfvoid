#import "ZipSpec.cp"
#import "DeflateEncoder.cp"



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

prvtInitStream := !(z_stream^,int,char^,u64)^ -> int
prvtInflate := !(z_stream^,int)^ -> int
prvtInflateEnd := !(z_stream^)^ -> int

prvtZipInited := bool
prvtInitZip := !() -> void
{
	if prvtZipInited
		return void
	prvtZipInited = true

	dllHandle := dlopen("libz.so",2)

	if dllHandle == 0
	{
		return void
	}

	prvtInitStream = dlsym(dllHandle,"inflateInit2_")
	prvtInflate = dlsym(dllHandle,"inflate")
	prvtInflateEnd = dlsym(dllHandle,"inflateEnd")
}

vZipEntry := class
{
	ptrToObj := vZipObject^
	offset := int
	realSize := u32
	zipSize := u32
	objName := StringSpan
	fullName := StringSpan
	comprType := int
	subFolders := Queue.{vZipEntry^}
	compressedPointer := void^

	"this" := !() -> void {}

	Size := !() -> size_t
	{
		return realSize
	}
	Map := !() -> void^
	{
		ptrToObj.AddUser()
		if comprType == 8
		{
			if not prvtZipInited
				prvtInitZip()
			compressedPointer = malloc(realSize)

			resPtr := ptrToObj.asMapped.point[offset]&
	
			sStream := z_stream
			memset(sStream&,0,z_stream->TypeSize)
			sStream.avail_in = zipSize
			sStream.avail_out = realSize
			sStream.next_in = resPtr
			sStream.next_out = compressedPointer
			//prvtDeflateInflate(resPtr,zipSize,compressedPointer,realSize)

			aa := prvtInitStream(sStream&,-15,"1.2.11",z_stream->TypeSize)
			bb := prvtInflate(sStream&,0)
			cc := prvtInflateEnd(sStream&)
			//printf("test %i %i %i\n",aa,bb,cc)
			//printf("hoh %i\n",z_stream->TypeSize)

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
	miniStack := Stack.{vZipEntry^,32}

	this := !(vZipEntry^ nd) .{} -> void
	{
		miniStack."this"()
		if nd != null 
			miniStack.Push(nd) ; $temp
	}
	IsEnd := !() -> bool { return miniStack.Empty() }
	"^" := !() -> ref vZipEntry^ { return miniStack.Front() }
	Inc := !() .{} -> void {
		nnd := miniStack.Front()
		miniStack.Pop()
		for nnd.subFolders
		{
			miniStack.Push(it)
		}
	}
	DoAMoveRight := !() -> void
	{
		while nNode.Right != null
		{
			if nNode.Left != null
				miniStack.Emplace(nNode.Left,true)
			miniStack.Emplace(nNode,false)
			nNode = nNode.Right
		}
		if nNode.Left != null
			miniStack.Emplace(nNode.Left,false)
	}
}

vZipObject := class
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

	AnalizeFile := !(char^ fileToLoad) -> bool
	{
		fileName = StrCopy(fileToLoad)
		AddUser()

		ptrToFl := asMapped.point
		eod := ptrToFl[asMapped.Size() - zipEndOfDirectory->TypeSize]&->{zipEndOfDirectory^}

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
	}
}

