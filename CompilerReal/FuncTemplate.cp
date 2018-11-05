FuncTemplate := class extend Object
{
	"this" := !(Object^ ob) -> void
	{
		
	}
	GetValue :=  virtual !() -> string
	{
		return "d{}()"
	}
}
