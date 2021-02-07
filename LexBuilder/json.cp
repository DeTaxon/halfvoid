main := !(int argc, char^^ argv) -> int
{
	itBuilder := LexBuilder()

	itBuilder.ApplyReg("\"((\\\\ [^]) | [^\"\\\\])*\"",1)
	itBuilder.ApplyReg("[0-9]+(.[0-9]+)?",2)
	itBuilder.ApplyReg(" true | false | null",3)
	itBuilder.ApplyReg("\\{ | \\} | \\, | \\: | \\[ | \\]",4)
	itBuilder.ApplyReg("\\ | \\t | \\n",5)

	res := itBuilder.GenerateMachine()

	mSize := res.ComputeFileSize()
	outData := new u8[mSize]
	res.SerializeToMap(outData)
	outFile := file("json.m","wb")
	outFile.write(outData->{void^},outData->len)
	outFile.close()

	return 0
}
