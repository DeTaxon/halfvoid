
gcIncRefFunc := BoxFunc^
gcDecRefFunc := BoxFunc^
gcSetValueFunc := BoxFunc^
gcCallDestroy := BoxFunc^

gcClass := BoxClass^

GCInit := !() -> void
{
	dummy := new Object ; $temp

	box := new FuncInputBox ; $temp
    box.itPars.Emplace(GTypeVoidP,false)

	gcIncRefFunc = FindFunc("internalGCIncRef",dummy,box^,false)
    if gcIncRefFunc == null
    {
        throw new Exception("Func internalGCIncRef not found")
    }
	gcIncRefFunc.ParseBlock()

	gcDecRefFunc = FindFunc("internalGCDecRef",dummy,box^,false)
    if gcDecRefFunc == null
    {
        throw new Exception("Func internalGCDecRef not found")
    }
	gcDecRefFunc.ParseBlock()

	gcCallDestroy = FindFunc("internalGCDestroyObj",dummy,box^,false)
    if gcCallDestroy == null
    {
        throw new Exception("Func internalGCDestroyObj not found")
    }
	gcCallDestroy.ParseBlock()

    
	box2 := new FuncInputBox ; $temp
    box2.itPars.Emplace(GTypeVoidP.GetPoint(),false)
    box2.itPars.Emplace(GTypeVoidP,false)
	gcSetValueFunc = FindFunc("internalGCSetPtr",dummy,box2^,false)
    if gcSetValueFunc == null
    {
        throw new Exception("Func internalGCSetPtr not found")
    }
	gcSetValueFunc.ParseBlock()
}

IsGCClass := !(Type^ toCmp) -> bool
{
	return toCmp is TypeClass and toCmp->{TypeClass^}.ToClass.IsGC
}
IsGCPtr := !(Type^ toCmp) -> bool
{
	return toCmp is TypePoint and toCmp.Base is TypeClass and toCmp.Base->{TypeClass^}.ToClass.IsGC
}

GCMakeAware := !(Object^ start, int allcId) -> void
{
	blk := GetBlock(start)
	if blk != null
	{
		blk.AddGC(allcId);
		AllocSetStruct(blk)
		blk.ApplyDeferUse(1)
	}
}
