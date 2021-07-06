
TestSpaceshipTuple := !() -> void
{
	assert(!{1,1} <=> !{1,1} == 0)
	assert(!{2,1} <=> !{1,1} == 1)
	assert(!{1,1} <=> !{2,1} < 0)
	assert(!{1,2} <=> !{2,1} < 0)
	assert(!{2,1} <=> !{1,2} > 0)

	assert(!{1,1} == !{1,1})
	assert(!{2,1} > !{1,1})
	assert(!{1,1} < !{2,1})
	assert(!{1,2} < !{2,1})
	assert(!{2,1} > !{1,2})
}
TestSpaceship := !() -> void
{
	someArr := int[3]
	ptr1 := someArr[0]&
	ptr2 := someArr[1]&
	assert(ptr1 <=> ptr2 < 0)
	assert(ptr1 <=> ptr1 == 0)
	assert(ptr2 <=> ptr1 == 1)
	//return 0
	assert(false <=> false == 0 )
	assert(false <=> true > 0)
	assert(true <=> false < 0)
	assert(true <=> true == 0)
}
