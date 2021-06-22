Screamer := class extend TGCObject
{
	Destroy := virtual !() -> void
	{
		printf("TOAST\n")
	}
}

TGCArray := class .{@T} extend TGCObject
{
	_arraySize := int
	this := !(int j) -> void
	{
		_arraySize = j
	}
	"[]" := !(int index) -> ref T
	{
		assert(index < _arraySize and index >= 0)
		return tempData[index]
	}
	Destroy := virtual !() -> void
	{
		printf("kinda\n")
		for i : _arraySize
			this[i] = null
	}
	tempData := T[100]
}

main := !(int argc, char^^ argv) -> int
{
	c := new TGCArray.{Screamer^}(13)
	printf("wut %p %p %p %p %p %p\n",c^[0],c^[1],c^[2],c^[3],c^[4],c^[5])
	c^[0] = new Screamer
	c^[1] = new Screamer 
	return 0
}
