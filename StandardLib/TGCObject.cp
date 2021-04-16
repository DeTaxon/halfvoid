TGCObject := class
{
	RefConterValue := int
	GetRef := !() -> int
	{
		return RefConterValue
	}
	SetRef := !(int val) -> int
	{
		RefConterValue = val
	}
	Destroy := virtual !() -> void
	{
	}
}
internalGCIncRef := !(void^ toIncP)  -> void
{
	if toIncP != null
	{
		toInc := weak toIncP->{TGCObject^}
		r := toInc.GetRef() //TODO: cant access RefConterValue directly
		toInc.SetRef(r + 1)
	}
}
internalGCDecRef := !(void^ toDecP) -> void
{
	if toDecP != null{
		toDec := weak toDecP->{TGCObject^}
		r := toDec.GetRef() //TODO: cant access RefConterValue directly
		toDec.SetRef(r - 1)
		if r == 1
		{
			//TODOGC: cycle check
			toDec.Destroy()
			delete toDec
		}	
	}
}