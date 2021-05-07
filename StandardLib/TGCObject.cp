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
internalZeroConst := 0
internalGCIncRef := !(void^ toIncP)  -> void
{
	if toIncP != null
	{
		toInc := weak toIncP->{TGCObject^}
		r := toInc.GCIncRef()
		//printf("added %i %p\n",toInc._GCRefCounterValue,toIncP)
	}
}
internalGCDecRef := !(void^ toDecP) -> void
{
	if toDecP != null{
		toDec := weak toDecP->{TGCObject^}
		//printf("old ref val %i %p\n",toDec._GCRefCounterValue,toDecP)
		r := toDec.GCSubRef()
		//printf("old ref %i\n",r)
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