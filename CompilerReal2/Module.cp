Modules := Queue.{CompilerModule^}

CompilerModule := class
{
	GetItem := virtual !(string name) -> Object^
	{
		return null
	}
	GetModuleType := virtual !(string name) -> Type^
	{
		return null
	}
	InitModule := virtual !() -> bool {
		return true
	}
}
