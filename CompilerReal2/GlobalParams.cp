#import "Tree.cp"

CodeSpace := class
{
	codeLibs := Queue.{BoxFile^}
	codeParams := AVLMap.{string,QueueSet.{ObjParam^}}
	codeTypeDefs := AVLMap.{string,TypeDef^}
}

CodeSpaces := AVLMap.{int,CodeSpace}
FilesInSpace := AVLSet.{Object^}

