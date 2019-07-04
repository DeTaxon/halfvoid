#import "Tree.cp"

CriticalAtomic := class extend Object
{
	itId := int
	this := !() -> void
	{
		itId = GetNewId()
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		f << "@Atomic" << itId << " = global i8 0\n"
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		f << "br label %StartAtomic" << itId << "\n"
		f << "StartAtomic" << itId << ":\n"
		f << "%PrevValue" << itId << " = atomicrmw xchg i8* @Atomic" << itId << " , i8 1 acquire\n"
		f << "%TestAtomic" << itId << " = icmp eq i8 %PrevValue" << itId << ",0\n"
		f << "br i1 %TestAtomic" << itId << " , label %EndAtomic" << itId << " , label %StartAtomic" << itId << "\n"
		f << "EndAtomic" << itId << ":\n"
	}
	PrintDestructor := virtual !(sfile f) -> void
	{
		f << "atomicrmw xchg i8* @Atomic" << itId << " , i8 0 acquire\n"
	}
}
