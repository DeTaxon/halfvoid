
DebugMode := false
cuId := int
DebugMetaData := Stack.{string}

DebugCalls := Stack.{Tuple.{int,int,int}}
DebugLocalVars := Stack.{Tuple.{int,char^,int,int,int ,int}} //resId,name,scope,file,line, type

DebugPretty := List.{char^}

CreateDebugCall := !(Object^ itm) -> int
{
	if itm.Line == null return -1
	if not DebugMode return -1

	//itr := itm
	//while itr != null and itr.GetValue() != "!()" and itr.GetValue() != "{!()}"  
	//	and not itr is SLambda  itr = itr.Up

	//if itr == null return -1

	//aBId := int
	//if itr is SLambda
	//{
	//	aBId = itr->{SLambda^}.ABox.ItId
	//}else{
	//	aBId = itr->{BoxFunc^}.ABox.ItId
	//}
	aBId := itm.GetScope()
	if aBId == 0 return -1
	itId := GetNewId()
	DebugCalls.Emplace(itId,itm.Line.LinePos,aBId)
	return itId
}
CreateDbgLocVar := !(Object^ itm,Type^ itType,char^ itName) -> int
{
	return CreateDbgLocVar(itm,itType,itName,false)
}
CreateDbgLocVar := !(Object^ itm,Type^ itType,char^ itName,bool isRef) -> int
{
	if itm.Line == null return -1

	itr := itm->{Object^}
	//while itr != null and itr.GetValue() != "!()" and itr.GetValue() != "{!()}"  
	//	and not itr is SLambda  itr = itr.Up

	//if itr == null return -1
	//
	//aBId := int
	//if itr is SLambda
	//{
	//	aBId = itr->{SLambda^}.ABox.ItId
	//}else{
	//	aBId = itr->{BoxFunc^}.ABox.ItId
	//}
	aBId := itm.GetScope()
	if aBId == 0 return -1

	while itr.Up != null itr = itr.Up

	asF := itr->{BoxFile^}
	
	typeMetaId := itType.metaId
	if isRef typeMetaId = GetDebugRef(itType) 
	newId := GetNewId()
	DebugLocalVars.Emplace(newId,itName,aBId,asF.fileId,itm.Line.LinePos,typeMetaId)
	return newId
}

PrintDebugMeta := !(sfile fil) -> void
{
	fileId := Files.Start.Data.fileId
	fil << "!llvm.dbg.cu = !{!"<< cuId <<"}\n"
	fil << "!" << cuId << "= distinct !DICompileUnit(producer: \"max\", isOptimized: false, runtimeVersion: 0,emissionKind: FullDebug, enums: !{},globals: !{}, language: DW_LANG_C99, file: !"<< fileId <<" )\n"

	nm := GetNewId()
	fil << "!" << nm << " = !{!\"Taxon's stupid compiler\"}\n" 
	fil << "!llvm.ident = !{!" << nm <<"}\n"
	fil << "declare void @llvm.dbg.declare(metadata, metadata, metadata)\n"
	fil << "declare void @llvm.dbg.value(metadata, metadata, metadata)\n"
	fil << "declare void @llvm.dbg.addr(metadata, metadata, metadata)\n"

	dwrdV := GetNewId() fil << "!" << dwrdV << " = !{i32 2, !\"Dwarf Version\", i32 4}\n"
	DbgInf := GetNewId() fil << "!" << DbgInf << " = !{i32 2, !\"Debug Info Version\",i32 3}\n"
	WchSize := GetNewId() fil << "!" << WchSize << " = !{i32 1, !\"wchar_size\",i32 4}\n"
	PicLevel := GetNewId() fil << "!" << PicLevel << " = !{i32 7, !\"PIC Level\", i32 2}\n"
	PieLevel := GetNewId() fil << "!" << PieLevel << " = !{i32 7, !\"PIE Level\", i32 2}\n"
	fil << "!llvm.module.flags = !{!" << dwrdV << ",!" << DbgInf << ",!" << WchSize << ",!" << PicLevel << ",!" << PieLevel << "}\n"
	for DebugCalls
		fil << "!" << it.0 << " = !DILocation(line:" << it.1 << ",column:1, scope:!" << it.2 << ")\n"
	for DebugLocalVars
		fil << "!" << it.0 << " = !DILocalVariable(name: \"" << it.1 << "\" , scope:!" << it.2 << " , file:!" << it.3 << ", line: " << it.4 << ", type:!" << it.5 << ")\n"

	for DebugMetaData fil << it


	if DebugPretty.Size() != 0
	{
		fattyPrint := (char^ pStr) ==> {
			miniBuff := char[1024]
			c := 0
			i := 0
			while pStr[i] != 0
			{
				if pStr[i] in "\"\n"
				{
					switch pStr[i]
					{
						case '\"'
							miniBuff[c++] = '\\'
							miniBuff[c++] = '5'
							miniBuff[c++] = 'C'
							miniBuff[c++] = '\\'
							miniBuff[c++] = '2'
							miniBuff[c++] = '2'
						case '\n'
							miniBuff[c++] = '\\'
							miniBuff[c++] = '5'
							miniBuff[c++] = 'C'
					}
					if pStr[i] == '\n' and pStr[i+1] == 0
					{
						c -= 3
					}
				}else{
					miniBuff[c++] = pStr[i]
				}
				i += 1
			}
			miniBuff[c] = 0
			fil << miniBuff[0]&
		}
		fil << "module asm \".pushsection \\22.debug_gdb_scripts\\22, \\22MS\\22, @progbits,1\"\n"
		fil << "module asm \".byte 4\"\n"
		for DebugPretty
		{
			fil << "module asm \".asciz \\22"
			fattyPrint(it)
			fil << "\\5Cn\\22\"\n"
		}
		fil << "module asm \".byte 0\"\n"
		fil << "module asm \".popsection\"\n"
	}
}
