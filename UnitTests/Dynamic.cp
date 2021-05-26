
classSetTypeA := class
{
	x := int
	Pt := virtual !() -> void
	{
		x = 13
	}
	pt2 := virtual !() -> void
	{
		this->SetType(classSetTypeB)
	}
}
classSetTypeB := class extend classSetTypeA
{
	Pt := virtual !() -> void
	{
		x = 27
	}
	pt2 := virtual !() -> void
	{
		this&->SetType(classSetTypeA)
	}
}
TestSetType := !() -> void
{
	j := new classSetTypeA
	j.Pt()
	assert(j.x == 13)
	j->SetType(classSetTypeB)
	j.Pt()
	assert(j.x == 27)
	j.pt2()
	j.Pt()
	assert(j.x == 13)
	j.pt2()
	j.Pt()
	assert(j.x == 27)
}
