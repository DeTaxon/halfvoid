FuncCallOperator := !(Object^ iter) -> Object^
{
	if IsOper(iter.GetValue())
	{
		oper := iter.GetValue()
		if iter.Right == null
		{
			iter = iter.Left
			PopOutNode(iter.Right)
			return OperFunc(oper,iter.Up.Down,iter) //OneCall(oper,iter.Up,true)

		}else
		{
			iter = iter.Right

			if iter.Right != null
			{

				oName := ""sbt + oper

				if IsOper(iter[^].GetValue())
				{
					oName << " " << it.GetValue()
				}
				bigOper := oName.Str() ; $temp
				if bigOper == "?: :" and iter.Right.Right != null
				{
					strt := iter.Left.Left
					typA := strt.GetType()
					if typA == null or TypeCmp(typA,GTypeBool) == 255
						strt.EmitError("Incorrect boolean type for ?:")
					typB := iter.GetType()
					if typB == null
						strt.EmitError("Incorrect first input for ?:")
					typC := iter.Right.Right.GetType()
					if typC == null
						strt.EmitError("Incorrect second input for ?:")
					resType := TypeFight(typB,typC)
					if resType == null
						strt.EmitError("Types can not be united for ?:")
					return new TrinaryOper(strt,resType)
				}
				iter.Up.Print(0)
				iter.EmitError("no more then binary allowed \n")
				return null
			} 
			if iter.GetType() != null
			{
				operNode := iter.Left
				PopOutNode(iter.Left)
				if oper == "in" and iter.Up != null
				{
					ForInSwapNodes(iter.Left)
				}
				return  OperFunc(oper,iter.Up.Down,operNode) //OneCall(oper,iter.Up)
			}
			return null
		}
	}
	return null
}
FuncCallPrefixOperator := !(Object^ iter) -> Object^
{
	if iter.GetValue() == "delete"
	{
		delCall := new DeleteCall(iter.Right)
		delCall.Line = iter.Line
		assert(delCall.Line != null)
		return delCall
	}
	if iter.GetValue() == "new"
	{
		useType := ParseType(iter.Right)

		if useType == null
		{
			ErrorLog.Push("Incorrect new type\n")
			return null
		}
		if iter.Right.Right != null
		{
			if iter.Right.Right.GetValue() != "()"
			{
				irr := iter.Right.Right
				itB := new FuncInputBox()  ; $temp
				itB.itPars.Emplace(irr.GetType(),irr.IsRef())
				itB.itConsts.Push(new ObjType(useType)) 
				
				FillAttrs(itB^,iter)

				func := FindFunc("new",iter,itB^,true)
				if func != null
				{
					irr.Up.Down = irr
					itF := MakeSimpleCall2(func,irr,iter)
					return itF
				}
			}
			if iter.Right.Right.GetValue() == "()" 
			{
				preRet :=  new NewCallOne(useType,iter.Right.Right) 
				preRet.Line = iter.Line
				preRet.appendTemp = iter.Up?.Left?.GetValue() == "throw"
				return preRet
			}

			//return new NewCall(useType,iter.Right.Right)
		}else{
			preRet := new NewCallOne(useType,null->{Object^})
			preRet.Line = iter.Line
			preRet.appendTemp = iter.Up?.Left?.GetValue() == "throw"
			return preRet
		}
	}else{
		if IsOper(iter.GetValue())
		{
			if iter.Right?.Right == null
			{
				name := ". "sbt + iter.GetValue()
				iter = iter.Right
				PopOutNode(iter.Left)
				return OneCall(name,iter.Up,null->{Object^})
			}
		}
	}
	if iter is ObjIndent //iter.GetValue() == "~ind"
	{
		if iter.Right == null return null

		if iter.Right.GetValue() == "()"
		{
			asInd := iter->{ObjIndent^}
			itName := asInd.MyStr

			return OneCall(itName,iter.Right,null->{Object^})
		}
	}
	return null
}
