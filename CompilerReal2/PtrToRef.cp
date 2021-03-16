PtrToRef := class extend ObjResult
{
	this := !(Object^ toReplace) -> void
	{
		ResultType = toReplace.GetType().Base
		temp := toReplace
		ReplaceNode(toReplace,this&)
		Down = temp
		Down.Up = this&
		inhAttrs = toReplace.inhAttrs
	}
	PrintPointPre := virtual !(TIOStream f) -> void
	{
		Down.PrintPre(f)
	}
	PrintPointUse := virtual !(TIOStream f) -> void
	{
		Down.PrintUse(f)
	}
	PrintUse := virtual !(TIOStream f) -> void
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
