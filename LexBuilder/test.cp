#import "../Libs/RegExpBuilder.cp"
#import "../Libs/WordParser.cp"
#import "../Libs/MappedFile.cp"
#import "../Libs/file.cp"
#import "../CompilerReal2/PriorityRead.cp"

main := !(int argc, char^^ argv) -> int{

	testFile := MappedFile("CompilerReal2/Lex.cp")

	if testFile.IsInvalid(){
		printf("cant read file\n")
		return 0
	}
	machFile := MappedFile("Mach.m")


	res := WordDetermMachine
	d := WordParser
	res.LoadFromMap(machFile.point,machFile.Size())
	bufff := char[1024]

	d.ReadText(res&->{void^},testFile.point,testFile.Size(), (a,b,c) =>
	{
		for i : c {
			bufff[i] = testFile.point->{char^}[b + i]
		}
		bufff[c] = 0
		if a == 20{
			printf("\n")
		}else{
			printf("<%i %s>",a,bufff)
		}
	})
	return 0
}
