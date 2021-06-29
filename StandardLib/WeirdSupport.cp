
internalGCMemClear := !(void^ ptr,size_t s) -> void
{
	//for compile mode this is a function
	//for JIT mode this it a pointer to function
	//too lazy to insert it inside compiler
	memset(ptr,0,s)
}

