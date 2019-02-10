#import "MemoryPool.cp"

gTemporaryPool := thread_local StupidMemoryPool.{16000000}
FlushTempMemory := !() -> void
{
	gTemporaryPool.FlushMem()
}

TempAsked := int
RealAsked := int

PrintMemUse := !() -> void
{
	printf("new temp called %i , new malloc called %i\n",TempAsked,RealAsked)
}

"new" := !() . {@R} -> void^ 
{
	val := R->TypeSize
	if $temp {
		TempAsked++
		return gTemporaryPool.GetMem(val,R->Align)
	}
	RealAsked++
	newNode :=  malloc(val)
	memset(newNode,0,val)
	return newNode
}
"new" := !(int count) .{@R} -> R[]
{
	val := R->FatTypeSize
	itSi := val*count
	itAl := R->Align
	if itAl < 4 itAl = 4
	itSi += itAl + 16
	newNode := null->{u8^}
	if $temp {
		TempAsked++
		newNode = gTemporaryPool.GetMem(itSi,itAl)->{u8^}
	}else{
		RealAsked++
		newNode = malloc(itSi)->{u8^}
		memset(newNode,0,itSi)
	}
	preRet := newNode[itAl]&->{R[]}
	preRet->{int^}[-1] = count	
	return preRet
}
"delete" := !(void^ item) .{@R[]} -> void
{
	sub := 4
	//if R->Align > 4 sub = R->Align
	//free(item - sub) //BUG: incorrect creating and deleting
}
