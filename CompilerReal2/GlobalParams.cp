#import "Tree.cp"

ForcedLibs := Queue.{BoxFile^}
ForcedGlobalParams := AVLMap.{string,QueueSet.{ObjParam^}}
GlobalParams := AVLMap.{string,QueueSet.{ObjParam^}}

CodeSpace := class
{
	codeLibs := Queue.{BoxFile^}
	codeParams := AVLMap.{string,QueueSet.{ObjParam^}}
}

CodeSpaces := AVLMap.{int,CodeSpace}
FilesInSpace := AVLSet.{Object^}

