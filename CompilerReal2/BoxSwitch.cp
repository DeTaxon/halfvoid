#import "Tree.cp"

BoxSwitch := class extend Object
{
	id := int
	itemCall  := MemParam^
	this := !(Object^ itm) -> void
	{
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
		}
		if pri == State_PreGetUse
		{
			WorkBag.Push(this&,State_GetUse)
			WorkBag.Push(Down,State_Start)
			WorkBag.Push(Down.Right,State_Start)
		}
		if pri == State_GetUse
		{
			iter := Down.Right.Down
			
			if Down.GetType() != null
			{

				itemCall = new RetFuncParam(Down)

				while iter != null
				{
					if iter is BoxCase
					{
						if iter.Down.GetType() != null
						{
							b := new FuncInputBox() ; $temp 

							b.itPars.Emplace(Down.GetType(),Down.IsRef())
							b.itPars.Emplace(iter.Down.GetType(),iter.Down.IsRef())

							func := FindFunc("==",this&,b^,false)
							if func == null func = FindFunc("in",this&,b^,false)

							if func != null	{
								if func.MyFuncType.RetType != GTypeBool
									EmitError("one of case compares is not boolean\n")
							}

							if func == null
							{
								EmitError("can not compare case object\n")
							}else{
								newCall := new ParamNaturalCall("",itemCall->{Object^})

								newCall.Right = iter.Down
								iter.Down = newCall
								newCall.Up = iter

								checkCall := MakeSimpleCall(func,iter.Down)
								iter.Down = checkCall
								checkCall.Up = iter
								
							}
						}else{
							asNeed := iter->{BoxCase^}
							if not asNeed.IsVoid{
								EmitError("bad case type\n")
							}
						}
					}
					iter = iter.Right
				}
			}else{
				EmitError("can not parse item at switch input\n")
			}

		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		things := Queue.{Object^}()
		defThing := Object^
		defThing = null
		iter := Down.Right.Down

		while iter != null
		{
			if iter is BoxCase {
				asNeed := iter->{BoxCase^}
				if asNeed.IsVoid {
					defThing = iter
				}else{
					things.Push(iter)
				}
			}
			iter = iter.Right
		}
		Down.PrintInBlock(f)

		for iter , i : things
		{
			iter.Down.PrintPre(f)
			f << "br i1 " << iter.Down.GetName() << " , label %Switch" << id << "true" << i
			f << " , label %Switch" << id << "false" << i << "\n"
			f << "Switch" << id << "true" << i << ":\n"
			iter = iter.Right
			while iter != null
			{
				if iter.GetValue() == "~case()"
				{
					iter = null
				}else{
					iter.PrintInBlock(f)
					iter = iter.Right
				}
			}
			f << "br label %SwitchEnd" << id << "\n"
			f << "Switch" << id << "false" << i << ":\n"
		}
		if defThing != null
		{
			iter = defThing.Right
			while iter != null
			{
				if iter.GetValue() == "~case()"
				{
					iter = null
				}else{
					iter.PrintInBlock(f)
					iter = iter.Right
				}
			}
		}
		f << "br label %SwitchEnd" << id << "\n"

		f << "SwitchEnd" << id << ":\n"
	}
	GetOutPath := virtual !(Object^ objs, int typ, int size) -> string
	{
		return Up.GetOutPath(this&,typ,size)
	}
	GetValue := virtual  !() -> string
	{
		return "~switch()"
	}
}

BoxCase := class extend Object
{
	IsVoid := bool
	this := !(Object^ itm) -> void
	{
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

