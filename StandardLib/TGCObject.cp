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
		toInc := toIncP->{TGCObject^} ; $weak
		r := toInc.GetRef() //TODO: cant access RefConterValue directly
		toInc.SetRef(r + 1)
		printf("added\n")
	}
}
internalGCDecRef := !(void^ toDecP) -> void
{
	if toDecP != null{
		toDec := toDecP->{TGCObject^} ; $weak
		r := toDec.GetRef() //TODO: cant access RefConterValue directly
		toDec.SetRef(r - 1)
		//printf("old ref %i\n",r)
		if r == 1
		{
			//TODOGC: cycle check
			toDec.Destroy()
			delete toDec
		}	
	}
}