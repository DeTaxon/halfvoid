#import "../CompilerReal2/PriorityRead.cp"

main := !(int argc, char^^ argv) -> int
{

	itM := MappedFile("Priority.pr")
	defer itM.Close()
	pr := PriorityBag(itM.point,itM.Size())

	itBuilder := LexBuilder()

	itBuilder.ApplyReg("$?[a-zA-Z_][a-zA-Z0-9_]*",1)
	itBuilder.ApplyReg("(@ | #)[a-zA-Z0-9_]*",2)
	itBuilder.ApplyReg("\"((\\\\ [^]) | [^\"\\\\])*\"",3)
	itBuilder.ApplyReg("'( (\\\\ [^]) | [^ '\\\\])  '",4)
	itBuilder.ApplyReg("[0-9]+",5)
	itBuilder.ApplyReg("[0-9]+.[0-9]+",6)
	itBuilder.ApplyReg("0x[0-9a-fA-F]+",7)



	tmpArr := new List.{char^}
	allOpers := new AVLSet.{char^} ; $temp

	tmpArr.Push(pr.Opers[^])
	tmpArr.Push(LexKeyWords[^])
	tmpArr.Push(AllKeyWords[^])

	for tmpArr^ {
		buff2 := char[64]
		k := 0
		for c : it
		{
			if c in "-+?[]^*|()"
			{
				buff2[k] = '\\'
				buff2[k+1] = c
				k += 2
			}else{
				buff2[k] = c
				k += 1
			}
		}
		buff2[k] = 0
		itBuilder.ApplyReg(buff2,8)
		allOpers.Insert(StrCopy(buff2))
	}
	printf("itms %i\n",allOpers.Size())
	for it,i : allOpers^
	{
		itBuilder.ApplyReg(it,50 + i)
	}

	//itBuilder.ApplyReg("\\( | \\) | \\{ | \\. | \\} | \\[ | \\]",8)
	//itBuilder.ApplyReg("\\? | \\, | \\! | \\| | \\.",8)
	itBuilder.ApplyReg("\\(",40)
	itBuilder.ApplyReg("\\{",41)
	itBuilder.ApplyReg("\\[",42)
	itBuilder.ApplyReg("\\)",43)
	itBuilder.ApplyReg("\\}",44)
	itBuilder.ApplyReg("\\]",45)

	itBuilder.ApplyReg("/ /[^\n]*",9)
	itBuilder.ApplyReg("\\ | \\t",10) // space
	itBuilder.ApplyReg("\n",20)
	itBuilder.ApplyReg("; [^\n]*",21)

	res := itBuilder.GenerateMachine()

	mSize := res.ComputeFileSize()
	outData := new u8[mSize]
	res.SerializeToMap(outData)
	outFile := file("Mach.m","wb")
	outFile.write(outData->{void^},outData->len)
	outFile.close()

	return 0
}
