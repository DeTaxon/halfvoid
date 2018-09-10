Pair := class .{@Frst,@Scnd}
{
	first := Frst
	second := Scnd
	this := !(Frst f1,Scnd f2) -> void
	{
		first = f1
		second = f2
	}
	"=" := !(Pair.{Frst,Scnd} toSet) -> ref Pair.{Frst,Scnd}
	{
		first = toSet.first
		second = toSet.second
		return this
	}
}
