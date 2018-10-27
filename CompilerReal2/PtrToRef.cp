PtrToRef := class extend ObjResult
{
	this := !(Object^ toReplace) -> void
	{
		ResultType = toReplace.GetType().Base
		temp := toReplace
		ReplaceNode(toReplace,this&)
		Down = temp
		Down.Up = this&
	}
	PrintPointPre := virtual !(sfile f) -> void
	{
		Down.PrintPre(f)
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
		Down.PrintUse(f)
	}
	PrintUse := virtual !(sfile f) -> void
	{
		f << "PtrToRef wip"
	}
	GetPointName := virtual !() -> string
	{
		return Down.GetName()
	}
	GetName := virtual !() -> string
	{
		return "PtrToRef wip"
	}
	GetOutputName := virtual !() -> string
	{
		return "PtrToRef wip"
	}
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
	GetValue := virtual !() -> string
	{
		return "d^->ref"
	}
}
