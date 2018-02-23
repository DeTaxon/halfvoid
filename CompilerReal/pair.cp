pair := class !{T1,T2}
{
	first := T1
	second := T2

	this := !() -> void
	{
	}
	this := !(T1 t1, T2 t2) -> void
	{
		first = t1
		second = t2
	}

	"==" := !(pair.{T1,T2} toCmp) -> bool
	{
		if first == toCmp.first return true
		return second == toCmp.second
	}
	"=" := !(pair.{T1.T2} toSet) -> bool
	{
		first = toSet.first
		second = toSet.second
	}
}
