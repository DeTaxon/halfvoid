Model := class
{
	Vert := float^
	Ind := int^
	Tris := int
	Size := int

	this := !(char^ Line) -> void
	{
		this.LoadFromPly(Line)
	}
	draw := !() -> void
	{
		glBegin(GL_TRIANGLES)
		for i : Tris for j : 3
		{
			Pos := Ind[i*3 + j]*Size
			glColor3fv(Vert[Pos+3]&)
			glVertex3fv(Vert[Pos]&)
		}
		glEnd()
	}
	LoadFromPly := !(char^ Name) ->bool
	{
		File := sfile
		MyLst := Queue.{char^}()
		MyLst.Start = null
		Divs := Queue.{char^}()
		Divs.Start = null
		File.open(Name, "r")

		if not File.IsOpen() { puts("File not found") return false }

		while File.good()
		{
			Line := File.readline()
			if Line != null MyLst.Push(Line)
		}
		File.close()
		Header := true
		VCount := 0

		while Header
		{
			while Divs.NotEmpty() free(Divs.Pop())
			Line := MyLst.Pop()
			DivideStr(Line," \n", Divs)
			if Divs.Start.Data == "element" if Divs.Start.Next.Data == "vertex"
			{
				VCount = StrToInt(Divs.Start.Next.Next.Data)
			}
			if Divs.Start.Data == "end_header"
			{
				Header = false
			}
		}

		Items := (3+3+2) // 	GetVertElem();
		MyVerts := new float[VCount*Items]
		IndData := Queue.{int}()
	
		for i :  VCount 
		{
			Temp := MyLst.Pop()
			while Divs.NotEmpty() free(Divs.Pop())
			DivideStr(Temp," \n",Divs)
			//for j : Items MyVerts[i*Items + j] = TmpLst[j])
			Iter := Divs.Start
			j := 0
			while Iter != null
			{	
				MyVerts[i*Items + j] = StrToFloat(Iter.Data)
				j += 1
				Iter = Iter.Next
			}
		}

		Value := 0
		while MyLst.NotEmpty()
		{
			Temp := MyLst.Pop()
			while Divs.NotEmpty() free(Divs.Pop())
			DivideStr(Temp," \n",Divs)
			Value = StrToInt(Divs.Start.Data)
			if (Value == 3)
			{
				for i : ![1,2,3]
				{
					Value = StrToInt(Divs[i])
					IndData.Push(Value)
				}
			}
			if(Value == 4)
			{
				for i : ![1,2,3,1,3,4]
				{
					Value = StrToInt(Divs[i])
					IndData.Push(Value)
				}
			}
		}
		Vert = MyVerts
		Ind = IndData.ToArray()
		Size = Items
		Tris = IndData.Size() / 3
		return true
	}
}
