GetBoxFor := !(Object^ dat) -> BoxFor^
{

	itemName := string
	indName := string

	itemName = null
	indName = null

	iterY := dat.Down.Right

	if iterY.Right.GetValue() == ","
	{
		itemName = iterY->{ObjIndent^}.MyStr
		indName = ((iterY.Right.Right)->{ObjIndent^}).MyStr
		
		iterY = iterY.Right.Right.Right
	}else
	{
		if iterY.Right.GetValue() == ":"
		{
			itemName = iterY->{ObjIndent^}.MyStr
			indName = null

			iterY = iterY.Right.Right
		}
	}
	if iterY.Right.GetValue() == ","
	{
		Names := Queue.{string}()
		Downs := Queue.{Object^}()
		Downs.Push(iterY)
		if itemName == null{
			Names.Push("it")
		}else{
			Names.Push(itemName)
		}

		iterY = iterY.Right
		while iterY.GetValue() == ","
		{
			iterY = iterY.Right
			asNeed := iterY->{ObjIndent^}
			Names.Push(asNeed.MyStr)
			iterY = iterY.Right.Right
			Downs.Push(iterY)
			iterY = iterY.Right
		}

		return new BoxForOldFashionMulti(Names,indName,Downs,iterY)
	}
	//TODO: check type

	return new BoxFor(itemName,indName,iterY,iterY.Right)


	//return null
}

BoxFor := class extend Object
{
	LocPar := MemParam^
	IndPar := MemParam^
	itName := string
	indName := string
	block := Object^

	this := !(string inN, string indN,Object^ Dwn, Object^ blk) -> void
	{
		itName = inN
		indName = indN
		block = blk
		Down = Dwn
		Down.Left = null
		Down.Right = null
		Dwn.Up = this&
		block.Up = null

		WorkBag.Push(this&,State_GetUse)
		WorkBag.Push(Down,State_PreGetUse)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_GetUse
		{
			newNode := Object^
			newNode = null
			if Down.GetType() == TypeTable[13] 
			{
				 newNode = new BoxForRange(this.itName,this.indName,this.Down,this.block)
			}
			if Down.GetType() == GetType("int")
			{
				newNode = new BoxForInt(this.itName,this.indName,null->{Object^},Down,this.block)
			}
			if Down.GetType().GetType() == "class"
			{
				asNeed := Down.GetType()->{TypeClass^}
				asNeed2 := asNeed.ToClass

				func := asNeed2.GetFunc("~For")

				if func != null
					newNode = new BoxForOldFashion(this.itName,this.indName,func,Down,this.block)

			}
			if newNode != null
			{
				newNode.Line = this.Line
				ReplaceNode(this&,newNode)
			}
		}
	}

}
BoxForOldFashionMulti := class extend BoxFor
{
	ItId := int

	itemsCount := int
	IncFuncs := Object^^
	UnrefFuncs := Object^^
	IsEndFuncs := Object^

	ProxyFuncs := BoxFunc^^

	this := !(Queue.{string} names, string f_ind,Queue.{Object^} items,Object^ block) -> void
	{
		Down = block
		itemsCount = items.Size()
		iter := Down

		for i : itemsCount
		{
			iter.Right = items[i]
			iter.Right.Left = iter
			iter = iter.Right
		}
		iter.Right = null
		Down.SetUp(this&)
		

		WorkBag.Push(this&,State_PreGetUse)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_PreGetUse
		{
			WorkBag.Push(this&,State_GetUse)
			iter := Down
			while iter != null
			{
				//WorkBag.Push(iter,State_Start)
				printf("wut %s\n",iter.GetValue())
				iter = iter.Right
			}
			//WorkBag.Push(Down.Right.Right,State_Start)
		}
		if pri == State_GetUse
		{
			IncFuncs = new Object^[itemsCount]
			UnrefFuncs = new Object^[itemsCount]
			ProxyFuncs = new BoxFunc^[itemsCount]

			iter := Down.Right
			for i : itemsCount
			{
				Pars := Queue.{Type^}()
				Pars.Push(iter.GetType())

				//func := FindFunc("~For",this&,Pars,false)
				//ProxyFuncs[i] = func
				iter = iter.Right
			}
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		//Down.PrintPre(f)

		//f << "br label %start" << ItId << "\n"
		//f << "start" << ItId << ":\n"
		//IsEndFunc.PrintPre(f)
		//f << "br i1 " << IsEndFunc.GetName() << " , label %End" << ItId << " , label %Next" << ItId << "\n"
		//f << "Next" << ItId << ":\n"

		//if UnrefFunc.IsRef() UnrefFunc.PrintPointPre(f) else UnrefFunc.PrintPre(f)
		//
		//Down.Right.PrintInBlock(f)
		//IncFunc.PrintPre(f)
		//f << "br label %start" << ItId << "\n"
		//f << "End" << ItId << ":\n"

	}
	GetValue := virtual !() -> string
	{
		return "~for()"
	}
}

BoxForOldFashion := class extend BoxFor
{
	ItId := int

	IncFunc := Object^
	UnrefFunc := Object^
	IsEndFunc := Object^

	ProxyFunc := BoxFunc^

	this := !(string f_it, string f_ind,BoxFunc^ Proxy, Object^ item,Object^ block) -> void
	{
		Proxy.ParseBlock()

		itName = f_it
		if itName == null itName = "it"

		indName = f_ind
		if indName == null indName = "it_ind"
		PopOutNode(item)
		PopOutNode(block)
		Down = MakeSimpleCall(Proxy,item)
		Down.Right = block
		block.Right = null
		block.Left = Down
		Down.Left = null
		Down.SetUp(this&)

		MakeItBlock(Down.Right)
		ProxyFunc = Proxy

		WorkBag.Push(this&,State_GetUse)
		WorkBag.Push(Down,State_GetUse)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_GetUse
		{
			ItIdPre := Down->{SomeFuncCall^}
			ItId = ItIdPre.GetItAllocId()

			asNeedPre := ProxyFunc.MyFuncType.RetType
			asNeedPre2 := asNeedPre->{TypeClass^}
			asNeed := asNeedPre2.ToClass

			IsEndFuncP := asNeed.GetFunc("IsEnd")
			UnrefFuncP := asNeed.GetFunc("^")
			IncFuncP := asNeed.GetFunc("Inc")

			IsGood := true

			if IsEndFuncP == null IsGood = false
			if UnrefFuncP == null IsGood = false
			if IncFuncP == null IsGood = false

			if IsGood
				if IsEndFuncP.MyFuncType.RetType != GetType("bool") IsGood = false

			if IsGood
			{
				IsEndFuncP.ParseBlock()
				UnrefFuncP.ParseBlock()
				IncFuncP.ParseBlock()

				ForItem := new LocalParam(asNeedPre,ItId)
				
				test := new ParamNaturalCall("",ForItem->{Object^})
				IsEndFunc = MakeSimpleCall(IsEndFuncP, test )
				test = new ParamNaturalCall("",ForItem->{Object^})
				UnrefFunc = MakeSimpleCall(UnrefFuncP,test)
				test = new ParamNaturalCall("",ForItem->{Object^})
				IncFunc = MakeSimpleCall(IncFuncP,test)

				LocPar = new RetFuncParam(UnrefFunc)

				WorkBag.Push(Down.Right,State_Start)
			}else{
				ErrorLog.Push("Incorect For class\n")
			}
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		Down.PrintPre(f)

		f << "br label %start" << ItId << "\n"
		f << "start" << ItId << ":\n"
		IsEndFunc.PrintPre(f)
		f << "br i1 " << IsEndFunc.GetName() << " , label %End" << ItId << " , label %Next" << ItId << "\n"
		f << "Next" << ItId << ":\n"

		if UnrefFunc.IsRef() UnrefFunc.PrintPointPre(f) else UnrefFunc.PrintPre(f)
		
		Down.Right.PrintInBlock(f)
		IncFunc.PrintPre(f)
		f << "br label %start" << ItId << "\n"
		f << "End" << ItId << ":\n"

	}
	GetValue := virtual !() -> string
	{
		return "~for()"
	}
}

BoxForInt := class extend BoxFor
{
	thisId := int
	this := !(string f_it, string f_ind,Object^ start, Object^ end, Object^ stuf) -> void
	{	
		thisId =  GetNewId()

		itName = f_it
		if itName == null itName = "it"

		indName = f_ind
		if indName == null indName = "it_ind"

		itemStart := start
		if start == null itemStart = new ObjInt(0)
		
		parId := GetAlloc(stuf,GetType("int"))
		LocPar = new ConstParam("iterInt" + thisId ,GetType("int"))
		IndPar = LocPar
		
		Down = stuf
		stuf.Left = null
		stuf.Right = itemStart
		itemStart.Right = end
		itemStart.Left = stuf
		end.Left = itemStart
		end.Right = null
		Down.SetUp(this&)
		MakeItBlock(stuf)

		WorkBag.Push(Down.Right,State_Start)
		WorkBag.Push(Down.Right.Right,State_Start)
		WorkBag.Push(Down,State_Start)

	}
	GetValue := virtual !() -> string
	{
		return "~for()"
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(Down,State_Start)
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		stuf := Down
		beg := Down.Right
		end := beg.Right

		IntName := string
		IntName = ((LocPar->{ConstParam^}).itName)

		beg.PrintPre(f)
		end.PrintPre(f)

		f << "br label %Start" << thisId <<"\n"
		f << "Start" << thisId << ":\n"
		f << "%GoingTo" << thisId << " = add i32 " << end.GetName() << " , 0\n"

		f << "br label %Check" << thisId <<"\n"
		f << "Check" << thisId << ":\n"
		f << IntName << " = phi i32 [0, %Start" << thisId << "] , [%nextInd" << thisId << " , %PreEnd" << thisId << " ]\n"
		f << "%Res" << thisId << " = icmp slt i32 "<< IntName << " ,%GoingTo" << thisId << "\n"
		f << "br i1 %Res" << thisId << ", label %Begin" << thisId << " , label %End" << thisId <<"\n"
		
		f << "Begin" << thisId <<":\n"
		stuf.PrintInBlock(f)
		f << "br label %PreEnd" << thisId << "\n"
		f << "PreEnd" << thisId << ":\n"
		f << "%nextInd" << thisId << " = add i32 1," << IntName << "\n"
		f << "br label %Check" << thisId << "\n"
		f << "End" << thisId <<":\n"
	}
}

BoxForRange := class extend BoxFor
{
	thisId := int
	this := !(string f_it, string f_ind,Object^ start, Object^ stuf) -> void
	{	
		thisId =  GetNewId()

		itName = f_it
		if itName == null itName = "it"

		indName = f_ind
		if indName == null indName = "it_ind"

		itemStart := start
		if start == null itemStart = new ObjInt(0)
		
		parId := GetAlloc(stuf,GetType("int"))
		LocPar = new ConstParam("iterInt" + thisId ,GetType("int"))
		IndPar = LocPar
		
		Down = stuf
		stuf.Left = null
		stuf.Right = itemStart
		itemStart.Right = null
		itemStart.Left = stuf
		Down.SetUp(this&)
		MakeItBlock(stuf)

		WorkBag.Push(stuf,State_Start)

	}
	GetValue := virtual !() -> string
	{
		return "~for()"
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(Down,State_Start)
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		stuf := Down
		beg := Down.Right

		IntName := string
		IntName = ((LocPar->{ConstParam^}).itName)

		beg.PrintPre(f)

		f << "br label %Start" << thisId <<"\n"
		f << "Start" << thisId << ":\n"
		f << "%GoingTo" << thisId << " = extractvalue %RangeTypeInt " + beg.GetName()  +",1\n"
		f << "%startVal" << thisId << " = extractvalue %RangeTypeInt " +beg.GetName() +",0\n"

		f << "br label %Check" << thisId <<"\n"
		f << "Check" << thisId << ":\n"
		f << IntName << " = phi i32 [%startVal" <<thisId << ", %Start" << thisId << "] , [%nextInd" << thisId << " , %PreEnd" << thisId << " ]\n"
		f << "%Res" << thisId << " = icmp sle i32 "<< IntName << " ,%GoingTo" << thisId << "\n"
		f << "br i1 %Res" << thisId << ", label %Begin" << thisId << " , label %End" << thisId <<"\n"
		
		f << "Begin" << thisId <<":\n"
		stuf.PrintInBlock(f)
		f << "br label %PreEnd" << thisId << "\n"
		f << "PreEnd" << thisId << ":\n"
		f << "%nextInd" << thisId << " = add i32 1," << IntName << "\n"
		f << "br label %Check" << thisId << "\n"
		f << "End" << thisId <<":\n"
	}
}
