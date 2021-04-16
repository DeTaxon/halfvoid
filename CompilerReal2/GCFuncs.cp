
gcIncRefFunc := BoxFunc^
gcDecRefFunc := BoxFunc^
gcSetValueFunc := BoxFunc^

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
