
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
