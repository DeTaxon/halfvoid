#import "../Libs/RegExpBuilder.cp"
#import "../Libs/WordParser.cp"
#import "../Libs/MappedFile.cp"
#import "../Libs/file.cp"
#import "../CompilerReal2/PriorityRead.cp"

main := !(int argc, char^^ argv) -> int
{
	keywords := Queue.{string}()
	keywords.Push(":=")
	keywords.Push("=>")
	keywords.Push("extern")
	keywords.Push("at")
	keywords.Push("defer")
	keywords.Push("type")
	keywords.Push("virtual")
	
	pr := PriorityBag("Priority.pr")

	itBuilder := LexBuilder()

	itBuilder.ApplyReg("[a-zA-Z_][a-zA-Z0-9_]*",1)
	itBuilder.ApplyReg("[$#][a-zA-Z0-9_]*",2)
	itBuilder.ApplyReg("\"([^\"] | (\\\\ \"))*\"",3)
	itBuilder.ApplyReg("'( (\\\\ [^]) |  [^'\\\\])'",4)
	itBuilder.ApplyReg("[0-9]+",5)
	itBuilder.ApplyReg("[0-9]+.[0-9]+(e[+-]?[0-9]+)?",6)
	itBuilder.ApplyReg("0x[0-9a-fA-F]+",7)

	for pr.Opers itBuilder.ApplyReg(it,8)
	for keywords itBuilder.ApplyReg(it,8)

	itBuilder.ApplyReg("/ /[^\\n]*",9)
	itBuilder.ApplyReg("\\{ | \\} | \\( | \\) | \\[ | \\]",10)
	itBuilder.ApplyReg("[?,|]",10)
	itBuilder.ApplyReg("\\n",20)
	itBuilder.ApplyReg("; [^\\n]*",21)

	res := itBuilder.GenerateMachine()

	mSize := res.ComputeFileSize()
	outData := new u8[mSize]
	res.SerializeToMap(outData)
	outFile := file("Mach.m","wb")
	outFile.write(outData->{void^},outData->len)
	outFile.close()

	return 0
}
