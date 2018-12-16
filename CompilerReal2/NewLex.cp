#import "Lex.cp"
#import "../Libs/MappedFile.cp"

//lexWordMachine := WordDetermMachine 

//LoadLexMachine := !() -> void
//{
//	machFile := MappedFile("Mach.m")
//	lexWordMachine.LoadFromMap(machFile.point,machFile.Size())
//	machFile.Close()
//}

//GetTokensFromFile2 := !(Path name,Queue.{Token^} toksOut) -> void
//{
//	inputFile := MappedFile(name.itStr)
//	d := WordParser
//	bufff := char[1024]
//	d.ReadText(lexWordMachine&->{void^},testFile.point,testFile.Size(), (a,b,c) =>
//	{
//		ptr := inputFile.point->{char^}[b]&
//		for i : c {
//			bufff[i] = ptr[i]
//		}
//		bufff[c] = 0
//		toksOut.Push(StrCopy(buff))
//	})
//	inputFile.Close()
//}

//GetObjectsFromFile2 := !(Path fileName) -> Object^
//{
//	Buf := Queue.{Token^}()
//	if not GetTokensFromFile2(fileName, LexMachine^,Buf)
//		return null
//	
//	for iterC : Buf
//	{
//		iterG := PriorityData.Opers.Start
//		while iterG != null
//		{
//			if iterG.Data == iterC.Buff{
//				iterC.Id = 10
//			}
//			iterG = iterG.Next
//		}
//	}
//
//	Ob := TokensToObjects(fileName,Buf)
//	UniteSkobs(Ob)
//	Buf.Clean()
//	return Ob
//}




