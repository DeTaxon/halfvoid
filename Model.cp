Model := class
{
	Vert := float^
	Ind := int^
	draw := !() -> void
	{
	}
	LoadFromPly := !(char^ Name) ->bool
	{
		File := sfile
		MyLst := Queue.{char^} 
		//TString Temp;
		//Queue<TString> MyLst;

		File.open(Name, "r")

		if not File.IsOpen() { puts("File not found") return false }

		while File.good()
		{
			Line := File.readline()
			if Line[0] != 0 MyLst.Push(Line)
		}

		//MyFile.close();

		Header := true
		//Queue<TString> TmpLst
		VCount := 0

		//while(Header)
		//{
		//	TmpLst.Clear();
		//	MyLst.Pop(Temp);
		//	Temp.Divide(' ', TmpLst);
		//	if (TmpLst[0] == "element" && TmpLst[1] == "vertex")
		//	{
		//		VCount = TmpLst[2].ToInt();
		//	}
		//	else if (TmpLst[0] == "end_header")
		//	{
		//		Header = false;
		//	}
		//}
		//int Items = (3+3+2);// 	GetVertElem();
		//float* MyVerts = new float[VCount*Items];
		//Queue<int> IndData;
		//int* MyInd;
		//for (int i = 0; i < VCount; i++)
		//{
		//	MyLst.Pop(Temp);
		//	TmpLst.Clear();
		//	Temp.Divide(' ',TmpLst);
		//	for (int j = 0; j < Items; j++)
		//		MyVerts[i*Items + j] = TmpLst[j].ToFloat();
		//}

		//int Value = 0;
		//while (!MyLst.Empty())
		//{
		//	MyLst.Pop(Temp);
		//	TmpLst.Clear();
		//	Temp.Divide(' ', TmpLst);
		//	Value = TmpLst[0].ToInt();
		//	if (Value == 3)
		//	{
		//		Value = TmpLst[1].ToInt();
		//		IndData.Push(Value);

		//		Value = TmpLst[2].ToInt();
		//		IndData.Push(Value);

		//		Value = TmpLst[3].ToInt();
		//		IndData.Push(Value);
		//	}
		//	else if(Value == 4)
		//	{
		//		Value = TmpLst[1].ToInt();
		//		IndData.Push(Value);

		//		Value = TmpLst[2].ToInt();
		//		IndData.Push(Value);

		//		Value = TmpLst[3].ToInt();
		//		IndData.Push(Value);

		//		Value = TmpLst[1].ToInt();
		//		IndData.Push(Value);

		//		Value = TmpLst[3].ToInt();
		//		IndData.Push(Value);

		//		Value = TmpLst[4].ToInt();
		//		IndData.Push(Value);
		//	}
		//}
		//MyInd = IndData.ToArray();
		//int DrawCount = IndData.Size();

		//Create(MyVerts,VCount*8*4,MyInd,DrawCount*4);
		//if (MyInd) delete[] MyInd;
		//if (MyVerts) delete[] MyVerts;
		//return true;
		return true
	}
}
