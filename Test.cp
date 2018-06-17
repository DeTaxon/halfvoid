#import "lib.cp"
//#import "main.cp"
#import "MappedFileLinux.cp"

hi := !() -> void
{
	printf("hello\n")
	return void
	printf("word\n")
}

main := !(int argc, char^^ argv) -> int
{
	c := MappedFile("Test.cp")
	for c printf("%c",it)
	return 0
}

