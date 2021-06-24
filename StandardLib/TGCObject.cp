internalGCIncRef := !(void^ toIncP)  -> void
{
	if toIncP == null
		return void

	toInc := weak toIncP->{TGCObject^}
	r := toInc.GCIncRef()
}
internalGCDecRef := !(void^ toDecP) -> void
{
	if toDecP == null
		return void

	toDec := weak toDecP->{TGCObject^}
	r := toDec.GCSubRef()
	switch r
	{
		case 2
			toDec.GCLastRef()
		case 1
			//TODOGC: cycle check
			toDec._GCInternalDestroy()
			toDec._GCInternalDelete()
			free(toDec)
	}
}
internalGCSetPtr := !(void^^ toSet, void^ val) -> void
{
	lft := weak toSet->{TGCObject^^}
	rg := weak val->{TGCObject^}
	if lft^ == rg
		return void
	internalGCIncRef(rg)
	internalGCDecRef(lft^)
	toSet^ = val
}
internalGCDestroyObj := !(void^ toDest) -> void //for created on stack, no dec ref
{
	obj := weak toDest->{TGCObject^}
	obj._GCInternalDestroy()
	obj._GCInternalDelete()
}

TGCObject := class
{
	_GCRefCounterValue := int
	GCIncRef := !() -> int { return InterlockAdd(_GCRefCounterValue,1) }
	GCSubRef := !() -> int { return InterlockSub(_GCRefCounterValue,1) }
	GCGetRef := !() -> int { return _GCRefCounterValue }
	GCSetRef := !(int val) -> int { _GCRefCounterValue = val }
	GCLastRef := virtual !() -> void {}
	
	Destroy := virtual !() -> void {}

	_GCInternalDelete := poison virtual !() -> void
	{
		for it : this->Fields
		{
			if it->Type->Base >= TGCObject
			{
				it = null
			}
			if it->Type >= TGCObject
			{
				it._GCInternalDelete()
			}
		}
	}
	_GCInternalDestroy := poison virtual !() -> void
	{
		for it : this->Fields
		{
			if it->Type >= TGCObject
			{
				it._GCInternalDestroy()
			}
		}
		Destroy()
	}
}

internalGCNewArray := !(int count) .{@R} -> R[]
{
	baseSize := R->FatTypeSize
	arrSize := TGCArray.{R}->TypeSize
	totalSize := arrSize + baseSize*(count - 1)
	preRet := calloc(1,totalSize)
	preRet->{void^^}^ = TGCArray.{R}->VTable
	preRet->{TGCArray.{R}^}."this"(count)
	return preRet&->{void^}->{R[]^}^
}
TGCArray := class .{@T} extend TGCObject
{
	_arraySize := int
	_arrayData := T[1]
	this := !(int itemCounter) -> void
	{
		_arraySize = itemCounter
	}
	"[]" := !(int index) -> ref T
	{
		assert(index < _arraySize and index >= 0)
		return _arrayData[index]
	}
	Destroy := virtual !() -> void
	{
		for i : _arraySize
			this[i] = null
	}
}

internalTGCArrayGetElement := !(@T[] arr,int index) -> ref T
{
	assert(arr != null)
	vd := arr->{void^}
	ptr := vd->{TGCArray.{T}^}
	return ptr^._arrayData[index]
}
