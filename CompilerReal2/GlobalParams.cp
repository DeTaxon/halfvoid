CodeSpace := class
{
	codeLibs := Queue.{BoxFile^}
	codeParams := AVLMap.{char^,QueueSet.{ObjParam^}}
	codeTypeDefs := AVLMap.{char^,TypeDef^}
	codeTemplateTypedefs := AVLMap.{char^,Queue.{BoxTemplateTypedef^}}
}

CodeSpaces := AVLMap.{int,CodeSpace}
FilesInSpace := AVLSet.{Object^}

InfiniteCodeSpace := CodeSpace^

