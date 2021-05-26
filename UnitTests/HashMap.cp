
TestHashMap := !() -> void
{
	tst := HashAVLMap.{char^,int}()
	tst["hello"] = 1
	tst["world"] = 1
	tst["azb"] = 1
	tst["zb"] = 1
	tst["world"] = 2
	tst["1world"] = 2
	tst["2world"] = 2
	tst["2world"] = 3
	assert(tst.Size() == 6)
}
