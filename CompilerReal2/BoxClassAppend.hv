
BoxClassAppend := class extend Object
{
	className := char^
	classPtr := BoxClass^
	this := !() -> void
	{
	}
	GetItem := virtual !(char^ name) -> Object^
	{
		return classPtr.GetItem(name)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if visitedWork[pri] return void
		else visitedWork << pri

		if pri == State_Start
		{
			WorkBag.Push(this&,State_CheckTypes)
		}
		if pri == State_CheckTypes
		{
			if not( Down.Right is ObjIndent)
				EmitError("Class name is not indentifier")
			
			className = Down.Right->{ObjIndent^}.MyStr
			PopOutNode(Down)
			PopOutNode(Down)
			MakeItBlock(Down)
			WorkBag.Push(Down,State_Start)

			prm := ParseType(new ObjIndent(className)) ; $temp //TODO: optimize
			if prm == null 
				EmitError("Class with name "sbt + className + " not found")
			if not prm is TypeClass
				EmitError("Class with name "sbt + className + " is not a class")
			classPtr = prm->{TypeClass^}.ToClass
			classPtr.appendObjects.Push(this&)
		}
	}
}

