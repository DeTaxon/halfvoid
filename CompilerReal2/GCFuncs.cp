
gcIncRefFunc := BoxFunc^
gcDecRefFunc := BoxFunc^

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
}
