
DebugMode := false
cuId := int
DebugMetaData := Stack.{string}

DebugCalls := Stack.{Tuple.{int,int,int}}
DebugLocalVars := Stack.{Tuple.{int,char^,int,int,int ,int}} //resId,name,scope,file,line, type

CreateDebugCall := !(Object^ itm) -> int
{
	if itm.Line == null return -1
	if not DebugMode return -1

	itr := itm
	while itr != null and itr.GetValue() != "!()" and itr.GetValue() != "{!()}"  itr = itr.Up

	if itr == null return -1

	asN := itr->{BoxFunc^}
	itId := GetNewId()
	DebugCalls.Emplace(itId,itm.Line.LinePos,asN.ABox.ItId)
	return itId
}
CreateDbgLocVar := !(ObjParam^ itm) -> int
{
	if itm.Line == null return -1

	itr := itm->{Object^}
	while itr != null and itr.GetValue() != "!()" and itr.GetValue() != "{!()}"  itr = itr.Up

	if itr == null return -1

	asN := itr->{BoxFunc^}
	
	while itr.Up != null itr = itr.Up

	asF := itr->{BoxFile^}

	newId := GetNewId()
	DebugLocalVars.Emplace(newId,itm.MyStr,asN.ABox.ItId,asF.fileId,itm.Line.LinePos,itm.ObjType.metaId)
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
}
