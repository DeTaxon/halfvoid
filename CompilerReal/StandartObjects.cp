
ObjInt := class extend Object
{
	MyInt := s64
	"this" := !(int Value) -> void
	{
		MyInt = Value
	}
	Print := virtual !() -> void
	{
		printf("int %i",MyInt")
	}	
}
