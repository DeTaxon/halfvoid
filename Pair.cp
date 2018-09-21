MakePair := !(@A a , @B b) -> Pair.{A,B}
{
	return Pair.{A,B}(a.b)
}

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

MakeHalfPair := !(@A a, @B b)
{
	return HalfPair.{A,B}(a,b)
}

HalfPair := class .{@Frst,@Scnd}
{
	first := Frst
	second := Scnd
	this := !(Frst a, Scnd b) -> void
	{
		first = a
		second = b
	}
	"=" := !(HalfPair.{Frst,Scnd} toSet) -> ref HalfPair.{Frst,Scnd}
	{
		first = toSet.first
		second = toSet.second
		return this
	}
	"==" := !(HalfPair.{Frst,Scnd} toChk) -> bool	{ return first == toChk.first	}
	">=" := !(HalfPair.{Frst,Scnd} toChk) -> bool	{ return first >= toChk.first	}
	"<=" := !(HalfPair.{Frst,Scnd} toChk) -> bool	{ return first <= toChk.first	}
	"<"  := !(HalfPair.{Frst,Scnd} toChk) -> bool	{ return first <  toChk.first	}
	">"  := !(HalfPair.{Frst,Scnd} toChk) -> bool	{ return first >  toChk.first	}
	"!=" := !(HalfPair.{Frst,Scnd} toChk) -> bool	{ return first != toChk.first	}

	"==" := !(Frst toChk) -> bool	{ return first == toChk	}
	">=" := !(Frst toChk) -> bool	{ return first >= toChk	}
	"<=" := !(Frst toChk) -> bool	{ return first <= toChk	}
	"<"  := !(Frst toChk) -> bool	{ return first <  toChk	}
	">"  := !(Frst toChk) -> bool	{ return first >  toChk	}
	"!=" := !(Frst toChk) -> bool	{ return first != toChk	}
}
