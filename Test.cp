#import "lib.cp"
//#import "main.cp"
//#import "MappedFileLinux.cp"

hi := !() -> void
{
	printf("hello\n")
	return void
	printf("word\n")
}

main := !(int argc, char^^ argv) -> int
{
	//c := MappedFile("Test.cp")
	//if c.itemId == 0 - 1
	//{
	//	//printf("invalid file\n")
	//}else{
	//	printf("part %i %p\n",c.size,c.point)
	//	//for ch : c printf("%c\n",ch)
	//	//c.For()
	//	//for 1000 printf("%c",c.point[it])
	//}
	//printf("here %X %X %X %X\n", 0xA0 or_b 0x0E, 0xFF and_b 0xDA, 0xFF xor_b 0xDA,not_b 0xCA)
	return 0
}

