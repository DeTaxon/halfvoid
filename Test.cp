#import "lib.cp"
//#import "main.cp"
#import "MappedFileLinux.cp"

main := !(int argc, char^^ argv) -> int
{
	b := "Z"[0]
	a := "A"[0]
	c := MappedFile("tmp.txt",FILE_CREATE,b - a + 1)
	i := a
	for c {
		it = i
		i += 1
	}
	c.Close()

	return 0
}

