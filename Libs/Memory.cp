#import "MemoryPool.cp"

gTemporaryPool := StupidMemoryPool.{16000000}
FlushTempMemory := !() -> void
{
	gTemporaryPool.FlushMem()
}

"new" := !() . {@R} -> void^ 
{
	val := R->TypeSize
	if $temp {
		return gTemporaryPool.GetMem(val,R->Align)
	}
	newNode :=  malloc(val)
	memset(newNode,0,val)
	return newNode
}
"new" := !(int count) .{@R} -> R^
{
	val := R->FatTypeSize
	itSi := val*count
	if $temp {
		return gTemporaryPool.GetMem(itSi,R->Align)->{R^}
	}
	newNode := malloc(itSi)
	memset(newNode,0,itSi)
	return newNode->{R^}
}
"delete" := !(void^ item) .{@R[]} -> void
{
	sub := 4
	//if R->Align > 4 sub = R->Align
	//free(item - sub) //BUG: incorrect creating and deleting
}
