Type_Standart :: 1
Type_Point :: 2
Type_Fixed :: 3
Type_FuncP :: 4


Type := class {
	Id := int
	Base := Type^
	IRName := char^
	ItType := int
	
	"this" := !() -> void
	{
		Base = null
		IRName = null
		ItType = 0
	}
}
