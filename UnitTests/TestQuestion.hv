
TestQuestionPrt1 := !() -> void
{
	v := 13
	c := int^()
	c2 := v&
	for i : c?^
	{
		assert(false)
	}

	if c?^ == 0
	{
		assert(false)
	}
	if c?^ == 0
	{	
		assert(false)
	}else{
		v = 25
	}
	assert(v == 25)

	if c2^ == 25
	{
		v = -3
	}
	assert(v == -3)

	switch c?^
	{
		case 0..100
			assert(false)
		case void
			v = 103
	}
	assert(v == 103)

	switch c2?^
	{
		case 103
			v = 7
		case void 
			assert(false)
	}
	assert(v == 7)
}
TestQuestionPrt2 := !() -> void
{
	zero := int^()
	x := 0
	xp := x&

	zero?^ = 0 //excet crash
	xp?^ = 13
	assert(x == 13)

}
TestQuestionPrt3 := !() -> void
{
	a1 := AVLMap.{int,int}()
	a1[3] = 100
	a2 := AVLMap.{int,int}()
	a2[4] = 120
	sum := 0
	for i : 10
	{
		sum += a1.TryFind(i)?^ ?? a2.TryFind(i)?^ ?? 2
	}
	assert(sum == 236)

	tst2 := OnOddTrue
	tst2.x = 17
	assert(tst2[14] == 68)
	assert(tst2[14]? == 0)
	assert(tst2.x == 67)

	a1[3] = 5

	c := 0
	for i : a1[3]?
	{
		c += i
	}
	assert(c == 10)
	bSize := a1.Size()
	for i : a1[99999]?
	{
		assert(false)
	}
	assert(bSize == a1.Size())
}


TestQuestionPrt4 := !() -> void
{
	tst := List.{List.{int}}^()
	tst?.Start?.Data.Start?.Data = 1
}
TestQuestion := !() -> void
{
	TestQuestionPrt1()
	TestQuestionPrt2()
	TestQuestionPrt3()
	TestQuestionPrt4()
}

OnOddTrue := class
{
	x := int
	"[]?" := !(int ind) -> ref int
	{
		x = 67
		if ind != 13
			return null
		return x
	}
	"[]" := !(int ind) -> ref int
	{	
		x = 68
		return x
	}
}
globInt := int
retrVal := !(bool retNull) -> int^
{
	if retNull
		return null
	return globInt&
}
TestRetQ := !() -> int^
{
	h := 0
	defer assert(h == 26)
	return retrVal(true)?
	h = 26
	return retrVal(false)?
	assert(false)
}
