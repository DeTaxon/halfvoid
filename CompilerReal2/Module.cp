Modules := List.{CompilerModule^}

CompilerModule := class extend Object
{
	GetItem := virtual !(string name) -> Object^
	{
		return null
	}
	GetFunc := virtual !(char^ name) -> Object^
	{
	}
	GetModuleType := virtual !(string name) -> Type^
	{
		return null
	}
	InitModule := virtual !() -> bool {
		return true
	}
	PrintGlobal := virtual !(TIOStream f) -> void {}
}
