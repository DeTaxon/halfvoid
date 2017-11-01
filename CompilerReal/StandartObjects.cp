
ObjInt := class extend Object
{
	MyInt := s64
	"this" := !(int Value) -> void
	{
		MyInt = Value
		Clean()
	}
	Print := virtual !() -> void
	{
		printf("int %i",MyInt")
	}	
}
ObjDouble := class extend Object
{
	MyDouble := double
	"this" := !(double Value) -> void
	{
		//MyInt = Value
		Clean()
	}
	Print := virtual !() -> void
	{
		printf("double %f",MyDouble)
	}	
}
