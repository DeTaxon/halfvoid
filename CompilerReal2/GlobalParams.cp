#import "Tree.cp"

CodeSpace := class
{
	codeLibs := Queue.{BoxFile^}
	codeParams := AVLMap.{string,QueueSet.{ObjParam^}}
}

CodeSpaces := AVLMap.{int,CodeSpace}
FilesInSpace := AVLSet.{Object^}

