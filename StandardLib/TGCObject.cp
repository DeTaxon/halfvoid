TGCObject := class
{
	_GCRefCounterValue := int
	_GCIncRef := !() -> int { return InterlockAdd(_GCRefCounterValue,1) }
	_GCSubRef := !() -> int { return InterlockSub(_GCRefCounterValue,1) }
	_GCGetRef := !() -> int { return _GCRefCounterValue }
	_GCSetRef := !(int val) -> int { _GCRefCounterValue = val }
	Destroy := virtual !() -> void
	{
	}
	GCLastRef := virtual !() -> void
	{

	}
}
internalGCIncRef := !(void^ toIncP)  -> void
{
	if toIncP != null
	{
		toInc := weak toIncP->{TGCObject^}
		r := toInc._GCIncRef()
		printf("added %i\n",toInc._GCRefCounterValue)
	}
}
internalGCDecRef := !(void^ toDecP) -> void
{
	if toDecP != null{
		toDec := weak toDecP->{TGCObject^}
		//printf("old ref val %i\n",toDec._GCRefCounterValue)
		r := toDec._GCSubRef()
		printf("old ref %i\n",r)
		switch r
		{
			case 2
				toDec.GCLastRef()
			case 1
				//TODOGC: cycle check
				toDec.Destroy()
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