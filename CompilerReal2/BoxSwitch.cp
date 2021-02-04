ForInSwapNodes := !(Object^ onLeft) -> void
{
	up := onLeft.Up
	lft := onLeft
	rt := onLeft.Right
	rt.Left = lft.Left
	lft.Right = rt.Right
	rt.Right = lft
	lft.Left = rt
	if up.Down == lft up.Down = rt
}

BoxSwitch := class extend Object
{
	id := int
	caseIdIter := int
	itemCall  := MemParam^
	addedQ := List.{QuestionBox^}
	
	isAllowTreeOpt := bool
	this := !(Object^ itm) -> void
	{
		Line = itm.Line
		Down = itm.Down
		Down.SetUp(this&)
		PopOutNode(Down)
		MakeItBlock(Down.Right,false)
		id = GetNewId()
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if visitedWork[pri] return void
		else visitedWork << pri

		if pri == State_Start
		{
			WorkBag.Push(this&,State_PreGetUse)
			while TryParseMacro(Down,this&) != null {}

			WorkBag.Push(this&,State_GetUse)
			WorkBag.Push(Down,State_Start)
			WorkBag.Push(Down.Right,State_Start)
		}
		if pri == State_GetUse
		{
			isAllowTreeOpt = true
			if Down.GetType() != null
			{
				
				itemCall = new RetFuncParam(Down)
				defCase := Object^()
				regType := Type^()

				for iter : Down.Right.Down
				{
					if not (iter is BoxCase)
						continue
						
					if iter.Down.GetType() != null
					{
						b := new FuncInputBox() ; $temp 

						b.itPars.Emplace(Down.GetType(),Down.IsRef())
						b.itPars.Emplace(iter.Down.GetType(),iter.Down.IsRef())
						
						if (not iter.Down.IsConst) or regType? != iter.Down.GetType()
						{
							isAllowTreeOpt = false
						}
						
						regType = iter.Down.GetType()
						if regType? != GTypeInt and regType? != GTypeString
						{
							isAllowTreeOpt = false
						}

						func := BoxFunc^()
					
						mustSwapNodes := false
						func = FindFunc("<=>",this&,b^,false)
						if func == null
						{
							isAllowTreeOpt = false
							
							func = FindFunc("==",this&,b^,false)
							if func == null {
								c := new FuncInputBox() ; $temp 

								c.itPars.Emplace(iter.Down.GetType(),iter.Down.IsRef())
								c.itPars.Emplace(Down.GetType(),Down.IsRef())
								func = FindFunc("in",this&,c^,false)
								if func != null
								{
									mustSwapNodes = true					
								}
							}
	
							if func?.MyFuncType.RetType != GTypeBool
								EmitError("one of case compares is not boolean\n")
						}else{
							if func?.MyFuncType.RetType != GTypeInt
								EmitError("one of case <=> compares is not int\n")
						}

						if func == null
						{
							EmitError("can not compare case object\n")
						}else{
							newCall := new ParamNaturalCall("",itemCall->{Object^})

							newCall.Right = iter.Down
							iter.Down.Left = newCall
							iter.Down = newCall
							newCall.Up = iter

							if mustSwapNodes ForInSwapNodes(newCall)
							checkCall := MakeSimpleCall(func,iter.Down)
							iter.Down = checkCall
							checkCall.Line = this.Line
							checkCall.Up = iter
							
						}
					}else{
						asNeed := iter->{BoxCase^}
						if not asNeed.IsVoid{
							EmitError("bad case type\n")
						}else{
							defCase = asNeed
						}
					}
				}
				if defCase != null
				{
					if addedQ.Size() != 0
					{
						jName := StrCopy("SwitchVoid"sbt + id)
						for it : addedQ
						{
							it.jmpName = jName
						}
					}
				}				
			}else{
				EmitError("can not parse item at switch input\n")
			}

		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		
		things := List.{Object^}() ; $temp
		defThing := Object^()

		for iter : Down.Right.Down
		{
			if iter is BoxCase {
				asNeed := iter->{BoxCase^}
				if asNeed.IsVoid {
					defThing = iter
				}else{
					things.Push(iter)
				}
			}
		}
		Down.PrintInBlock(f)
	
		if isAllowTreeOpt and things.Size() > 2
		{
			fatArr := things.ToArray() ; $temp

			//TODO: replace on quick_sort
			for j : fatArr->len
			{
				for i :( fatArr->len - 1)
				{
					if SpaceCmpConstObjs(fatArr[i].Down.Down.Right,fatArr[i+1].Down.Down.Right) > 0
					{
						ttmp := fatArr[i]
						fatArr[i] = fatArr[i+1]
						fatArr[i+1] = ttmp
					}
				}
			}
			//qsort_switch_case(fatArr,0,fatArr->len - 1)
			//for fatArr
			//{
			//	it.Print(0)
			//}
			reqPrint := ( int start,int end) ==> {
				myPos := (start + end) div 2
				caseId := fatArr[myPos]->{BoxCase^}.caseId
				
				f << "CaseStart" << id << "w" << myPos << ":\n"
				fatArr[myPos].Down.PrintPre(f)
				f << "%CmpTest" << id << "_" << myPos << " = icmp eq i32 " << fatArr[myPos].Down.GetName() << " , 0\n"
				f << "br i1 %CmpTest" << id << "_" << myPos << " , label %Switch" << id << "true" << caseId
				f << ", label %Switch" << id << "false" << myPos << "\n"
				
				f << "Switch" << id << "false" << myPos << ":\n"				
				f << "%CmpGtTest" << id << "_" << myPos << " = icmp slt i32 " << fatArr[myPos].Down.GetName() << " , 0\n"				
				f << "br i1 %CmpGtTest" << id << "_" << myPos << " "
				
				if start != myPos{
					f << ", label %CaseStart" << id << "w" << ((start + myPos - 1) div 2)
				}else{
					f << ", label %SwitchVoid" << id 
				}
				
				if end != myPos{
					f << ", label %CaseStart" << id << "w" << ((end + myPos + 1) div 2)
				}else{
					f << ", label %SwitchVoid" << id 
				}
				f << "\n"
				f << "Switch" << id << "true" << caseId << ":\n"
				//fatArr[myPos].Down.PrintInBlock(f)
				for it : fatArr[myPos].Right
				{
					if it.GetValue() == "~case()"
					{
						break
					}
					it.PrintInBlock(f)
				}
				f << "br label %SwitchEnd" << id << "\n"
				
				if start != myPos{
					reqPrint(start,myPos - 1)
				}				
				if end != myPos{
					reqPrint(myPos + 1,end)
				}
			}
			f << "br label %CaseStart" << id << "w" << ((fatArr->len - 1) div 2) << "\n"
			reqPrint(0,fatArr->len - 1)
			f << "br label %SwitchVoid" << id << "\n"
			f << "SwitchVoid" << id << ":\n"
			if defThing != null
			{
				for iter : defThing.Right
				{
					if iter.GetValue() == "~case()"
					{
						break
					}
					iter.PrintInBlock(f)
				}
			}
			f << "br label %SwitchEnd" << id << "\n"
			f << "SwitchEnd" << id << ":\n"
			return void
		}
	
		for iter , i : things
		{
			caseId := iter->{BoxCase^}.caseId
			iter.Down.PrintPre(f)
			if iter.Down.GetType() == GTypeInt{
				f << "%CmpTest" << id << "_" << i << " = icmp eq i32 " << iter.Down.GetName() << " , 0\n"
				f << "br i1 %CmpTest" << id << "_" <<i << " , label %Switch" << id << "true" << caseId
			}else{
				f << "br i1 " << iter.Down.GetName() << " , label %Switch" << id << "true" << caseId
			}
			f << " , label %Switch" << id << "false" << i << "\n"
			f << "Switch" << id << "true" << caseId << ":\n"
			
			for iter2 : iter.Right
			{
				if iter2.GetValue() == "~case()"
				{
					break
				}
				iter2.PrintInBlock(f)
			}
			f << "br label %SwitchEnd" << id << "\n"
			f << "Switch" << id << "false" << i << ":\n"
		}
		f << "br label %SwitchVoid" << id << "\n"
		f << "SwitchVoid" << id << ":\n"
		if defThing != null
		{
			for iter : defThing.Right
			{
				if iter.GetValue() == "~case()"
				{
					break
				}
				iter.PrintInBlock(f)
			}
		}
		f << "br label %SwitchEnd" << id << "\n"
		f << "SwitchEnd" << id << ":\n"
	}
	GetOutPath := virtual !(Object^ objs, int typ, int size) -> string
	{
		if typ == PATH_CONTINUE //TODO: use size
		{	
			itr := objs
			while itr != null
			{
				if itr is BoxCase
					break
				if itr.Left != null
					itr = itr.Left
				else
					itr = itr.Up
			}
			assert(itr != null)
			itr = itr.Right
			while itr != null
			{
				if itr is BoxCase
					break
				itr = itr.Right
			}
			assert(itr != null)
			bs := itr->{BoxCase^}
			res := "Switch"sbt + id + "true" + bs.caseId
			return res.Str()
		}
		return Up.GetOutPath(this&,typ,size)
	}
	GetValue := virtual  !() -> string
	{
		return "~switch()"
	}
}
qsort_switch_case := !(Object^^ arr,int low,int hi) -> void
{
	i := low
	j := hi
	piv := (low + hi) div 2
	while i <= j
	{
		while SpaceCmpConstObjs(arr[i].Down.Down.Right,arr[piv].Down.Down.Right) < 0
		{
			i++
		}
			//		if SpaceCmpConstObjs(fatArr[i].Down.Down.Right,fatArr[i+1].Down.Down.Right) > 0
		while SpaceCmpConstObjs(arr[j].Down.Down.Right,arr[piv].Down.Down.Right) > 0
		{
			j--
		}
		if i <= j
		{
			tmp := arr[i]
			arr[i] = arr[j]
			arr[j] = tmp
			i++
			j--
		}
	}
	if j > low qsort_switch_case(arr,low,j)
	if i < hi qsort_switch_case(arr,i,hi)
}

BoxCase := class extend Object
{
	IsVoid := bool
	caseId := int
	this := !(Object^ itm) -> void
	{
		itrUp := itm
		while itrUp != null
		{
			if itrUp is BoxSwitch
			{
				bs := itrUp->{BoxSwitch^}
				caseId = bs.caseIdIter
				bs.caseIdIter += 1
				break
			}
			itrUp = itrUp.Up
			assert(itrUp != null) //TODO: case outside switch
		}

		Down = itm.Down
		Down.SetUp(this&)
		PopOutNode(Down)

		if Down is ObjIndent
		{
			asNeed := Down->{ObjIndent^}
			IsVoid = asNeed.MyStr == "void"
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			//WorkBag.Push(this&,State_PreGetUse)
			WorkBag.Push(Down,State_Start)
		}
		if pri == State_PreGetUse
		{
			//WorkBag.Push(Down,State_Start)
			//WorkBag.Push(Down,State_PreGetUse)
		}
	}
	GetValue := virtual !() -> string
	{
		return "~case()"
	}
}

