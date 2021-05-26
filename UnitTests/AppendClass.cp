
ToAppendClass := class 
{
	getValue := !() -> int
	{
		return getValue2()*13
	}
	extraProblem := fake
	{
		getValue3 := !() -> int
		{
			return 108
		}
		//recheck := poison virtual !() -> void TODO: support this test, poison + virtual
		//{
		//	printf("heh %s\n",this->Type->Name)
		// 	//this."ToAppendClass.extraProblem.getValue3"() //TODO support
		//}
		"[]" := !(int indx) -> int
		{
			return 15
		}
	}
}
AppendClass ToAppendClass
{
	y := int
	r := int
	getValue2 := !() -> int
	{
		return x
	}
	extraProblem := fake
	{
		CheckValue := !() -> void
		{
			assert(this.extraProblem.getValue3() == 108)
		}
		
		"[]?" := !(int indx) -> ref int
		{
			r = 16
			return r
		}
	}
}

AppendClass ToAppendClass
{
	x := int
	getValue3 := !() -> int
	{
		return y
	}
}

AppendClassTest := !() -> void
{
	tstObj := ToAppendClass
	tstObj.x = 3
	tstObj.y = 4
	retVal := tstObj.getValue()
	assert(retVal == tstObj.x*13)
	assert(tstObj.getValue3() == 4)
	assert(tstObj.extraProblem.getValue3() == 108)
	assert(tstObj.extraProblem[0] == 15)
	assert(tstObj.extraProblem[0]? == 16)
	//tstObj.extraProblem.CheckValue()
}

